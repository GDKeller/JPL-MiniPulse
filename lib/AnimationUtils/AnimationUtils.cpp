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

/* Read brightness potentiometer
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

/* Set global brightness value based on poteniometer value */
void AnimationUtils::updateBrightness()
{
	brightness = readBrightness();
	// Serial.println("Brightness:" + String(brightness) + "\t");
	FastLED.setBrightness(brightness);
}
