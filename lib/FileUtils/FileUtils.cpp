#include <FileUtils.h>
#include <DevUtils.h>


FileUtils::Config FileUtils::config;

void FileUtils::loadDefaultConfig(JsonDocument& doc)
{
	if (!SPIFFS.exists("/config_default.json")) {
		Serial.print(String(DevUtils::termColor("red")) + "No default config file found!" + String(DevUtils::termColor("reset")) + "\n");
		return;
	}
	File configFile = SPIFFS.open("/config_default.json", "r");
	if (configFile) {
		Serial.println("Successfully opened config_default.json");
	} else {
		Serial.println("Failed to open default config file");
		return;
	}

	size_t size = configFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);
	deserializeJson(doc, buf.get());
	configFile.close();
}

void FileUtils::loadUserConfig(JsonDocument& doc)
{
	if (!SPIFFS.exists("/config_user.json")) {
		Serial.print("No user config file found\n");
		return;
	}

	File configFile = SPIFFS.open("/config_user.json", "r");

	if (configFile) {
		Serial.println("Successfully opened config_user.json");
	} else {
		Serial.println("Failed to open user config file, falling back to defaults");
		return;
	}

	size_t size = configFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);
	deserializeJson(doc, buf.get());
	configFile.close();
}

void FileUtils::merge(JsonObject& dest, const JsonObject& src)
{
	// Serial.println("Merging user config with default config");

	for (JsonPair kvp : src) {
		if (kvp.value().is<JsonObject>() && dest.containsKey(kvp.key())) {
			JsonObject destChild = dest[kvp.key()].as<JsonObject>();
			merge(destChild, kvp.value().as<JsonObject>());
		} else {
			dest[kvp.key()] = kvp.value();
		}
	}
}

void FileUtils::loadConfig()
{
	DynamicJsonDocument defaultDoc(2048);
	DynamicJsonDocument userDoc(2048);

	loadDefaultConfig(defaultDoc);
	loadUserConfig(userDoc);

	JsonObject defaultObj = defaultDoc.as<JsonObject>();
	JsonObject userObj = userDoc.as<JsonObject>();

	merge(defaultObj, userObj);

	// Use defaultObj to extract values to struct
	// Debug Utilities
	if (defaultObj.containsKey("debugUtils")) {
		JsonObject debugUtils = defaultObj["debugUtils"];
		if (debugUtils.containsKey("testCores"))
			config.debugUtils.testCores = debugUtils["testCores"];
		if (debugUtils.containsKey("showSerial"))
			config.debugUtils.showSerial = debugUtils["showSerial"];
		if (debugUtils.containsKey("diagMeasure"))
			config.debugUtils.diagMeasure = debugUtils["diagMeasure"];
		if (debugUtils.containsKey("disableWiFi"))
			config.debugUtils.disableWiFi = debugUtils["disableWiFi"];
		if (debugUtils.containsKey("testLEDs"))
			config.debugUtils.testLEDs = debugUtils["testLEDs"];
	}

	// WiFi Network
	if (defaultObj.containsKey("wifiNetwork")) {
		JsonObject wifiNetwork = defaultObj["wifiNetwork"];
		if (wifiNetwork.containsKey("apSSID")) {
			const char* apSSID = wifiNetwork["apSSID"];
			if (apSSID != nullptr) {
				strncpy(config.wifiNetwork.apSSID, wifiNetwork["apSSID"], sizeof(config.wifiNetwork.apSSID) - 1);
				config.wifiNetwork.apSSID[sizeof(config.wifiNetwork.apSSID) - 1] = '\0';
			}
		}
		if (wifiNetwork.containsKey("apPass")) {
			const char* apPass = wifiNetwork["apPass"];
			if (apPass != nullptr) {
				strncpy(config.wifiNetwork.apPass, wifiNetwork["apPass"], sizeof(config.wifiNetwork.apPass) - 1);
				config.wifiNetwork.apPass[sizeof(config.wifiNetwork.apPass) - 1] = '\0';
			}
		}
		if (wifiNetwork.containsKey("serverName")) {
			const char* serverName = wifiNetwork["serverName"];
			if (serverName != nullptr) {
				strncpy(config.wifiNetwork.serverName, wifiNetwork["serverName"], sizeof(config.wifiNetwork.serverName) - 1);
				config.wifiNetwork.serverName[sizeof(config.wifiNetwork.serverName) - 1] = '\0';
			}
		}
		if (wifiNetwork.containsKey("forceDummyData"))
			config.wifiNetwork.forceDummyData = wifiNetwork["forceDummyData"];
		if (wifiNetwork.containsKey("noTargetLimit"))
			config.wifiNetwork.noTargetLimit = wifiNetwork["noTargetLimit"];
		if (wifiNetwork.containsKey("retryDataFetchLimit"))
			config.wifiNetwork.retryDataFetchLimit = wifiNetwork["retryDataFetchLimit"];
	}

	// Pins Hardware
	if (defaultObj.containsKey("pinsHardware")) {
		JsonObject pinsHardware = defaultObj["pinsHardware"];
		if (pinsHardware.containsKey("outerPin"))
			config.pinsHardware.outerPin = pinsHardware["outerPin"];
		if (pinsHardware.containsKey("middlePin"))
			config.pinsHardware.middlePin = pinsHardware["middlePin"];
		if (pinsHardware.containsKey("innerPin"))
			config.pinsHardware.innerPin = pinsHardware["innerPin"];
		if (pinsHardware.containsKey("bottomPin"))
			config.pinsHardware.bottomPin = pinsHardware["bottomPin"];
		if (pinsHardware.containsKey("wifiRst"))
			config.pinsHardware.wifiRst = pinsHardware["wifiRst"];
		if (pinsHardware.containsKey("outputEnable"))
			config.pinsHardware.outputEnable = pinsHardware["outputEnable"];
		if (pinsHardware.containsKey("potentiometer"))
			config.pinsHardware.potentiometer = pinsHardware["potentiometer"];
	}

	// Display LED
	if (defaultObj.containsKey("displayLED")) {
		JsonObject displayLED = defaultObj["displayLED"];
		if (displayLED.containsKey("brightness"))
			config.displayLED.brightness = displayLED["brightness"];
		if (displayLED.containsKey("fps"))
			config.displayLED.fps = displayLED["fps"];
		if (displayLED.containsKey("outerPixelsTotal"))
			config.displayLED.outerPixelsTotal = displayLED["outerPixelsTotal"];
		if (displayLED.containsKey("middlePixelsTotal"))
			config.displayLED.middlePixelsTotal = displayLED["middlePixelsTotal"];
		if (displayLED.containsKey("innerPixelsTotal"))
			config.displayLED.innerPixelsTotal = displayLED["innerPixelsTotal"];
		if (displayLED.containsKey("bottomPixelsTotal"))
			config.displayLED.bottomPixelsTotal = displayLED["bottomPixelsTotal"];
		if (displayLED.containsKey("outerChunks"))
			config.displayLED.outerChunks = displayLED["outerChunks"];
		if (displayLED.containsKey("middleChunks"))
			config.displayLED.middleChunks = displayLED["middleChunks"];
		if (displayLED.containsKey("innerChunks"))
			config.displayLED.innerChunks = displayLED["innerChunks"];
		if (displayLED.containsKey("bottomChunks"))
			config.displayLED.bottomChunks = displayLED["bottomChunks"];
		if (displayLED.containsKey("craftDelay"))
			config.displayLED.craftDelay = displayLED["craftDelay"];
		if (displayLED.containsKey("displayMinDuration"))
			config.displayLED.displayMinDuration = displayLED["displayMinDuration"];
	}

	// Text Typography
	if (defaultObj.containsKey("textTypography")) {
		JsonObject textTypography = defaultObj["textTypography"];
		if (textTypography.containsKey("characterWidth"))
			config.textTypography.characterWidth = textTypography["characterWidth"];
		if (textTypography.containsKey("textMeteorGap"))
			config.textTypography.textMeteorGap = textTypography["textMeteorGap"];
		if (textTypography.containsKey("meteorOffset"))
			config.textTypography.meteorOffset = textTypography["meteorOffset"];
	}

	// Timers Delays
	if (defaultObj.containsKey("timersDelays")) {
		JsonObject timersDelays = defaultObj["timersDelays"];
		if (timersDelays.containsKey("timerDelay"))
			config.timersDelays.timerDelay = timersDelays["timerDelay"];
	}

	// Miscellaneous
	if (defaultObj.containsKey("miscellaneous")) {
		JsonObject miscellaneous = defaultObj["miscellaneous"];
		if (miscellaneous.containsKey("colorTheme"))
			config.miscellaneous.colorTheme = miscellaneous["colorTheme"];
	}

	Serial.print("User config loaded\n");
}

void FileUtils::saveUserConfig(const std::vector<std::pair<String, JsonVariant>>& updates) {
	// Load the existing user config file into a JSON document
	DynamicJsonDocument userDoc(2048);
	loadUserConfig(userDoc);

	JsonObject userObj = userDoc.as<JsonObject>();

	for (const auto& update : updates) {
		String keyPath = update.first;
		JsonVariant value = update.second;

		JsonObject targetObj = userObj;
		int separatorIndex;
		while ((separatorIndex = keyPath.indexOf('.')) != -1) {
			String key = keyPath.substring(0, separatorIndex);
			if (!targetObj.containsKey(key)) {
				targetObj.createNestedObject(key);
			}
			targetObj = targetObj[key].as<JsonObject>();
			keyPath = keyPath.substring(separatorIndex + 1);
		}
		Serial.print("Saving key: " + keyPath + "...\n");
		targetObj[keyPath] = value;
	}

	// Open the config user file in write mode
	File configFile = SPIFFS.open("/config_user.json", "w");
	if (!configFile) {
		Serial.println("Failed to open config_user.json for writing");
		return;
	}

	Serial.print("Saving user config to file...\n");

	// Serialize the JSON document and write it to the file
	serializeJson(userDoc, configFile);

	// Close the file
	Serial.print("User config saved\n");
	configFile.close();
}
