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
	uint8_t region = meteor->region;
	uint8_t regionLength = meteor->regionLength;
	CHSV pColor = meteor->pColor;
	int8_t meteorSize = meteor->meteorSize;
	bool hasTail = meteor->hasTail;
	bool meteorRandomDecay = meteor->meteorRandomDecay;
	int8_t tailHueStart = meteor->tailHueStart;
	bool tailHueAdd = meteor->tailHueAdd;
	double_t tailHueExponent = meteor->tailHueExponent;
	int8_t tailHueSaturation = meteor->tailHueSaturation;
	int8_t beginPixel = meteor->firstPixel;

	CHSV pRepeatColor = pColor; // Initalize repeat color to meteor color
	int8_t hue = tailHueStart;

	// Calculations are multiplied by two for pixel doubling on frontside/backside LED pairs on stick

	// Calculate tail length
	uint8_t tailStartBrightness = tailBrightnessMap[0];
	uint8_t tailBrightnessMax = tailStartBrightness + 8;
	uint8_t tailLength = tailBrightnessMapLength;
	uint8_t tailSection = tailStartBrightness / tailLength;
	uint8_t halfTailSection = tailSection * 0.5;
	uint8_t adjustedLength = ((meteorSize + tailLength) * 2); // Length of animation for both sides of LED strip

	// First pixel of each region
	int8_t startPixel = meteor->directionDown == true ?
		((region + 1) * regionLength) - 1 : startPixel = region * regionLength;

	// Current pixel to draw
	int8_t drawPixel = meteor->directionDown == true
		? startPixel - beginPixel
		: drawPixel = startPixel + beginPixel;

	// Calculate region start and end
	int8_t regionStart = regionLength * region; // Calculate the pixel before the region start
	int8_t regionEnd = (regionLength * (region + 1) - 1); // Calculate the pixel after the region end

	bool endTail = false; // Flag to end tail


	// Draw every LED based on meteor size and tail length
	for (int8_t d = 1; d < (adjustedLength) + 2; d++) {
		
		// Set current pixel to draw
		int8_t currentPixel;
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
		int8_t tailBrightness;
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