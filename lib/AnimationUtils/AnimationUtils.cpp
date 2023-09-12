#include <AnimationUtils.h>
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

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
void AnimationUtils::updateBrightness(int brightness)
{
	brightness = constrain(brightness, 8, 255); // Constrain brightness to 8-255
	FileUtils::config.displayLED.brightness = brightness; // Update global brightness value
	FastLED.setBrightness(brightness);

	/* Write new brightness to user config file */
	if (SPIFFS.exists("/config_user.json")) {
		File configFile = SPIFFS.open("/config_user.json", "w");
		if (!configFile) {
			if (FileUtils::config.debugUtils.showSerial)
				Serial.println("Failed to open config file for writing");
			return;
		} else {
			DynamicJsonDocument doc(1024);
			JsonVariant jsonVariantBrightness = doc.to<JsonVariant>();
			jsonVariantBrightness.set(brightness);

			std::vector<std::pair<String, JsonVariant>> updates = {
				{"displayLED.brightness", jsonVariantBrightness}
			};


			FileUtils::saveUserConfig(updates);
		}
	}
}
