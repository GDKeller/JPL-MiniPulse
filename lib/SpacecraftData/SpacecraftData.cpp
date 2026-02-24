#include <SpacecraftData.h>

DynamicJsonDocument SpacecraftData::spacecraftNamesJson(5120);
DynamicJsonDocument SpacecraftData::spacecraftBlacklistJson(1024);


void SpacecraftData::loadJson() {
    Serial.println("Loading spacecraft data...");
    SpacecraftData::loadSpacecraftNamesFile();
    delay(100);
    Serial.println("Loading spacecraft blacklist...");
    SpacecraftData::loadSpacecraftBlacklistFile();
    delay(100);
};

// Create and write spacecraft names file
void SpacecraftData::createAndWriteNamesFile() {
    if (LittleFS.exists("/spacecraft_data/names.json")) {
        Serial.println("names.json already exists, overwriting...");
    }

    // Open file for writing
    Serial.println("Opening names.json for writing...");
    File file = LittleFS.open("/spacecraft_data/names.json", "w");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open names.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Prep JSON buffer
    Serial.println("Clearing global JSON document...");
    spacecraftNamesJson.clear();

    // Add spacecraft data directly into the JSON object
    spacecraftNamesJson["ACE"] = "Advanced Composition Explorer";
    spacecraftNamesJson["PLC"] = "Akatsuki";
    spacecraftNamesJson["ARGO"] = "ArgoMoon";
    spacecraftNamesJson["BIOS"] = "BioSentinel";
    spacecraftNamesJson["CAPS"] = "Capstone";
    spacecraftNamesJson["CHDR"] = "Chandra Xray Observatory";
    spacecraftNamesJson["CH2"] = "Chandrayaan 2";
    spacecraftNamesJson["CUE3"] = "CU Earth Escape Explorer";
    spacecraftNamesJson["CuSP"] = "CubeSat-Observation of Solar Particles";
    spacecraftNamesJson["DART"] = "DART";
    spacecraftNamesJson["unknown10"] = "Dragonfly";
    spacecraftNamesJson["DSCO"] = "Deep Space Climate Observatory";
    spacecraftNamesJson["EMM"] = "Emirates Mars Mission";
    spacecraftNamesJson["EQUL"] = "EQUilibriUm Lunar-Earth Spacecraft";
    spacecraftNamesJson["EURC"] = "Europa Clipper";
    spacecraftNamesJson["RSP"] = "ExoMars Rover";
    spacecraftNamesJson["GAIA"] = "Gaia";
    spacecraftNamesJson["GTL"] = "Geotail";
    spacecraftNamesJson["HYB2"] = "Hayabusa 2";
    spacecraftNamesJson["EM1"] = "Artemis 1";
    spacecraftNamesJson["EM2"] = "Artemis 2";
    spacecraftNamesJson["EM3"] = "Artemis 3";
    spacecraftNamesJson["NSYT"] = "InSight";
    spacecraftNamesJson["JWST"] = "James Webb Space Telescope";
    spacecraftNamesJson["JNO"] = "Juno";
    spacecraftNamesJson["KPLO"] = "Korea Pathfinder Lunar Orbiter";
    spacecraftNamesJson["LICI"] = "LICIA Cube";
    spacecraftNamesJson["LND1"] = "Lunar Node 1";
    spacecraftNamesJson["LUCY"] = "Lucy";
    spacecraftNamesJson["LFL"] = "Lunar Flashlight";
    spacecraftNamesJson["HMAP"] = "Lunar Hydrogen Mapper";
    spacecraftNamesJson["LRO"] = "Lunar Reconnaissance Orbiter";
    spacecraftNamesJson["MMS1"] = "Magnetospheric MultiScale Formation Flyer 1";
    spacecraftNamesJson["MMS2"] = "Magnetospheric MultiScale Formation Flyer 2";
    spacecraftNamesJson["MMS3"] = "Magnetospheric MultiScale Formation Flyer 3";
    spacecraftNamesJson["MMS4"] = "Magnetospheric MultiScale Formation Flyer 4";
    spacecraftNamesJson["M01O"] = "Mars Odyssey";
    spacecraftNamesJson["M20"] = "Mars 2020";
    spacecraftNamesJson["MVN"] = "MAVEN";
    spacecraftNamesJson["MEX"] = "Mars Express";
    spacecraftNamesJson["MOM"] = "Mars Orbiter";
    spacecraftNamesJson["MRO"] = "Mars Reconnaissance Orbiter";
    spacecraftNamesJson["MSL"] = "Curiosity";
    spacecraftNamesJson["MLI"] = "Morehead Lunar Ice Cube";
    spacecraftNamesJson["NEAS"] = "Near Earth Asteroid Scout";
    spacecraftNamesJson["NHPC"] = "New Horizons";
    spacecraftNamesJson["ORX"] = "OSIRIS REx";
    spacecraftNamesJson["OMOT"] = "OMOTENASHI";
    spacecraftNamesJson["PSYC"] = "Psyche";
    spacecraftNamesJson["SOHO"] = "Solar and Heliospheric Observatory";
    spacecraftNamesJson["SPP"] = "Parker Solar Probe";
    spacecraftNamesJson["STA"] = "STEREO A";
    spacecraftNamesJson["TESS"] = "Transiting Exoplanet Survey Satellite";
    spacecraftNamesJson["TGO"] = "ExoMars Trace Gas Orbiter";
    spacecraftNamesJson["THB"] = "THEMIS B";
    spacecraftNamesJson["THC"] = "THEMIS C";
    spacecraftNamesJson["TM"] = "TeamMiles";
    spacecraftNamesJson["VGR1"] = "Voyager 1";
    spacecraftNamesJson["VGR2"] = "Voyager 2";
    spacecraftNamesJson["WIND"] = "Wind";
    spacecraftNamesJson["XMM"] = "XMM Newton";
    spacecraftNamesJson["ATOT"] = "Advanced Tracking and Observational Techniques";
    spacecraftNamesJson["EGS"] = "EVN and Global Sevices";
    spacecraftNamesJson["GBRA"] = "Ground Based Radio Astronomy";
    spacecraftNamesJson["GSSR"] = "Goldstone Solar System Radar";
    spacecraftNamesJson["GVRT"] = "Goldstone Apple Valley Radio Telescope";
    spacecraftNamesJson["SGP"] = "Space Geodesy Program";
    spacecraftNamesJson["TDR6"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TDR7"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TDR8"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TDR9"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TD10"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TD11"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TD12"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["TD13"] = "Tracking and Data Relay Satellites (TDRS)";
    spacecraftNamesJson["Rate1"] = "Test Rate 1";
    spacecraftNamesJson["Rate2"] = "Test Rate 2";
    spacecraftNamesJson["Rate3"] = "Test Rate 3";
    spacecraftNamesJson["Rate4"] = "Test Rate 4";
    spacecraftNamesJson["Rate5"] = "Test Rate 5";
    spacecraftNamesJson["Rate6"] = "Test Rate 6";


    // Serialize JSON to file
    Serial.println("Serializing JSON to file...");
    if (serializeJson(spacecraftNamesJson, file) == 0) {
        Serial.print(DevUtils::termColor("red") + "Failed to write to names.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Close file
    Serial.println("Closing names.json...");
    file.close();
    Serial.print(DevUtils::termColor("green") + "names.json created and written" + DevUtils::termColor("reset") + "\n");

    // print all key value pairs
    int craftCount = 0;
    for (JsonPair kv : spacecraftNamesJson.as<JsonObject>()) {
        craftCount++;
        // Serial.print(String(kv.key().c_str()) + ": " + kv.value().as<String>() + "\n");
    }
    
    char craftCountBuffer[128];
    snprintf(
        craftCountBuffer,
        sizeof(craftCountBuffer),
        "Known spacecraft: %d\n\n",
        craftCount
    );
}

// Load spacecraft Names
void SpacecraftData::loadSpacecraftNamesFile()
{
    SpacecraftData::createAndWriteNamesFile();

    Serial.println("Loading spacecraft names from filesystem...");

    FileUtils::createDir("spacecraft_data");

    // Open file for reading
    Serial.println("Opening names.json for reading...");
    File file = LittleFS.open("/spacecraft_data/names.json", "r");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open names.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    Serial.println("File \"names.json\" opened successfully");

    size_t size = file.size();
    if (size > 5120) {
        Serial.print(DevUtils::termColor("red") + "names.json file size is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    Serial.println("Deserializing JSON from file...");
    DeserializationError error = deserializeJson(spacecraftNamesJson, file);
    file.close();
    vTaskDelay(100); // Make sure the file is closed

    if (error) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
        return;
    }

    Serial.print(DevUtils::termColor("green") + "Spacecraft names loaded" + DevUtils::termColor("reset") + "\n");
    // Serial.print("\n");
}

void SpacecraftData::createAndWriteBlacklistFile() {
    if (LittleFS.exists("/spacecraft_data/blacklist.json")) {
        Serial.println("blacklist.json already exists, overwriting...");
    }

    // Open file for writing
    Serial.println("Opening blacklist.json for writing...");
    File file = LittleFS.open("/spacecraft_data/blacklist.json", "w");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open blacklist.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Prep JSON buffer
    Serial.println("Clearing global JSON document...");
    spacecraftBlacklistJson.clear();

    // Add spacecraft data directly into the JSON object
    spacecraftBlacklistJson["TEST"] = true;
    spacecraftBlacklistJson["DSN"] = true;
    spacecraftBlacklistJson["RFC(VLBI)"] = true;
    spacecraftBlacklistJson["GO19"];

    // Serialize JSON to file
    Serial.println("Serializing JSON to file...");
    if (serializeJson(spacecraftBlacklistJson, file) == 0) {
        Serial.print(DevUtils::termColor("red") + "Failed to write to blacklist.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    file.close();
    Serial.println(DevUtils::termColor("green") + "blacklist.json written" + DevUtils::termColor("reset") + "\n");

    // print all key value pairs
    for (JsonPair kv : spacecraftBlacklistJson.as<JsonObject>()) {
        Serial.println(kv.key().c_str());
    }

}

void SpacecraftData::loadSpacecraftBlacklistFile() {
    SpacecraftData::createAndWriteBlacklistFile();
    
    Serial.println("Loading spacecraft blacklist from filesystem...");
    
    FileUtils::createDir("spacecraft_data");
    

    // Open file for reading
    Serial.println("Opening blacklist.json for reading...");
    File file = LittleFS.open("/spacecraft_data/blacklist.json", "r");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open blacklist.json" + DevUtils::termColor("reset") + "\n");
        return;
    }

    Serial.println("File \"blacklist.json\" opened successfully");

    size_t size = file.size();
    if (size > 1024) {
        Serial.print(DevUtils::termColor("red") + "blacklist.json file size is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    Serial.println("Deserializing JSON...");
    DeserializationError error = deserializeJson(spacecraftBlacklistJson, file);
    file.close();

    if (error) {
        if (FileUtils::config.debugUtils.showSerial)
            Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

        return;
    }

    Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
    Serial.print("\n\n");
}





/* Check Name */
const char* SpacecraftData::callsignToName(const char* key) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;

    // if (showSerial) {
    //     Serial.print("\n\n------------------------------\n");
    //     Serial.print("*  Spacecraft Names  *\n");
    //     Serial.print("------------------------------\n");
    // }
    
    // File jsonFile = LittleFS.open("/spacecraft_data/names.json", "r");

    // if (jsonFile) {
    //     if (showSerial)
    //         Serial.println("File \"names.json\" opened");
    // } else {
    //     if (showSerial)
    //         Serial.println(DevUtils::termColor("red") + "Failed to open names.json" + DevUtils::termColor("reset") + "\n");
    // }
    
    // DeserializationError error = deserializeJson(spacecraftNamesJson, jsonFile);
    // jsonFile.close();

    // print every key in spacecraftNamesJson
    // for (JsonPair kv : spacecraftNamesJson.as<JsonObject>()) {
    //     Serial.println(String(kv.key().c_str()) + ": " + kv.value().as<String>());
    //     // break;
    // }

    if (showSerial) {
        char buffer[256];
        snprintf(
            buffer,
            sizeof(buffer),
            "%s>>> Checking names lookup for %s...%s\n",
            DevUtils::termColor("yellow"),
            key,
            DevUtils::termColor("reset")
        );
        Serial.print(buffer);
    }

    if (spacecraftNamesJson.containsKey(key)) {
        if (showSerial)
            Serial.print(DevUtils::termColor("green") + "Found spacecraft name for " + String(key) + DevUtils::termColor("reset") + "\n");
        const char* spacecraftName = spacecraftNamesJson[key];
        return spacecraftName;
    }

    if (showSerial) {
        Serial.print(DevUtils::termColor("red") + "Spacecraft name not found for " + String(key) + DevUtils::termColor("reset") + "\n");
    }

    // Spacecraft name not found, return the callsign
    return key;
}


/* Check Blacklist */
bool SpacecraftData::checkBlacklist(const char* key) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    bool isBlacklisted = spacecraftBlacklistJson[key] != nullptr; // If the key is not null, it is blacklisted

    if (showSerial) {
        char buffer[128]; // Size this appropriately for your expected output length
        // const char* isBlacklistedString = isBlacklisted ? "TRUE" : "FALSE";

        if (isBlacklisted) {
            snprintf(
                buffer,
                sizeof(buffer),
                "%s%s is blacklisted, skipping...%s\n", 
                DevUtils::termColor("purple"), 
                key, 
                DevUtils::termColor("reset")
            );
            Serial.print(buffer);
        }
    }

    return isBlacklisted;
}








// void SpacecraftData::loadSpacecraftNamesProgmem()
// {
//     /* Memory warning!
//      * The values cannot be longer than 100 characters, or there will be a fatal error.
//      * The memory for this variable for animation function is statically allocated
//      */

//     static const char spacecraftNamesJsonRaw[] = PROGMEM R"RAW-NAMES(
//         {
//             "ACE": "Advanced Composition Explorer",
//             "PLC": "Akatsuki",
//             "ARGO": "ArgoMoon",
//             "BIOS": "BioSentinel",
//             "CHDR": "Chandra Xray Observatory",
//             "CH2": "Chandrayaan 2",
//             "CUE3": "CU Earth Escape Explorer",
//             "CuSP": "CubeSat-Observation of Solar Particles",
//             "DART": "DART",
//             "unknown10": "Dragonfly",
//             "DSCO": "Deep Space Climate Observatory",
//             "EMM": "Emirates Mars Mission",
//             "EQUL": "EQUilibriUm Lunar-Earth Spacecraft",
//             "EURC": "Europa Clipper",
//             "RSP": "ExoMars Rover",
//             "GAIA": "Gaia",
//             "GTL": "Geotail",
//             "HYB2": "Hayabusa 2",
//             "EM1": "Artemis 1",
//             "EM2": "Artemis 2",
//             "EM3": "Artemis 3",
//             "NSYT": "InSight",
//             "JWST": "James Webb Space Telescope",
//             "JNO": "Juno",
//             "KPLO": "Korea Pathfinder Lunar Orbiter",
//             "LICI": "LICIA Cube",
//             "LND1": "Lunar Node 1",
//             "LUCY": "Lucy",
//             "LFL": "Lunar Flashlight",
//             "HMAP": "Lunar Hydrogen Mapper",
//             "LRO": "Lunar Reconnaissance Orbiter",
//             "MMS1": "Magnetospheric MultiScale Formation Flyer 1",
//             "MMS2": "Magnetospheric MultiScale Formation Flyer 2",
//             "MMS3": "Magnetospheric MultiScale Formation Flyer 3",
//             "MMS4": "Magnetospheric MultiScale Formation Flyer 4",
//             "M01O": "Mars Odyssey",
//             "M20": "Mars 2020",
//             "MVN": "MAVEN",
//             "MEX": "Mars Express",
//             "MOM": "Mars Orbiter",
//             "MRO": "Mars Reconnaissance Orbiter",
//             "MSL": "Curiosity",
//             "MLI": "Morehead Lunar Ice Cube",
//             "NEAS": "Near Earth Asteroid Scout",
//             "NHPC": "New Horizons",
//             "ORX": "OSIRIS REx",
//             "OMOT": "OMOTENASHI",
//             "PSYC": "Psyche",
//             "SOHO": "Solar and Heliospheric Observatory",
//             "SPP": "Parker Solar Probe",
//             "STA": "STEREO A",
//             "TESS": "Transiting Exoplanet Survey Satellite",
//             "TGO": "ExoMars Trace Gas Orbiter",
//             "THB": "THEMIS B",
//             "THC": "THEMIS C",
//             "TM": "TeamMiles",
//             "VGR1": "Voyager 1",
//             "VGR2": "Voyager 2",
//             "WIND": "Wind",
//             "XMM": "XMM Newton",
//             "ATOT": "Advanced Tracking and Observational Techniques",
//             "EGS": "EVN and Global Sevices",
//             "GBRA": "Ground Based Radio Astronomy",
//             "GSSR": "Goldstone Solar System Radar",
//             "GVRT": "Goldstone Apple Valley Radio Telescope",
//             "SGP": "Space Geodesy Program",
//             "TDR6": "Tracking and Data Relay Satellites (TDRS)",
//             "TDR7": "Tracking and Data Relay Satellites (TDRS)",
//             "TDR8": "Tracking and Data Relay Satellites (TDRS)",
//             "TDR9": "Tracking and Data Relay Satellites (TDRS)",
//             "TD10": "Tracking and Data Relay Satellites (TDRS)",
//             "TD11": "Tracking and Data Relay Satellites (TDRS)",
//             "TD12": "Tracking and Data Relay Satellites (TDRS)",
//             "TD13": "Tracking and Data Relay Satellites (TDRS)",
//             "Rate1": "Test Rate 1",
//             "Rate2": "Test Rate 2",
//             "Rate3": "Test Rate 3",
//             "Rate4": "Test Rate 4",
//             "Rate5": "Test Rate 5",
//             "Rate6": "Test Rate 6"
//         }
//     )RAW-NAMES";


//     // Check if JSON is too large
//     if (strlen_P(spacecraftNamesJsonRaw) > 5120) {
//         Serial.print(DevUtils::termColor("red") + "Raw names JSON is too large" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Make sure there is enough heap memory to load names
//     if (ESP.getFreeHeap() < 5120) {
//         Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load names" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Initialize buffer
//     char buffer[5120];
//     if (!buffer) {
//         Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Copy raw JSON to buffer
//     strncpy_P(buffer, spacecraftNamesJsonRaw, 5119);
//     buffer[5119] = '\0';


//     DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

//     if (error) {
//         Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
//         return;
//     }
//     Serial.print(DevUtils::termColor("green") + "Spacecraft callsigns loaded" + DevUtils::termColor("reset") + "\n");
//     Serial.println("Rate1: " + spacecraftNamesJson["Rate1"].as<String>());
//     Serial.print("\n\n");
// }


// Load spacecraft blacklist
// void SpacecraftData::loadSpacecraftBlacklistProgmem() {
//     static const char spacecraftBlacklistJsonRaw[] = PROGMEM R"RAW-BLACKLIST(
//         {
//             "TEST": true,
//             "DSN": true,
//             "RFC(VLBI)": true
//         }
//     )RAW-BLACKLIST";

//     // Check if JSON is too large
//     if (strlen_P(spacecraftBlacklistJsonRaw) > 1024) {
//         Serial.print(DevUtils::termColor("red") + "Raw blacklist JSON is too large" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Make sure there is enough heap memory to load blacklist
//     if (ESP.getFreeHeap() < 1024) {
//         Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load blacklist" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Initialize buffer
//     char buffer[1024];
//     if (!buffer) {
//         Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     // Copy raw JSON to buffer
//     strncpy_P(buffer, spacecraftBlacklistJsonRaw, 1023);
//     buffer[1023] = '\0';


//     DeserializationError error = deserializeJson(spacecraftBlacklistJson, buffer);

//     if (error) {
//         if (FileUtils::config.debugUtils.showSerial)
//             Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

//         return;
//     }
//     Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
// }
