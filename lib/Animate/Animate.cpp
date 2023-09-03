#include <Animate.h>

Animate::Animate() {}

AnimationUtils Animate::aUtilAnimate = AnimationUtils();

void Animate::animateMeteor(Meteor* meteor)
{
	CRGB* strip = meteor->rStrip;
	int region = meteor->region;
	int regionLength = meteor->regionLength;
	CHSV pColor = meteor->pColor; 
	int meteorSize = meteor->meteorSize;
	bool hasTail = meteor->hasTail;
	bool meteorTrailDecay = meteor->meteorTrailDecay;
	float meteorTrailDecayValue = meteor->meteorTrailDecayValue;
	bool meteorRandomDecay = meteor->meteorRandomDecay;
	int tailHueStart = meteor->tailHueStart;
	bool tailHueAdd = meteor->tailHueAdd;
	double tailHueExponent = meteor->tailHueExponent;
	int tailHueSaturation = meteor->tailHueSaturation;
	int beginPixel = meteor->firstPixel;

	CHSV pRepeatColor = pColor;
	int hue = tailHueStart;

	// First pixel of each region
	int startPixel = meteor->directionDown == true ?
		((region + 1) * regionLength) -1 : startPixel = region * regionLength;

	// Current pixel to draw
	int drawPixel = meteor->directionDown == true ?
		startPixel - beginPixel : drawPixel = startPixel + beginPixel;

	int regionStart = regionLength * region; // Calculate the pixel before the region start
	int regionEnd = (regionLength * (region + 1) - 1); // Calculate the pixel after the region end

	// Draw every LED in entire region 
	for (int d = 1; d < (regionLength/2) + 1; d++)
	{
		int currentPixel;
		if (meteor->directionDown == true) {
			currentPixel = drawPixel + d + 1;
			if (currentPixel > regionEnd) break;
			if (currentPixel < regionStart) continue;
		} else {
			currentPixel = drawPixel - d - 1;
			if (currentPixel < regionStart) break;
			if (currentPixel > regionEnd) continue;
		}

		if (d - 1 % 2 == 0) {
			// aUtilAnimate.setPixelColor(*strip, currentPixel, pRepeatColor);
			strip[currentPixel] = pRepeatColor;
			continue;
		}

		// Draw meteor
		if (d < (meteorSize * 2) + 1)
		{
			// aUtilAnimate.setPixelColor(*strip, currentPixel, pColor);
			strip[currentPixel] = pColor;
			continue;
		}

		if (hasTail == false) {
			if (d > (meteorSize * 2) - 1) // Multiply by two for pixel doubling on inward/outward LED pairs on stick
			{
				strip[currentPixel] = CHSV(0, 0	, 0);
				continue;
			}
		}


		// Draw tail
		uint8_t brightValue;
		CHSV trailColor;
		if (meteorTrailDecay == true) {
			int satExpo = ceil(tailHueSaturation * log(d + 1)); // Calculate logarithmic growth
			satExpo += random(32) - 16;							// Add random variance to saturation
			uint8_t satValue = satExpo > tailHueSaturation ? tailHueSaturation : (satExpo < 0 ? 0 : satExpo);
			int brightExpo = ceil(255 * mPower(meteorTrailDecayValue, d)); // Calculate exponential decay
			brightExpo += random(32) - 16;				  // Add randomvariance to brightness
			uint8_t brightValue = brightExpo > 255 ? 255 : (brightExpo < 0 ? 0 : brightExpo);
			int brightValueMap = map(brightValue, 0, 255, 0, AnimationUtils::brightness);
			int randVal = (4 * d) * (4 * d);
			int hueRandom = hue + (random(randVal) - (randVal / 2));
			trailColor = CHSV(hueRandom, satValue, brightValue);
		} else {
			// int satCalc = tailHueSaturation - (d * 16);
			// satCalc += random(32) - 16;
			// uint8_t satValue = satCalc < 0 ? 0 : satCalc;
			int satValue = tailHueSaturation;
			int brightCalc = 255 - (d * 16);
			// brightCalc += random(32) - 16;
			brightValue = brightCalc < 0 ? 0 : brightCalc;
			trailColor = CHSV(hue, satValue, brightValue);
		}

		// Cycle hue through time
		// tailHueAdd == true ? hue += ceil(4096 * mPower(tailHueExponent, d)) : hue -= ceil(4096 * mPower(tailHueExponent, d));

		// Make sure the pixel right after the meteor will get drawn so meteor values aren't repeated
		if (d < (meteorSize + 1))
		{
			// aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
			strip[currentPixel] = trailColor;
			continue;
		}

		// Roll the dice on showing each pixel for a "fizzle" effect
		if (meteorRandomDecay == true) {
			if (random(10) < 5) strip[currentPixel] = trailColor;
		} else {
			strip[currentPixel] = trailColor;
		}

		pRepeatColor = trailColor;

		if (brightValue == 0) continue;
	}
}
