#include <Animate.h>

#ifndef MATHHELPERS_H
#include <MathHelpers.h>
#endif


Animate::Animate() {}

AnimationUtils Animate::aUtilAnimate = AnimationUtils();


void Animate::animateMeteor(Meteor* meteor)
{
	// Set variables from meteor struct
	CRGB* strip = meteor->rStrip;
	int region = meteor->region;
	int regionLength = meteor->regionLength;
	CHSV pColor = meteor->pColor;
	int meteorSize = meteor->meteorSize;
	bool hasTail = meteor->hasTail;
	bool meteorRandomDecay = meteor->meteorRandomDecay;
	int tailHueStart = meteor->tailHueStart;
	bool tailHueAdd = meteor->tailHueAdd;
	double tailHueExponent = meteor->tailHueExponent;
	int tailHueSaturation = meteor->tailHueSaturation;
	int beginPixel = meteor->firstPixel;

	CHSV pRepeatColor = pColor; // Initalize repeat color to meteor color
	int hue = tailHueStart;

	// Calculations are multiplied by two for pixel doubling on frontside/backside LED pairs on stick

	// Calculate tail length
	int tailStartBrightness = tailBrightnessMap[0];
	int tailBrightnessMax = tailStartBrightness + 8;
	int tailLength = tailBrightnessMapLength;
	int tailSection = tailStartBrightness / tailLength;
	int halfTailSection = tailSection * 0.5;
	int adjustedLength = ((meteorSize + tailLength) * 2); // Length of animation for both sides of LED strip

	// First pixel of each region
	int startPixel = meteor->directionDown == true ?
		((region + 1) * regionLength) - 1 : startPixel = region * regionLength;

	// Current pixel to draw
	int drawPixel = meteor->directionDown == true
		? startPixel - beginPixel
		: drawPixel = startPixel + beginPixel;

	// Calculate region start and end
	int regionStart = regionLength * region; // Calculate the pixel before the region start
	int regionEnd = (regionLength * (region + 1) - 1); // Calculate the pixel after the region end

	bool endTail = false; // Flag to end tail


	// Draw every LED based on meteor size and tail length
	for (int d = 1; d < (adjustedLength) + 2; d++) {
		
		// Set current pixel to draw
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

		// Draw backside LED
		if (d - 1 % 2 == 0) {
			strip[currentPixel] = pRepeatColor; // Set pixel to match frontside LED
			continue;
		}

		// Draw meteor core
		if (d < (meteorSize * 2) + 1) {
			strip[currentPixel] = pColor; // Set pixel color
			continue;
		}

		// No tail, so turn off pixel
		if (hasTail == false) {
			if (d > (meteorSize * 2) - 1) {
				strip[currentPixel] = CHSV(0, 0, 0);
				continue;
			}
		}

		// If end tail has been flagged, end the loop
		if (endTail == true) {
			strip[currentPixel] = CHSV(0, 0, 0); // Turn off pixel
			break;
		}

		// Set tail brightness
		int tailBrightness;
		if (d < 0 || d > (tailBrightnessMapLength * 2) - 1) {
			// This LED is out of bounds, turn off pixel
			strip[currentPixel] = CHSV(0, 0, 0);
			continue;
		} else {
			// Look up tail brightness from map
			tailBrightness = d % 2 == 1
				? tailBrightnessMap[(d - 1) / 2] // This LED is odd
				: tailBrightnessMap[d / 2]; // This LED is even
		}

		
		if (meteorRandomDecay == true) {
			// Calculate random variance
			uint8_t tailNumberVarianceCalc = (d + 2) * 4;
			tailBrightness += random8(tailNumberVarianceCalc) - (tailNumberVarianceCalc - 8);
		}

		// Clamp brightness to 8-190 via min/max
		uint8_t brightValue = std::min((int)tailBrightnessMax, std::max(8, (int)tailBrightness));
		
		// Turn off tail after tailSectionLength pixels
		if (d > (tailBrightnessMapLength * 2) + 4) brightValue = 0;

		// Adjust brightness for gamma correction
		uint8_t brightCalc = dim8_video(brightValue); // dim8_video floor is 1

		// Create tail color color object
		CHSV tailColor = CHSV(hue, tailHueSaturation, brightCalc);


		// Make sure the pixel right after the meteor will get drawn so meteor values aren't repeated
		if (d < (meteorSize + 1)) {
			strip[currentPixel] = tailColor;
			continue;
		}

		strip[currentPixel] = tailColor;

		pRepeatColor = tailColor;

		if (brightValue == 0) {
			// endTail = true;
			continue;
		}

		// if (brightValue == 0) endTail == true;
	
	} // End for loop
}