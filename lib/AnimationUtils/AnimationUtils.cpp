#include <AnimationUtils.h>

/* Initialize global brightness */
int AnimationUtils::brightness = 16;

AnimationUtils::AnimationUtils()
{
	// be happy
}

/* Constructor - set brightness potentiometer pin */
AnimationUtils::AnimationUtils(int pin)
{
	potPin = pin;
}

/**
 * Read potentiometer for global brightness value
 *
 * @return Potentiometer value mapped to 0-255
 */
int AnimationUtils::readBrightness()
{
	int potValue = analogRead(potPin); // Read potentiometer value
	potValue = constrain(potValue, 0, 127);	// Constrain potentiometer value to 0-127 so brightness won't wrap around
	int potBrightness = map(potValue, 0, 127, 8, 255); // Map potentiometer value to 8-255 for brightness - less than 8 causes flickering
	int hardcodedBrightness = 32; // Hardcoded brightness value for testing
	return potBrightness;
}

/**
 * Sets the global brightness value based on poteniometer value
 */
void AnimationUtils::updateBrightness()
{
	brightness = readBrightness();
	// Serial.println("Brightness:" + String(brightness) + "\t");
	FastLED.setBrightness(brightness);
}

/**
 * Sets pixel color relative to global BRIGHTNESS definition and adds gamma correction
 *
 * The color value must be a pointer - it is deferenced here so Color objects aren't being copied as they're passed
 *
 * @param strip The memory reference of the NeoPixel strip to be updated
 * @param n The pixel on the strip to be set
 * @param color The memory reference of the color to set pixel to
 */
void AnimationUtils::setPixelColor(Adafruit_NeoPixel &strip, uint16_t n, const uint32_t *color)
{
	// uint8_t rgb[4];				// Create array that will hold color channel values
	// *(uint32_t *)&rgb = *color; // Assigns color value to the color channel array
	// uint8_t channelR = rgb[0];	// blue color channel value
	// uint8_t channelG = rgb[1];	// Green color channel value
	// uint8_t channelB = rgb[2];	// Blue color channel value
	// uint32_t newColor = Adafruit_NeoPixel::Color(((AnimationUtils::brightness * channelR) / 255), ((AnimationUtils::brightness * channelG) / 255), ((AnimationUtils::brightness * channelB) / 255));
	// if (newColor > 4294967295) newColor = 4294967295;
	// if (newColor < 0) newColor = 0;
	// uint32_t gammaCorrected = Adafruit_NeoPixel::gamma32(newColor);
	// strip.setPixelColor(n, Adafruit_NeoPixel::gamma32(*color));
	strip.setPixelColor(n, *color);
}
