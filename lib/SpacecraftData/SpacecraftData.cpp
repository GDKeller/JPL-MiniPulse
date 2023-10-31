#include <SpacecraftData.h>


SpacecraftData::SpacecraftData()
    : spacecraftNamesJson(5120), spacecraftBlacklistJson(1024), spacecraftPlaceholderRatesJson(6144) {}


void SpacecraftData::loadJson() {
    Serial.println("Loading spacecraft data...");
    SpacecraftData::loadSpacecraftNamesProgmem();
    delay(1000);
    Serial.println("Loading spacecraft blacklist...");
    SpacecraftData::loadSpacecraftBlacklistProgmem();
    delay(3000);
    Serial.println("Loading spacecraft placeholder rates...");
    SpacecraftData::loadSpacecraftPlaceholderRatesProgmem();
    delay(1000);
};


// Load spacecraft Names
void SpacecraftData::loadSpacecraftNamesProgmem()
{
    /* Memory warning!
     * The values cannot be longer than 100 characters, or there will be a fatal error.
     * The memory for this variable for animation function is statically allocated
     */

    static const char spacecraftNamesJsonRaw[] = PROGMEM R"RAW-NAMES(
        {
            "ACE": "Advanced Composition Explorer",
            "PLC": "Akatsuki",
            "ARGO": "ArgoMoon",
            "BIOS": "BioSentinel",
            "CHDR": "Chandra Xray Observatory",
            "CH2": "Chandrayaan 2",
            "CUE3": "CU Earth Escape Explorer",
            "CuSP": "CubeSat-Observation of Solar Particles",
            "DART": "DART",
            "unknown10": "Dragonfly",
            "DSCO": "Deep Space Climate Observatory",
            "EMM": "Emirates Mars Mission",
            "EQUL": "EQUilibriUm Lunar-Earth Spacecraft",
            "EURC": "Europa Clipper",
            "RSP": "ExoMars Rover",
            "GAIA": "Gaia",
            "GTL": "Geotail",
            "HYB2": "Hayabusa 2",
            "EM1": "Artemis 1",
            "EM2": "Artemis 2",
            "EM3": "Artemis 3",
            "NSYT": "InSight",
            "JWST": "James Webb Space Telescope",
            "JNO": "Juno",
            "KPLO": "Korea Pathfinder Lunar Orbiter",
            "LICI": "LICIA Cube",
            "LND1": "Lunar Node 1",
            "LUCY": "Lucy",
            "LFL": "Lunar Flashlight",
            "HMAP": "Lunar Hydrogen Mapper",
            "LRO": "Lunar Reconnaissance Orbiter",
            "MMS1": "Magnetospheric MultiScale Formation Flyer 1",
            "MMS2": "Magnetospheric MultiScale Formation Flyer 2",
            "MMS3": "Magnetospheric MultiScale Formation Flyer 3",
            "MMS4": "Magnetospheric MultiScale Formation Flyer 4",
            "M01O": "Mars Odyssey",
            "M20": "Mars 2020",
            "MVN": "MAVEN",
            "MEX": "Mars Express",
            "MOM": "Mars Orbiter",
            "MRO": "Mars Reconnaissance Orbiter",
            "MSL": "Curiosity",
            "MLI": "Morehead Lunar Ice Cube",
            "NEAS": "Near Earth Asteroid Scout",
            "NHPC": "New Horizons",
            "ORX": "OSIRIS REx",
            "OMOT": "OMOTENASHI",
            "PSYC": "Psyche",
            "SOHO": "Solar and Heliospheric Observatory",
            "SPP": "Parker Solar Probe",
            "STA": "STEREO A",
            "TESS": "Transiting Exoplanet Survey Satellite",
            "TGO": "ExoMars Trace Gas Orbiter",
            "THB": "THEMIS B",
            "THC": "THEMIS C",
            "TM": "TeamMiles",
            "VGR1": "Voyager 1",
            "VGR2": "Voyager 2",
            "WIND": "Wind",
            "XMM": "XMM Newton",
            "ATOT": "Advanced Tracking and Observational Techniques",
            "EGS": "EVN and Global Sevices",
            "GBRA": "Ground Based Radio Astronomy",
            "GSSR": "Goldstone Solar System Radar",
            "GVRT": "Goldstone Apple Valley Radio Telescope",
            "SGP": "Space Geodesy Program",
            "TDR6": "Tracking and Data Relay Satellites (TDRS)",
            "TDR7": "Tracking and Data Relay Satellites (TDRS)",
            "TDR8": "Tracking and Data Relay Satellites (TDRS)",
            "TDR9": "Tracking and Data Relay Satellites (TDRS)",
            "TD10": "Tracking and Data Relay Satellites (TDRS)",
            "TD11": "Tracking and Data Relay Satellites (TDRS)",
            "TD12": "Tracking and Data Relay Satellites (TDRS)",
            "TD13": "Tracking and Data Relay Satellites (TDRS)",
            "Rate1": "Test Rate 1",
            "Rate2": "Test Rate 2",
            "Rate3": "Test Rate 3",
            "Rate4": "Test Rate 4",
            "Rate5": "Test Rate 5",
            "Rate6": "Test Rate 6"
        }
    )RAW-NAMES";


    // Check if JSON is too large
    if (strlen_P(spacecraftNamesJsonRaw) > 5120) {
        Serial.print(DevUtils::termColor("red") + "Raw names JSON is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Make sure there is enough heap memory to load names
    if (ESP.getFreeHeap() < 5120) {
        Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load names" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Initialize buffer
    char buffer[5120];
    if (!buffer) {
        Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Copy raw JSON to buffer
    strncpy_P(buffer, spacecraftNamesJsonRaw, 5119);
    buffer[5119] = '\0';


    DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

    if (error) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft callsigns loaded" + DevUtils::termColor("reset") + "\n");
    Serial.println("Rate1: " + spacecraftNamesJson["Rate1"].as<String>());
}


// Load spacecraft blacklist
void SpacecraftData::loadSpacecraftBlacklistProgmem() {
    static const char spacecraftBlacklistJsonRaw[] = PROGMEM R"RAW-BLACKLIST(
        {
            "TEST": true,
            "DSN": true,
            "RFC(VLBI)": true
        }
    )RAW-BLACKLIST";



    // Check if JSON is too large
    if (strlen_P(spacecraftBlacklistJsonRaw) > 1024) {
        Serial.print(DevUtils::termColor("red") + "Raw blacklist JSON is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Make sure there is enough heap memory to load blacklist
    if (ESP.getFreeHeap() < 1024) {
        Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load blacklist" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Initialize buffer
    char buffer[1024];
    if (!buffer) {
        Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Copy raw JSON to buffer
    strncpy_P(buffer, spacecraftBlacklistJsonRaw, 1023);
    buffer[1023] = '\0';


    DeserializationError error = deserializeJson(spacecraftBlacklistJson, buffer);

    if (error) {
        if (FileUtils::config.debugUtils.showSerial)
            Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
}

// Load spacecraft placeholder rates
void SpacecraftData::loadSpacecraftPlaceholderRatesProgmem() {
    Serial.println("Loading spacecraft placeholder rates...");

    static const char spacecraftPlaceholderRatesJsonRaw[] = PROGMEM R"RAW-RATES(
        {
            "CAPS": [
                "7.813e+00",
                "1.250e+02",
                "9.999e+02",
                "4.000e+03"
            ],
            "DART": [
                "7.813e+00",
                "2.000e+03",
                "1.600e+04"
            ],
            "DSSR": [
                "1.000e+03",
                "2.000e+03",
                "4.000e+03",
                "1.600e+04"
            ],
            "JWST": [
                "1.250e+02",
                "2.500e+02",
                "4.000e+03",
                "2.500e+05"
            ],
            "KPLO": [
                "7.813e+00",
                "1.250e+02",
                "2.500e+02",
                "4.000e+03",
                "1.600e+04",
                "2.500e+05"
            ],
            "LICI": [
                "7.813e+00",
                "1.600e+01",
                "5.000e+02",
                "4.000e+03",
                "1.600e+04"
            ],
            "M01O": [
                "7.813e+00",
                "2.500e+02",
                "2.000e+03",
                "2.500e+05"
            ],
            "M20": [
                "7.813e+00",
                "1.250e+02",
                "1.000e+03",
                "2.000e+03",
                "1.600e+04"
            ],
            "MMS1": [
                "7.813e+00",
                "1.250e+02",
                "1.000e+03",
                "2.500e+05"
            ],
            "MMS2": [
                "6.250e+01",
                "2.500e+02",
                "1.000e+03"
            ],
            "MMS3": [
                "2.500e+02",
                "1.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "MMS4": [
                "0.000e+00",
                "2.500e+02",
                "2.000e+03"
            ],
            "MRO": [
                "7.812e+00",
                "6.250e+01",
                "1.250e+02",
                "4.000e+03",
                "1.600e+04"
            ],
            "MSL": [
                "7.813e+00",
                "1.250e+02",
                "4.000e+03",
                "1.600e+04"
            ],
            "MVN": [
                "7.813e+00",
                "9.999e+02",
                "2.000e+03",
                "1.600e+04",
                "2.500e+05"
            ],
            "PLSS": [
                "7.813e+00",
                "6.250e+01",
                "5.000e+02",
                "4.000e+03",
                "1.600e+04",
                "2.500e+05"
            ],
            "SPP": [
                "7.813e+00",
                "6.250e+01",
                "9.999e+02",
                "4.000e+03",
                "3.200e+04"
            ],
            "SOHO": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "1.600e+04",
                "2.500e+05"
            ],
            "TESS": [
                "7.813e+00",
                "4.000e+03",
                "1.600e+04",
                "2.500e+05"
            ],
            "THEMIS-A": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "THEMIS-B": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "THEMIS-C": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "THEMIS-D": [
                "7.813e+00",
                "9.999e+02",
                "1.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "THEMIS-E": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "3.200e+04",
                "2.500e+05"
            ],
            "VIK1": [
                "2.500e+02",
                "4.000e+03"
            ],
            "VIK2": [
                "2.500e+02",
                "4.000e+03"
            ],
            "WIND": [
                "7.813e+00",
                "9.999e+02",
                "4.000e+03",
                "1.600e+04",
                "2.500e+05"
            ]
        }
    )RAW-RATES";


    // Check if JSON is too large
    size_t jsonLength = strlen_P(spacecraftPlaceholderRatesJsonRaw);
    Serial.println("JSON length: " + String(jsonLength));
    if (jsonLength > 6144) {
        Serial.print(DevUtils::termColor("red") + "placeholder_rates.json is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // Make sure there is enough heap memory to load placeholder rates
    Serial.println("Free heap: " + String(ESP.getFreeHeap()));
    if (ESP.getFreeHeap() < 6144) {
        Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load placeholder rates" + DevUtils::termColor("reset") + "\n");
        return;
    }

    delay(1000);

    // Initialize buffer
    Serial.println("Initializing buffer...");
    try {
        char buffer[6144];

        // Copy raw JSON to buffer
        Serial.println("Copying raw JSON to buffer...");
        strncpy_P(buffer, spacecraftPlaceholderRatesJsonRaw, 6143);
        buffer[6143] = '\0';

        Serial.println("Deserializing JSON...");
        DeserializationError error = deserializeJson(spacecraftPlaceholderRatesJson, buffer);

        if (error) {
            Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize placeholder rates raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
            delay(10000);
            sleep(10);
            return;
        }

        Serial.println("CAPS[0]" + String(spacecraftPlaceholderRatesJson["CAPS"][3].as<String>()));
        delay(1000);
        Serial.println("Are we there yet?");
        delay(1000);
        Serial.print(DevUtils::termColor("green") + "Spacecraft placeholder rates loaded" + DevUtils::termColor("reset") + "\n");
    }
    catch (const std::exception& e) {
        Serial.println("Error: " + String(e.what()));
        Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
        return;
    }


}


/* Check Name */
const char* SpacecraftData::callsignToName(const char* key) {
    Serial.print("\n\n---------------------\n");
    Serial.print("-- Spacecraft Names --\n");
    // print every key in spacecraftNamesJson
    for (JsonPair kv : spacecraftNamesJson.as<JsonObject>()) {
        Serial.println(kv.key().c_str());
        break;
    }

    delay(10000);

    // Look for key in JSON
    Serial.println("Rate1: " + spacecraftNamesJson["Rate1"].as<String>());
    Serial.println("Checking names lookup for " + String(key) + "...");
    if (spacecraftNamesJson.containsKey(key)) {
        Serial.println("Found spacecraft name for " + String(key));
        const char* spacecraftName = spacecraftNamesJson[key];
        return spacecraftName;
    }
    Serial.println("Spacecraft name not found for " + String(key));

    // Spacecraft name not found, return the callsign
    return key;
}


/* Check Blacklist */
bool SpacecraftData::checkBlacklist(const char* key) {
    Serial.println("Blacklist:");
    for (JsonPair kv : spacecraftBlacklistJson.as<JsonObject>()) {
        Serial.println(kv.key().c_str());
    }
    Serial.print("\n\n");

    bool isBlacklisted = spacecraftBlacklistJson[key] != nullptr ? true : false; // If the key is not null, it is blacklisted

    if (FileUtils::config.debugUtils.showSerial)
        Serial.println(DevUtils::termColor("bg_bright_black") + ">>> Checking blacklist for " + String(key) + ": " + (isBlacklisted ? "true" : "false") + DevUtils::termColor("reset") + "\n");


    return spacecraftBlacklistJson[key] != nullptr ? true : false;
}





/* Get placeholder rate */
const char* SpacecraftData::getPlaceholderRate(const char* key) {
    // Look for key in JSON
    if (spacecraftPlaceholderRatesJson.containsKey(key)) {
        Serial.println("Found spacecraft rate for " + String(key));

        // Get JsonVariant at key
        JsonVariant variant = spacecraftPlaceholderRatesJson[key];

        // Check if the JsonVariant is an array
        if (variant.is<JsonArray>()) {
            Serial.println("It's an array");
            JsonArray arr = variant.as<JsonArray>();

            // Generate a random index within the bounds of the array using random8
            int randomIndex = random8(arr.size());
            Serial.println("Random index: " + String(randomIndex));

            // print all items in array
            // for (JsonVariant v : arr) {
            //     Serial.println(v.as<String>());
            // }
            Serial.print("\n\n");
            Serial.println("Array size: " + String(arr.size()));

            // Get the rate at the random index
            const char* spacecraftRate = arr[randomIndex];
            Serial.println("Spacecraft rate: " + String(spacecraftRate));

            return spacecraftRate;
        } else {
            Serial.println("It's not an array");
            // If it's not an array, assume it's a single rate and return it
            const char* spacecraftRate = variant.as<const char*>();
            return spacecraftRate;
        }
    }

    Serial.println("Spacecraft rate not found for " + String(key));

    // Spacecraft rate not found, return the default rate
    return "1.000e+03";
}
























// #include <SpacecraftData.h>


// // Constructor 
// SpacecraftData::SpacecraftData()
//     : spacecraftNamesJson(5120), spacecraftPlaceholderRatesJson(8192), spacecraftBlacklistJson(1024) {}

// void SpacecraftData::loadJson() {
//     loadSpacecraftNamesRaw();
//     // delay(1000);
//     loadSpacecraftPlaceholderRatesRaw();
//     // delay(1000);
//     loadSpacecraftBlacklistRaw();
// }

// /**
//  * Spacecraft Callsign to Name
//  */
// // void SpacecraftData::loadSpacecraftNames()
// // {
// //     /* Memory warning!
// //      * The values cannot be longer than 100 characters, or there will be a fatal error.
// //      * The memory for this variable for animation function is statically allocated
// //      */
// //     File file = LittleFS.open("/spacecraft_data/names.json", "r");

// //     if (!file) {
// //         Serial.print(DevUtils::termColor("red") + "Failed trying to open names.json with error: " + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }

// //     size_t fileSize = file.size();
// //     if (fileSize > 5120) {
// //         Serial.print(DevUtils::termColor("red") + "names.json is too large" + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }

// //     // StaticJsonDocument<5120> spacecraftNamesJson;
// //     char buffer[5120];
// //     strcpy(buffer, file.readString().c_str());

// //     DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

// //     if (error) {
// //         Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }
// //     Serial.print(DevUtils::termColor("green") + "Spacecraft callsigns loaded" + DevUtils::termColor("reset") + "\n");
// // }


// /**
//  * Spacecraft Placeholder Rates
//  */
// // void SpacecraftData::loadSpacecraftPlaceholderRates() {
// //     Serial.println("Loading spacecraft placeholder rates...");
// //     Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

// //     File file = LittleFS.open("/spacecraft_data/placeholder_rates.json", "r");

// //     Serial.println("File opened");
// //     Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

// //     if (!file) {
// //         Serial.print(DevUtils::termColor("red") + "Failed trying to open placeholder_rates.json with error: " + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }

// //     size_t fileSize = file.size();
// //     if (fileSize > 8192) {
// //         Serial.print(DevUtils::termColor("red") + "placeholder_rates.json is too large" + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }

// //     // DynamicJsonDocument spacecraftPlaceholderRatesJson(8192);

// //     char buffer[8192];
// //     strcpy(buffer, file.readString().c_str());
// //     DeserializationError error = deserializeJson(spacecraftPlaceholderRatesJson, buffer);

// //     if (error) {
// //         Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize placeholder_rates.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }
// //     Serial.print(DevUtils::termColor("green") + "Spacecraft placeholder rates loaded" + DevUtils::termColor("reset") + "\n");
// // }

// void SpacecraftData::loadSpacecraftPlaceholderRatesRaw() {
//     Serial.println("Loading spacecraft placeholder rates...");
//     Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

//     static const char spacecraftPlaceholderRatesJsonRaw[] = R"RAW(
//         {
//             "CAPS": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03"
//             ],
//             "DART": [
//                 "7.813e+00",
//                 "2.000e+03",
//                 "1.600e+04"
//             ],
//             "DSSR": [
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04"
//             ],
//             "JWST": [
//                 "1.250e+02",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "KPLO": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "LICI": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "5.000e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04"
//             ],
//             "M01O": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "M20": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04"
//             ],
//             "MMS1": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "2.500e+05"
//             ],
//             "MMS2": [
//                 "6.250e+01",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03"
//             ],
//             "MMS3": [
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "MMS4": [
//                 "0.000e+00",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03"
//             ],
//             "MRO": [
//                 "7.812e+00",
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04"
//             ],
//             "MSL": [
//                 "7.813e+00",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04"
//             ],
//             "MVN": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "PLSS": [
//                 "7.813e+00",
//                 "1.600e+01",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "SPP": [
//                 "7.813e+00",
//                 "6.250e+01",
//                 "1.250e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04"
//             ],
//             "SOHO": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "TESS": [
//                 "7.813e+00",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ],
//             "THEMIS-A": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "THEMIS-B": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "THEMIS-C": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "THEMIS-D": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "THEMIS-E": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "5.000e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "3.200e+04",
//                 "2.500e+05"
//             ],
//             "VIK1": [
//                 "1.250e+02",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03"
//             ],
//             "VIK2": [
//                 "1.250e+02",
//                 "2.500e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03"
//             ],
//             "WIND": [
//                 "7.813e+00",
//                 "1.250e+02",
//                 "2.500e+02",
//                 "9.999e+02",
//                 "1.000e+03",
//                 "2.000e+03",
//                 "4.000e+03",
//                 "1.600e+04",
//                 "2.500e+05"
//             ]
//         }
//     )RAW";


//     Serial.println("JSON length: " + String(strlen(spacecraftPlaceholderRatesJsonRaw)));


//     if (strlen(spacecraftPlaceholderRatesJsonRaw) > 8192) {
//         Serial.print(DevUtils::termColor("red") + "placeholder_rates.json is too large" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     if (ESP.getFreeHeap() < 8192) {
//         Serial.print(DevUtils::termColor("red") + "Not enough heap memory to load placeholder rates" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     char* buffer = (char*)malloc(8192 * sizeof(char));
//     if (!buffer) {
//         Serial.print(DevUtils::termColor("red") + "Failed to allocate memory for buffer" + DevUtils::termColor("reset") + "\n");
//         return;
//     }

//     strncpy(buffer, spacecraftPlaceholderRatesJsonRaw, 8191);
//     buffer[8191] = '\0';

//     // DynamicJsonDocument spacecraftPlaceholderRatesJson(8192);
//     DeserializationError error = deserializeJson(spacecraftPlaceholderRatesJson, buffer);

//     free(buffer); // Free the dynamically allocated memory


//     if (error) {
//         Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize placeholder rates raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
//         return;
//     }
//     Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
//     Serial.print(DevUtils::termColor("green") + "Spacecraft placeholder rates loaded" + DevUtils::termColor("reset") + "\n");
// }



// /**
//  * Spacecraft Blacklist
//  *
//  * Returns true if spacecraft is on blacklist
//  */
// // void SpacecraftData::loadSpacecraftBlacklist() {
// //     File file = LittleFS.open("/spacecraft_data/blacklist.json", "r");

// //     if (!file) {
// //         Serial.print(DevUtils::termColor("red") + "Failed trying to open blacklist.json with error: " + DevUtils::termColor("reset") + "\n");
// //         return;
// //     }

// //     char buffer[1024];
// //     strcpy(buffer, file.readString().c_str());
// //     // StaticJsonDocument<1024> spacecraftBlacklistJson;
// //     DeserializationError error = deserializeJson(spacecraftBlacklistJson, buffer);

// //     if (error) {
// //         if (FileUtils::config.debugUtils.showSerial)
// //             Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

// //         return;
// //     }
// //     Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
// // }








// /* Get placeholder rate */
// const char* SpacecraftData::getPlaceholderRate(const char* key) {
//     // Look for key in JSON
//     if (spacecraftPlaceholderRatesJson.containsKey(key)) {
//         Serial.println("Found spacecraft rate for " + String(key));

//         // Get JsonVariant at key
//         JsonVariant variant = spacecraftPlaceholderRatesJson[key];

//         // Check if the JsonVariant is an array
//         if (variant.is<JsonArray>()) {
//             Serial.println("It's an array");
//             JsonArray arr = variant.as<JsonArray>();

//             // Generate a random index within the bounds of the array using random8
//             int randomIndex = random8(arr.size());
//             Serial.println("Random index: " + String(randomIndex));

//             // Get the rate at the random index
//             const char* spacecraftRate = arr[randomIndex];
//             Serial.println("Spacecraft rate: " + String(spacecraftRate));

//             return spacecraftRate;
//         } else {
//             Serial.println("It's not an array");
//             // If it's not an array, assume it's a single rate and return it
//             const char* spacecraftRate = variant.as<const char*>();
//             return spacecraftRate;
//         }
//     }

//     Serial.println("Spacecraft rate not found for " + String(key));

//     // Spacecraft rate not found, return the default rate
//     return "1.000e+03";
// }

