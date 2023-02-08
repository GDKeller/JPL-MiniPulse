#include <Animate.h>

Animate::Animate() {}

AnimationUtils Animate::aUtilAnimate = AnimationUtils();

void Animate::animateMeteor(Meteor* meteor)
{
	Adafruit_NeoPixel *&strip = meteor->rStrip;
	int region = meteor->region;
	int regionLength = meteor->regionLength;
	uint32_t *pColor = meteor->pColor; 
	int meteorSize = meteor->meteorSize;
	bool meteorTrailDecay = meteor->meteorTrailDecay;
	bool meteorRandomDecay = meteor->meteorRandomDecay;
	int tailHueStart = meteor->tailHueStart;
	bool tailHueAdd = meteor->tailHueAdd;
	double tailHueExponent = meteor->tailHueExponent;
	int tailHueSaturation = meteor->tailHueSaturation;
	int beginPixel = meteor->firstPixel;

	uint32_t *pRepeatColor = pColor;
	int hue = degreeToSixteenbit(tailHueStart);

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
			aUtilAnimate.setPixelColor(*strip, currentPixel, pRepeatColor);
			continue;
		}

		// Draw meteor
		if (d < meteorSize + 1)
		{
			aUtilAnimate.setPixelColor(*strip, currentPixel, pColor);
			continue;
		}


		// Draw tail
		uint8_t brightValue;
		uint32_t trailColor;
		if (meteorTrailDecay == true) {
			int satExpo = ceil(tailHueSaturation * log(d + 1)); // Calculate logarithmic growth
			satExpo += random(32) - 16;							// Add random variance to saturation
			uint8_t satValue = satExpo > tailHueSaturation ? tailHueSaturation : (satExpo < 0 ? 0 : satExpo);
			int brightExpo = ceil(255 * mPower(0.85, d)); // Calculate exponential decay
			brightExpo += random(32) - 16;				  // Add randomvariance to brightness
			uint8_t brightValue = brightExpo > 255 ? 255 : (brightExpo < 0 ? 0 : brightExpo);
			int brightValueMap = map(brightValue, 0, 255, 0, AnimationUtils::brightness);
			int randVal = (4 * d) * (4 * d);
			int hueRandom = hue + (random(randVal) - (randVal / 2));
			trailColor = strip->ColorHSV(hueRandom, satValue, brightValue);
		} else {
			// int satCalc = tailHueSaturation - (d * 16);
			// satCalc += random(32) - 16;
			// uint8_t satValue = satCalc < 0 ? 0 : satCalc;
			int satValue = tailHueSaturation;
			int brightCalc = 255 - (d * 16);
			// brightCalc += random(32) - 16;
			brightValue = brightCalc < 0 ? 0 : brightCalc;
			trailColor = strip->ColorHSV(hue, satValue, brightValue);
		}
		uint32_t *pTrailColor = &trailColor;

		// Cycle hue through time
		// tailHueAdd == true ? hue += ceil(4096 * mPower(tailHueExponent, d)) : hue -= ceil(4096 * mPower(tailHueExponent, d));

		// Make sure the pixel right after the meteor will get drawn so meteor values aren't repeated
		if (d < (meteorSize + 1))
		{
			aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
			continue;
		}

		// Roll the dice on showing each pixel for a "fizzle" effect
		if (meteorRandomDecay == true) {
			if (random(10) < 5) aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
		} else {
			aUtilAnimate.setPixelColor(*strip, currentPixel, pTrailColor);
		}

		pRepeatColor = pTrailColor;

		if (brightValue == 0) continue;
	}
}



// // Fill strip pixels one after another with a color. Strip is NOT cleared
// // first; anything there will be covered pixel by pixel. Pass in color
// // (as a single 'packed' 32-bit value, which you can get by calling
// // strip.Color(red, green, blue) as shown in the loop() function above),
// // and a delay time (in milliseconds) between pixels.
// void colorWipe(Adafruit_NeoPixel &strip, uint32_t *color, int wait)
// {
// 	for (int i = 0; i < strip.numPixels(); i++)
// 	{									   // For each pixel in strip...
// 		au.setPixelColor(strip, i, color); //  Set pixel's color (in RAM)
// 		strip.show();					   //  Update strip to match
// 		delay(wait);					   //  Pause for a moment
// 	}
// }

// // Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// // a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// // between frames.
// void theaterChase(Adafruit_NeoPixel &strip, uint32_t *color, int wait)
// {
// 	for (int a = 0; a < 10; a++)
// 	{ // Repeat 10 times...
// 		for (int b = 0; b < 3; b++)
// 		{				   //  'b' counts from 0 to 2...
// 			strip.clear(); //   Set all pixels in RAM to 0 (off)

// 			// 'c' counts up from 'b' to end of strip in steps of 3...
// 			for (int c = b; c < strip.numPixels(); c += 3)
// 			{
// 				au.setPixelColor(strip, c, color); // Set pixel 'c' to value 'color'
// 			}
// 			strip.show(); // Update strip with new contents
// 			delay(wait);  // Pause for a moment
// 		}
// 	}
// }

// // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
// void rainbow(Adafruit_NeoPixel &strip, int wait)
// {
// 	// Hue of first pixel runs 5 complete loops through the color wheel.
// 	// Color wheel has a range of 65536 but it's OK if we roll over, so
// 	// just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
// 	// means we'll make 5*65536/256 = 1280 passes through this loop:
// 	for (long firstPixelHue = 0; firstPixelHue < 1 * 65536; firstPixelHue += 256)
// 	{
// 		// strip.rainbow() can take a single argument (first pixel hue) or
// 		// optionally a few extras: number of rainbow repetitions (default 1),
// 		// saturation and value (brightness) (both 0-255, similar to the
// 		// ColorHSV() function, default 255), and a true/false flag for whether
// 		// to apply gamma correction to provide 'truer' colors (default true).
// 		au.updateBrightness();
// 		strip.rainbow(firstPixelHue, 1, 255, AnimationUtils::brightness, true);
// 		// Above line is equivalent to:
// 		// strip.rainbow(firstPixelHue, 1, 255, 255, true);
// 		strip.show(); // Update strip with new contents
// 		delay(wait);  // Pause for a moment
// 	}
// }

// // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
// void hueCycle(Adafruit_NeoPixel &strip, int wait)
// {
// 	// Hue of first pixel runs 5 complete loops through the color wheel.
// 	// Color wheel has a range of 65536 but it's OK if we roll over, so
// 	// just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
// 	// means we'll make 5*65536/256 = 1280 passes through this loop:
// 	for (long hue = 0; hue < 1 * 65536; hue += 256)
// 	{
// 		au.updateBrightness();
// 		strip.fill(Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(hue, 255, AnimationUtils::brightness)));
// 		strip.show(); // Update strip with new contents
// 		delay(wait);  // Pause for a moment
// 	}
// }

// // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
// void theaterChaseRainbow(Adafruit_NeoPixel &strip, int wait)
// {
// 	int firstPixelHue = 0; // First pixel starts at red (hue 0)
// 	for (int a = 0; a < 30; a++)
// 	{ // Repeat 30 times...
// 		for (int b = 0; b < 3; b++)
// 		{				   //  'b' counts from 0 to 2...
// 			strip.clear(); //   Set all pixels in RAM to 0 (off)
// 			// 'c' counts up from 'b' to end of strip in increments of 3...
// 			for (int c = b; c < strip.numPixels(); c += 3)
// 			{
// 				// hue of pixel 'c' is offset by an amount to make one full
// 				// revolution of the color wheel (range 65536) along the length
// 				// of the strip (strip.numPixels() steps):
// 				int hue = firstPixelHue + c * 65536L / strip.numPixels();
// 				uint32_t color = strip.ColorHSV(hue); // hue -> RGB
// 				const uint32_t *pColor = &color;
// 				au.setPixelColor(strip, c, pColor); // Set pixel 'c' to value 'color'
// 			}
// 			strip.show();				 // Update strip with new contents
// 			delay(wait);				 // Pause for a moment
// 			firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
// 		}
// 	}
// }
