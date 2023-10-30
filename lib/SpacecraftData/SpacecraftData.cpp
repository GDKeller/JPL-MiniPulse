#include <SpacecraftData.h>

/* JSON Data */
// static const char spacecraftNamesJsonRaw[] = PROGMEM R"=---=(
//     {
//         "ACE": "Advanced Composition Explorer",
//         "PLC": "Akatsuki",
//         "ARGO": "ArgoMoon",
//         "BIOS": "BioSentinel",
//         "CHDR": "Chandra Xray Observatory",
//         "CH2": "Chandrayaan 2",
//         "CUE3": "CU Earth Escape Explorer",
//         "CuSP": "CubeSat-Observation of Solar Particles",
//         "DART": "DART",
//         "unknown10": "Dragonfly",
//         "DSCO": "Deep Space Climate Observatory",
//         "EMM": "Emirates Mars Mission",
//         "EQUL": "EQUilibriUm Lunar-Earth Spacecraft",
//         "EURC": "Europa Clipper",
//         "RSP": "ExoMars Rover",
//         "GAIA": "Gaia",
//         "GTL": "Geotail",
//         "HYB2": "Hayabusa 2",
//         "EM1": "Artemis 1",
//         "EM2": "Artemis 2",
//         "EM3": "Artemis 3",
//         "NSYT": "InSight",
//         "JWST": "James Webb Space Telescope",
//         "JNO": "Juno",
//         "KPLO": "Korea Pathfinder Lunar Orbiter",
//         "LICI": "LICIA Cube",
//         "LND1": "Lunar Node 1",
//         "LUCY": "Lucy",
//         "LFL": "Lunar Flashlight",
//         "HMAP": "Lunar Hydrogen Mapper",
//         "LRO": "Lunar Reconnaissance Orbiter",
//         "MMS1": "Magnetospheric MultiScale Formation Flyer 1",
//         "MMS2": "Magnetospheric MultiScale Formation Flyer 2",
//         "MMS3": "Magnetospheric MultiScale Formation Flyer 3",
//         "MMS4": "Magnetospheric MultiScale Formation Flyer 4",
//         "M01O": "Mars Odyssey",
//         "M20": "Mars 2020",
//         "MVN": "MAVEN",
//         "MEX": "Mars Express",
//         "MOM": "Mars Orbiter",
//         "MRO": "Mars Reconnaissance Orbiter",
//         "MSL": "Curiosity",
//         "MLI": "Morehead Lunar Ice Cube",
//         "NEAS": "Near Earth Asteroid Scout",
//         "NHPC": "New Horizons",
//         "ORX": "OSIRIS REx",
//         "OMOT": "OMOTENASHI",
//         "PSYC": "Psyche",
//         "SOHO": "Solar and Heliospheric Observatory",
//         "SPP": "Parker Solar Probe",
//         "STA": "STEREO A",
//         "TESS": "Transiting Exoplanet Survey Satellite",
//         "TGO": "ExoMars Trace Gas Orbiter",
//         "THB": "THEMIS B",
//         "THC": "THEMIS C",
//         "TM": "TeamMiles",
//         "VGR1": "Voyager 1",
//         "VGR2": "Voyager 2",
//         "WIND": "Wind",
//         "XMM": "XMM Newton",
//         "ATOT": "Advanced Tracking and Observational Techniques",
//         "EGS": "EVN and Global Sevices",
//         "GBRA": "Ground Based Radio Astronomy",
//         "GSSR": "Goldstone Solar System Radar",
//         "GVRT": "Goldstone Apple Valley Radio Telescope",
//         "SGP": "Space Geodesy Program",
//         "TDR6": "Tracking and Data Relay Satellites (TDRS)",
//         "TDR7": "Tracking and Data Relay Satellites (TDRS)",
//         "TDR8": "Tracking and Data Relay Satellites (TDRS)",
//         "TDR9": "Tracking and Data Relay Satellites (TDRS)",
//         "TD10": "Tracking and Data Relay Satellites (TDRS)",
//         "TD11": "Tracking and Data Relay Satellites (TDRS)",
//         "TD12": "Tracking and Data Relay Satellites (TDRS)",
//         "TD13": "Tracking and Data Relay Satellites (TDRS)",
//         "Rate1": "Test Rate 1",
//         "Rate2": "Test Rate 2",
//         "Rate3": "Test Rate 3",
//         "Rate4": "Test Rate 4",
//         "Rate5": "Test Rate 5",
//         "Rate6": "Test Rate 6"
//     }
// )=---=";

// static const char spacecraftPlaceholderRatesJsonRaw[] = R"RAW(
//     {
//         "CAPS": [
//             "7.813e+00",
//             "1.250e+02",
//             "2.500e+02",
//             "5.000e+02",
//             "9.999e+02",
//             "1.000e+03",
//             "2.000e+03",
//             "4.000e+03"
//         ]
//     }
// )RAW";

// static const char spacecraftBlacklistJsonRaw[] = PROGMEM R"=---=(
//     {
//         "TEST": true,
//         "DSN": true,
//         "RFC(VLBI)": "Reference Frame Calibration"
//     }
// )=---=";


// Constructor 
SpacecraftData::SpacecraftData()
    : spacecraftNamesJson(5120), spacecraftBlacklistJson(1024), spacecraftPlaceholderRatesJson(8192) {}

void SpacecraftData::loadJson() {
    loadSpacecraftNamesRaw();
    delay(1000);
    // loadSpacecraftPlaceholderRates();
    delay(1000);
    loadSpacecraftBlacklistRaw();
}

/**
 * Spacecraft Callsign to Name
 */
void SpacecraftData::loadSpacecraftNames()
{
    /* Memory warning!
     * The values cannot be longer than 100 characters, or there will be a fatal error.
     * The memory for this variable for animation function is statically allocated
     */
    File file = LittleFS.open("/spacecraft_data/names.json", "r");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open names.json with error: " + DevUtils::termColor("reset") + "\n");
        return;
    }

    size_t fileSize = file.size();
    if (fileSize > 5120) {
        Serial.print(DevUtils::termColor("red") + "names.json is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // StaticJsonDocument<5120> spacecraftNamesJson;
    char buffer[5120];
    strcpy(buffer, file.readString().c_str());

    DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

    if (error) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft callsigns loaded" + DevUtils::termColor("reset") + "\n");
}

void SpacecraftData::loadSpacecraftNamesRaw()
{
    /* Memory warning!
     * The values cannot be longer than 100 characters, or there will be a fatal error.
     * The memory for this variable for animation function is statically allocated
     */

    static const char spacecraftNamesJsonRaw[] = PROGMEM R"=---=(
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
    )=---=";

    // delay(1000);


    if (spacecraftNamesJsonRaw == nullptr) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open names raw JSON with error: " + DevUtils::termColor("reset") + "\n");
        return;
    }

    // size check
    if (strlen(spacecraftNamesJsonRaw) > 5120) {
        Serial.print(DevUtils::termColor("red") + "Raw names JSON is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // StaticJsonDocument<5120> spacecraftNamesJson;
    char buffer[5120];
    strcpy(buffer, spacecraftNamesJsonRaw);

    DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

    if (error) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize names raw JSON with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft callsigns loaded" + DevUtils::termColor("reset") + "\n");
}


/**
 * Spacecraft Placeholder Rates
 */
void SpacecraftData::loadSpacecraftPlaceholderRates() {
    Serial.println("Loading spacecraft placeholder rates...");
    Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

    File file = LittleFS.open("/spacecraft_data/placeholder_rates.json", "r");

    Serial.println("File opened");
    Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open placeholder_rates.json with error: " + DevUtils::termColor("reset") + "\n");
        return;
    }

    size_t fileSize = file.size();
    if (fileSize > 8192) {
        Serial.print(DevUtils::termColor("red") + "placeholder_rates.json is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    // DynamicJsonDocument spacecraftPlaceholderRatesJson(8192);

    char buffer[8192];
    strcpy(buffer, file.readString().c_str());
    DeserializationError error = deserializeJson(spacecraftPlaceholderRatesJson, buffer);

    if (error) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize placeholder_rates.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");
        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft placeholder rates loaded" + DevUtils::termColor("reset") + "\n");
}


/**
 * Spacecraft Blacklist
 *
 * Returns true if spacecraft is on blacklist
 */
void SpacecraftData::loadSpacecraftBlacklist() {
    File file = LittleFS.open("/spacecraft_data/blacklist.json", "r");

    if (!file) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open blacklist.json with error: " + DevUtils::termColor("reset") + "\n");
        return;
    }

    char buffer[1024];
    strcpy(buffer, file.readString().c_str());
    // StaticJsonDocument<1024> spacecraftBlacklistJson;
    DeserializationError error = deserializeJson(spacecraftBlacklistJson, buffer);

    if (error) {
        if (FileUtils::config.debugUtils.showSerial)
            Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
}

void SpacecraftData::loadSpacecraftBlacklistRaw() {
    static const char spacecraftBlacklistJsonRaw[] = PROGMEM R"=---=(
        {
            "TEST": true,
            "DSN": true,
            "RFC(VLBI)": "Reference Frame Calibration"
        }
    )=---=";

    if (spacecraftBlacklistJsonRaw == nullptr) {
        Serial.print(DevUtils::termColor("red") + "Failed trying to open blacklist.json with error: " + DevUtils::termColor("reset") + "\n");
        return;
    }

    if (strlen(spacecraftBlacklistJsonRaw) > 1024) {
        Serial.print(DevUtils::termColor("red") + "Raw blacklist JSON is too large" + DevUtils::termColor("reset") + "\n");
        return;
    }

    char buffer[1024];
    strcpy(buffer, spacecraftBlacklistJsonRaw);
    // StaticJsonDocument<1024> spacecraftBlacklistJson;
    DeserializationError error = deserializeJson(spacecraftBlacklistJson, buffer);

    if (error) {
        if (FileUtils::config.debugUtils.showSerial)
            Serial.print(DevUtils::termColor("red") + "Failed trying to deserialize blacklist.json with error: " + error.c_str() + DevUtils::termColor("reset") + "\n");

        return;
    }
    Serial.print(DevUtils::termColor("green") + "Spacecraft blacklist loaded" + DevUtils::termColor("reset") + "\n");
}




/* Check Name */
const char* SpacecraftData::callsignToName(const char* key) {
    // Look for key in JSON
    if (spacecraftNamesJson.containsKey(key)) {
        const char* spacecraftName = spacecraftNamesJson[key];
        return spacecraftName;
    }

    // Spacecraft name not found, return the callsign
    return key;
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


/* Check Blacklist */
bool SpacecraftData::checkBlacklist(const char* key) {
    bool isBlacklisted = spacecraftBlacklistJson[key] != nullptr ? true : false; // If the key is not null, it is blacklisted

    if (FileUtils::config.debugUtils.showSerial)
        Serial.println(DevUtils::termColor("bg_bright_black") + ">>> Checking blacklist for " + String(key) + ": " + (isBlacklisted ? "true" : "false") + DevUtils::termColor("reset") + "\n");


    return spacecraftBlacklistJson[key] != nullptr ? true : false;
}