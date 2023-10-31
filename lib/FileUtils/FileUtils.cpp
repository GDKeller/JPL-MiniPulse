#include <FileUtils.h>
#include <DevUtils.h>


FileUtils::Config FileUtils::config = {
	{ // debugUtils
		false, // testCores
		false, // showSerial
		false, // diagMeasure
		false, // disableWiFi
		false  // testLEDs
	},
	{ // wifiNetwork
		"JPL MiniPulse", // apSSID
		"",              // apPass
		"https://eyes.nasa.gov/dsn/data/dsn.xml?r=", // serverName
		false, // forceDummyData
		3,     // noTargetLimit
		10     // retryDataFetchLimit
	},
	{ // pinsHardware
		17, // outerPin
		18, // middlePin
		19, // innerPin
		16, // bottomPin
		21, // wifiRst
		22, // outputEnable
		32  // potentiometer
	},
	{ // displayLED
		8, // brightness
		60,  // fps
		800, // outerPixelsTotal
		800, // middlePixelsTotal
		960,  // innerPixelsTotal
		5,   // bottomPixelsTotal
		10,  // outerChunks
		10,  // middleChunks
		12,  // innerChunks
		1,   // bottomChunks
		8000, // craftDelay
		1000  // displayMinDuration
	},

	{ // textTypography
		5,    // characterWidth
		4000, // textMeteorGap
		32    // meteorOffset
	},
	{ // timersDelays
		10000 // timerDelay
	},
	{ // miscellaneous
		0 // colorTheme
	}
};


/* General File/Folder functions */

void FileUtils::listDir(const char* dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = LittleFS.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");

			Serial.print(file.name());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

			if (levels) {
				listDir(file.name(), levels - 1);
			}
		} else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");

			Serial.print(file.size());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
		}
		file = root.openNextFile();
	}
}

void FileUtils::listFilesystem(const char* dirname, int8_t loop = 0) {
	// Serial.printf("Listing directory: %s\r\n", dirname);

	File root = LittleFS.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		for (int8_t i = 0; i < loop; i++) {
			Serial.print("    ");
		}

		Serial.print("└──");

		if (file.isDirectory()) {
			Serial.print("/");
			Serial.print(file.name());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf(
				"  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
				(tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
				tmstruct->tm_mday, tmstruct->tm_hour,
				tmstruct->tm_min, tmstruct->tm_sec
			);

			// Create a new path with leading '/'
			String newPath = String(dirname) + "/" + file.name();
			listFilesystem(newPath.c_str(), loop + 1);
		} else {
			Serial.print(" ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.print(file.size());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf(
				"  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
				(tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
				tmstruct->tm_mday, tmstruct->tm_hour,
				tmstruct->tm_min, tmstruct->tm_sec
			);
		}
		file = root.openNextFile();
	}
}

void FileUtils::createDir(const char* path) {
	// prinmt free heap
	Serial.print("Free heap: " + String(ESP.getFreeHeap()) + "\n");
	Serial.printf("Creating Dir: %s\n", path);
	try {
		if (LittleFS.mkdir(path)) {
			Serial.println("Dir created");
		} else {
			Serial.println("mkdir failed");
		}
	} catch (const std::exception& e) {
		Serial.println(e.what());
	}
}

void FileUtils::removeDir(const char* path) {
	Serial.printf("Removing Dir: %s\n", path);
	if (LittleFS.rmdir(path)) {
		Serial.println("Dir removed");
	} else {
		Serial.println("rmdir failed");
	}
}

void FileUtils::readFile(const char* path) {
	Serial.printf("Reading file: %s\r\n", path);

	File file = LittleFS.open(path);
	if (!file || file.isDirectory()) {
		Serial.println("- failed to open file for reading");
		return;
	}

	Serial.println("- read from file:");
	while (file.available()) {
		Serial.write(file.read());
	}
	file.close();
}

void FileUtils::writeFile(const char* path, const char* message) {
	Serial.printf("Writing file: %s\r\n", path);

	File file = LittleFS.open(path, FILE_WRITE);
	if (!file) {
		Serial.println("- failed to open file for writing");
		return;
	}
	if (file.print(message)) {
		Serial.println("- file written");
	} else {
		Serial.println("- write failed");
	}
	file.close();
}

void FileUtils::appendFile(const char* path, const char* message) {
	Serial.printf("Appending to file: %s\r\n", path);

	File file = LittleFS.open(path, FILE_APPEND);
	if (!file) {
		Serial.println("- failed to open file for appending");
		return;
	}
	if (file.print(message)) {
		Serial.println("- message appended");
	} else {
		Serial.println("- append failed");
	}
	file.close();
}

void FileUtils::renameFile(const char* path1, const char* path2) {
	Serial.printf("Renaming file %s to %s\r\n", path1, path2);
	if (LittleFS.rename(path1, path2)) {
		Serial.println("- file renamed");
	} else {
		Serial.println("- rename failed");
	}
}

void FileUtils::deleteFile(const char* path) {
	Serial.printf("Deleting file: %s\r\n", path);
	if (LittleFS.remove(path)) {
		Serial.println("- file deleted");
	} else {
		Serial.println("- delete failed");
	}
}




/* Config file functions */
void FileUtils::initConfigFile() {
	Serial.println("Initializing config file... ");
	bool fileInitSuccess = FileUtils::checkOrCreateConfigFile();
	if (!fileInitSuccess) {
		Serial.println("Failed checking and creating config file");
		return;
	}

	File configFile = LittleFS.open("/config.json", "r");
	setConfigValuesFromFile(configFile);
}

bool FileUtils::checkConfigFileExists() {
	bool fileExists = LittleFS.exists("/config.json");
	return fileExists;
}

bool FileUtils::createConfigFile() {
	File configFile = LittleFS.open("/config.json", "w");
	if (!configFile) {
		Serial.println("Failed to create file");
		return false;
	} else {
		configFile.print("{}");
		configFile.flush();
		configFile.close();
		return true;
	}
}

bool FileUtils::checkOrCreateConfigFile() {
	if (!checkConfigFileExists()) {
		Serial.println("Config file does not exist, creating...");
		return createConfigFile();
	} else {
		Serial.println("Config file found");
		return true;
	}
}

void FileUtils::setConfigValuesFromFile(File configFile) {
	Serial.println("Setting config values from file...");
	// File configFile = LittleFS.open("/config.json", "r");
	if (!configFile) {
		Serial.println("Failed to open config file for reading");
		return;
	}

	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, configFile);
	if (error) {
		Serial.println("Failed to parse config file");
		return;
	}

	configFile.close();

	for (JsonPair kv : doc.as<JsonObject>()) {
		Serial.print(kv.key().c_str() + String(": ") + kv.value().as<String>() + String("\n"));
		if (kv.key() == "showSerial") {
			FileUtils::config.debugUtils.showSerial = kv.value().as<bool>();
		} else if (kv.key() == "diagMeasure") {
			FileUtils::config.debugUtils.diagMeasure = kv.value().as<int>();
		} else if (kv.key() == "brightness") {
			FileUtils::config.displayLED.brightness = kv.value().as<int>();
		}
	}
}

void FileUtils::printAllConfigFileKeys() {
	File configFile = LittleFS.open("/config.json", "r");
	if (!configFile) {
		Serial.println("Failed to open config file for reading");
		return;
	}

	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, configFile);
	if (error) {
		Serial.println("Failed to parse config file");
		return;
	}

	configFile.close();

	Serial.print("\n");
	Serial.print("┌────────────────────────────────┐\n");
	Serial.print("│        CONFIG FILE KEYS        │\n");
	Serial.print("└────────────────────────────────┘\n");
	

	for (JsonPair kv : doc.as<JsonObject>()) {
		Serial.print(kv.key().c_str() + String(": ") + kv.value().as<String>() + String("\n"));
	}
}


/* Update/Write values to config */

void FileUtils::updateDebugUtilsField(const char* key, const JsonVariant& value) {
	Serial.print("Updating debug utils field - " + String(key) + ": " + String(value.as<bool>()));


	if (strcmp(key, "testCores") == 0) {
		config.debugUtils.testCores = value;
	} else if (strcmp(key, "showSerial") == 0) {
		config.debugUtils.showSerial = value;
	} else if (strcmp(key, "diagMeasure") == 0) {
		config.debugUtils.diagMeasure = value;
	} else if (strcmp(key, "disableWiFi") == 0) {
		config.debugUtils.disableWiFi = value;
	} else if (strcmp(key, "testLEDs") == 0) {
		config.debugUtils.testLEDs = value;
	} else {
		Serial.println("Key not found in DebugUtils struct");
	}
}

void FileUtils::updateWifiNetworkField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "apSSID") == 0) {
		strncpy(config.wifiNetwork.apSSID, value, sizeof(config.wifiNetwork.apSSID));
	} else if (strcmp(key, "apPass") == 0) {
		strncpy(config.wifiNetwork.apPass, value, sizeof(config.wifiNetwork.apPass));
	} else if (strcmp(key, "serverName") == 0) {
		strncpy(config.wifiNetwork.serverName, value, sizeof(config.wifiNetwork.serverName));
	} else if (strcmp(key, "forceDummyData") == 0) {
		config.wifiNetwork.forceDummyData = value;
	} else if (strcmp(key, "noTargetLimit") == 0) {
		config.wifiNetwork.noTargetLimit = value;
	} else if (strcmp(key, "retryDataFetchLimit") == 0) {
		config.wifiNetwork.retryDataFetchLimit = value;
	} else {
		Serial.println("Key not found in WifiNetwork struct");
	}
}

void FileUtils::updatePinsHardwareField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "outerPin") == 0) {
		config.pinsHardware.outerPin = value;
	} else if (strcmp(key, "middlePin") == 0) {
		config.pinsHardware.middlePin = value;
	} else if (strcmp(key, "innerPin") == 0) {
		config.pinsHardware.innerPin = value;
	} else if (strcmp(key, "bottomPin") == 0) {
		config.pinsHardware.bottomPin = value;
	} else if (strcmp(key, "wifiRst") == 0) {
		config.pinsHardware.wifiRst = value;
	} else if (strcmp(key, "outputEnable") == 0) {
		config.pinsHardware.outputEnable = value;
	} else if (strcmp(key, "potentiometer") == 0) {
		config.pinsHardware.potentiometer = value;
	} else {
		Serial.println("Key not found in PinsHardware struct");
	}
}

// Function to update DisplayLED fields
void FileUtils::updateDisplayLEDField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "brightness") == 0) {
		config.displayLED.brightness = value;
	} else if (strcmp(key, "fps") == 0) {
		config.displayLED.fps = value;
	} else if (strcmp(key, "outerPixelsTotal") == 0) {
		config.displayLED.outerPixelsTotal = value;
	} else if (strcmp(key, "middlePixelsTotal") == 0) {
		config.displayLED.middlePixelsTotal = value;
	} else if (strcmp(key, "innerPixelsTotal") == 0) {
		config.displayLED.innerPixelsTotal = value;
	} else if (strcmp(key, "bottomPixelsTotal") == 0) {
		config.displayLED.bottomPixelsTotal = value;
	} else if (strcmp(key, "outerChunks") == 0) {
		config.displayLED.outerChunks = value;
	} else if (strcmp(key, "middleChunks") == 0) {
		config.displayLED.middleChunks = value;
	} else if (strcmp(key, "innerChunks") == 0) {
		config.displayLED.innerChunks = value;
	} else if (strcmp(key, "bottomChunks") == 0) {
		config.displayLED.bottomChunks = value;
	} else if (strcmp(key, "craftDelay") == 0) {
		config.displayLED.craftDelay = value;
	} else if (strcmp(key, "displayMinDuration") == 0) {
		config.displayLED.displayMinDuration = value;
	} else {
		Serial.println("Key not found in DisplayLED struct");
	}
}

// Function to update TextTypography fields
void FileUtils::updateTextTypographyField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "characterWidth") == 0) {
		config.textTypography.characterWidth = value;
	} else if (strcmp(key, "textMeteorGap") == 0) {
		config.textTypography.textMeteorGap = value;
	} else if (strcmp(key, "meteorOffset") == 0) {
		config.textTypography.meteorOffset = value;
	} else {
		Serial.println("Key not found in TextTypography struct");
	}
}

// Function to update TimersDelays fields
void FileUtils::updateTimersDelaysField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "timerDelay") == 0) {
		config.timersDelays.timerDelay = value;
	} else {
		Serial.println("Key not found in TimersDelays struct");
	}
}

// Function to update Miscellaneous fields
void FileUtils::updateMiscellaneousField(const char* key, const JsonVariant& value) {
	if (strcmp(key, "colorTheme") == 0) {
		config.miscellaneous.colorTheme = value;
	} else {
		Serial.println("Key not found in Miscellaneous struct");
	}
}

void FileUtils::writeConfigFileBool(const char* key, bool value) {
	File configFile = LittleFS.open("/config.json", "r+");
	if (!configFile) {
		Serial.println("Failed to open config file for reading");
		return;
	}

	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, configFile);
	if (error) {
		Serial.println("Failed to parse config file");
		return;
	}

	configFile.close();

	doc[key] = value;

	configFile = LittleFS.open("/config.json", "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		return;
	}

	serializeJson(doc, configFile);
	configFile.close();

	String stringValue = value ? "true" : "false";
	Serial.println("Config key " + String(key) + " set to " + stringValue);
}

void FileUtils::writeConfigFileInt(const char* key, int value) {
	if (!checkConfigFileExists()) {
		Serial.println("Config file does not exist");
	} else {
		Serial.println("Config file found");
	}
	File configFile = LittleFS.open("/config.json", "r+");
	if (!configFile) {
		Serial.println("Failed to open config file for reading");
		return;
	}

	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, configFile);
	if (error) {
		Serial.println("Failed to parse config file");
		return;
	}

	configFile.close();

	doc[key] = value; // Update file value

	configFile = LittleFS.open("/config.json", "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		return;
	}

	if (serializeJson(doc, configFile) == 0) {
		Serial.println("Failed to write to config file");
		return;
	}

	configFile.flush(); // Flush file to write
	configFile.close(); // Close file

	Serial.println("CONFIG FILE saved: " + String(key) + " == " + String(value));
}

void FileUtils::writeConfigFileString(const char* key, const char* value) {
	File configFile = LittleFS.open("/config.json", "r+");
	if (!configFile) {
		Serial.println("Failed to open config file for reading");
		return;
	}

	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, configFile);
	if (error) {
		Serial.println("Failed to parse config file");
		return;
	}

	configFile.close();

	doc[key] = value;

	configFile = LittleFS.open("/config.json", "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		return;
	}

	serializeJson(doc, configFile);
	configFile.close();

	Serial.println("Config key " + String(key) + " set to " + String(value));
}