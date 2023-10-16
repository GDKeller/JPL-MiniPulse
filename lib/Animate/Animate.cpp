#include <Animate.h>

#ifndef MATHHELPERS_H
#include <MathHelpers.h>
#endif


Animate::Animate() {}

AnimationUtils Animate::aUtilAnimate = AnimationUtils();


void Animate::animateMeteor(Meteor* meteor)
{
	int tailStartBrightness = tailBrightnessMap[0];

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

	int tailLength = tailBrightnessMapLength;
	int tailSection = tailStartBrightness / tailLength;
	int halfTailSection = tailSection * 0.5;
	int adjustedLength = (meteorSize + tailLength) * 2;

	// First pixel of each region
	int startPixel = meteor->directionDown == true ?
		((region + 1) * regionLength) - 1 : startPixel = region * regionLength;

	// Current pixel to draw
	int drawPixel = meteor->directionDown == true ?
		startPixel - beginPixel : drawPixel = startPixel + beginPixel;

	int regionStart = regionLength * region; // Calculate the pixel before the region start
	int regionEnd = (regionLength * (region + 1) - 1); // Calculate the pixel after the region end

	bool endTail = false;


	// Draw every LED in entire region 
	for (int d = 1; d < (adjustedLength)+1; d++) {
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
		if (d < (meteorSize * 2) + 1) {
			// aUtilAnimate.setPixelColor(*strip, currentPixel, pColor);
			strip[currentPixel] = pColor;
			continue;
		}

		if (hasTail == false) {
			if (d > (meteorSize * 2) - 1) // Multiply by two for pixel doubling on inward/outward LED pairs on stick
			{
				strip[currentPixel] = CHSV(0, 0, 0);
				continue;
			}
		}

		if (endTail == true) {
			strip[currentPixel] = CHSV(0, 0, 0);
			break;
		}

		// Draw tail
		uint8_t brightValue;
		CHSV trailColor;
		// if (meteorTrailDecay == true) {
		// 	// You can adjust the calculations for brightValue here to control the rate of decay
		// 	brightValue = 255 * pow(meteorTrailDecayValue, d);
		// 	// Ensure brightValue does not exceed 255 or go below 0
		// 	brightValue = brightValue > 255 ? 255 : (brightValue < 0 ? 0 : brightValue);
		// 	trailColor = CHSV(hue, tailHueSaturation, brightValue);
		// } else {
		// 	int brightCalc = 255 - (d * 16);
		// 	brightValue = brightCalc < 0 ? 0 : brightCalc;
		// 	trailColor = CHSV(hue, tailHueSaturation, brightValue);
		// }

		// meteorTrailDecayValue = std::max(0.97, meteorTrailDecayValue + 0.01);


		if (meteorTrailDecay == true) {
			// int satExpo = ceil(tailHueSaturation * log(d + 1)); // Calculate logarithmic growth
			// satExpo += random(32) - 16;							// Add random variance to saturation
			// uint8_t satValue = satExpo > tailHueSaturation ? tailHueSaturation : (satExpo < 0 ? 0 : satExpo);
			// int brightExpo = ceil(tailStartBrightness * MathHelpers::mPower(meteorTrailDecayValue, d)); // Calculate exponential decay
			// int brightExpo = 255 - (d * random8(8, 32));
			// int brightExpo = 255 / ((d + 2) );
			// const int tailBrightness = brightExpo;

			// This version calculate brightness based on defined tail length
			// int tailBrightness = tailStartBrightness - round((tailSection * d) * 0.5);

			int tailBrightness;
			if (d >= 0 && d < tailBrightnessMapLength * 2) {
				if (d % 2 == 1) { // Check if d is odd
					tailBrightness = tailBrightnessMap[(d - 1) / 2];
				} else {
					tailBrightness = tailBrightnessMap[d / 2];
				}
			} else {
				tailBrightness = 0;
			}

			// tailBrightness += (random8(tailSection) - (halfTailSection));	// Add random variance to brightness
			tailBrightness += (random8(16) - 8);	// Add random variance to brightness

			// uint8_t brightValue = tailBrightness > 255 ? 255 : (tailBrightness < 0 ? 0 : tailBrightness);
			uint8_t brightValue = std::min(tailStartBrightness, std::max(0, tailBrightness)); // Clamp brightness to 8-190 via min/max
			// if (brightValue < 4) brightValue = 0;

			// int randVal = (4 * d) * (4 * d); // Calculate random variance
			// int hueRandom = hue + (random(randVal) - (randVal / 2));
			// brightValue = 255;
			// Serial.println(brightValue);

			uint8_t brightCalc = dim8_video(brightValue);
			trailColor = CHSV(hue, tailHueSaturation, brightCalc);
		} else {
			// int satCalc = tailHueSaturation - (d * 16);
			// satCalc += random(32) - 16;
			// uint8_t satValue = satCalc < 0 ? 0 : satCalc;
			// int satValue = tailHueSaturation;
			// int brightCalc = 255 - (d * 16);
			// brightCalc += random(32) - 16;
			// brightValue = brightCalc < 0 ? 0 : brightCalc;
			// trailColor = CHSV(hue, tailHueSaturation, 255);
			trailColor = CHSV(0, 0, 128);
		}

		// Cycle hue through time
		// tailHueAdd == true ? hue += ceil(4096 * mPower(tailHueExponent, d)) : hue -= ceil(4096 * mPower(tailHueExponent, d));

		// Make sure the pixel right after the meteor will get drawn so meteor values aren't repeated
		if (d < (meteorSize + 1)) {
			// aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
			strip[currentPixel] = trailColor;
			continue;
		}

		// Roll the dice on showing each pixel for a "fizzle" effect
		if (meteorRandomDecay == true) {
			if (random8(10) < 5) strip[currentPixel] = trailColor;
		} else {
			strip[currentPixel] = trailColor;

			pRepeatColor = trailColor;

			if (brightValue == 0) continue;
		}
		if (brightValue == 0) endTail == true;
		// if (meteorTrailDecay == true)
		// 	strip[currentPixel].fadeToBlackBy(d * 8);
	} // End for loop
}