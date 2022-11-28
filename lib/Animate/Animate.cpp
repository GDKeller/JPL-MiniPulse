#include <Arduino.h>
#include <Animate.h>
#include <AnimationUtils.h>
#include <Adafruit_NeoPixel.h>

Animate::Animate() {}

AnimationUtils Animate::aUtilAnimate = AnimationUtils();

void Animate::animateMeteor(Meteor* meteor)
{
	Adafruit_NeoPixel *&strip = meteor->rStrip;
	int region = meteor->region;
	int regionLength = meteor->regionLength;
	uint32_t *pColor = meteor->pColor; 
	int meteorSize = meteor->meteorSize;
	int meteorTrailDecay = meteor->meteorTrailDecay;
	bool meteorRandomDecay = meteor->meteorRandomDecay;
	int tailHueStart = meteor->tailHueStart;
	bool tailHueAdd = meteor->tailHueAdd;
	double tailHueExponent = meteor->tailHueExponent;
	int tailHueSaturation = meteor->tailHueSaturation;
	int beginPixel = meteor->firstPixel;


	int hue = degreeToSixteenbit(tailHueStart);
	int startPixel = region * regionLength; // First pixel of each region
	int drawPixel = startPixel + beginPixel;		  // Current pixel to draw

	int regionStart = (regionLength * region) - regionLength - 1; // Calculate the pixel before the region start
	int regionEnd = regionLength * (region + 1);							  // Calculate the pixel after the region end

	// Draw every LED in entire region 
	for (int d = 1; d < regionLength + 1; d++)
	{
		int currentPixel = drawPixel - d - 1;

		if (currentPixel < regionStart) continue;
		if (currentPixel >= regionEnd) continue;

		// Draw meteor
		if (d < meteorSize + 1)
		{
			aUtilAnimate.setPixelColor(*strip, currentPixel, pColor);
			continue;
		}

		// Draw tail
		int satExpo = ceil(tailHueSaturation * log(d + 1)); // Calculate logarithmic growth
		satExpo += random(32) - 16;							// Add random variance to saturation
		int satValue = satExpo > tailHueSaturation ? tailHueSaturation : (satExpo < 0 ? 0 : satExpo);
		int brightExpo = ceil(255 * mPower(0.85, d)); // Calculate exponential decay
		brightExpo += random(32) - 16;				  // Add randomvariance to brightness
		int brightValue = brightExpo > 255 ? 255 : (brightExpo < 0 ? 0 : brightExpo);
		int brightValueMap = map(brightValue, 0, 255, 0, AnimationUtils::brightness);
		int randVal = (4 * d) * (4 * d);
		int hueRandom = hue + (random(randVal) - (randVal / 2));
		uint32_t trailColor = strip->ColorHSV(hueRandom, satValue, brightValue);
		uint32_t *pTrailColor = &trailColor;

		// Cycle hue through time
		tailHueAdd == true ? hue += ceil(4096 * mPower(tailHueExponent, d)) : hue -= ceil(4096 * mPower(tailHueExponent, d));

		// Make sure the pixel right after the meteor will get drawn so meteor values aren't repeated
		if (d < (meteorSize + 2))
		{
			aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
			continue;
		}

		// Roll the dice on showing each pixel for a "fizzle" effect
		if (random(10) < 5)
			aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
	}
}