#include <SpacecraftData.h>

DynamicJsonDocument SpacecraftData::spacecraftNamesJson(5120);
DynamicJsonDocument SpacecraftData::spacecraftBlacklistJson(1024);

void SpacecraftData::loadJson(const char* firmwareVersion) {
    Serial.println("Initializing spacecraft data...");
    populateNamesInMemory();
    populateBlacklistInMemory();

    if (shouldWriteFiles(firmwareVersion)) {
        Serial.println("Firmware version changed — writing spacecraft data to flash");
        writeFilesToFlash(firmwareVersion);
    } else {
        Serial.println("Spacecraft data unchanged — skipping flash writes");
    }
}

void SpacecraftData::populateNamesInMemory() {
    spacecraftNamesJson.clear();
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
    spacecraftNamesJson["SWFO"] = "SOLAR 1";
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
}

void SpacecraftData::populateBlacklistInMemory() {
    spacecraftBlacklistJson.clear();
    spacecraftBlacklistJson["TEST"] = true;
    spacecraftBlacklistJson["DSN"] = true;
    spacecraftBlacklistJson["RFC(VLBI)"] = true;
    spacecraftBlacklistJson["GO19"] = true;
}

bool SpacecraftData::shouldWriteFiles(const char* firmwareVersion) {
    File versionFile = LittleFS.open("/spacecraft_data/version.txt", "r");
    if (!versionFile) {
        return true; // File doesn't exist — first boot or fresh flash
    }

    String storedVersion = versionFile.readStringUntil('\n');
    versionFile.close();

    if (storedVersion.length() == 0) {
        return true; // Empty or corrupted file
    }

    return strcmp(storedVersion.c_str(), firmwareVersion) != 0;
}

void SpacecraftData::writeFilesToFlash(const char* firmwareVersion) {
    FileUtils::createDir("spacecraft_data");

    // Write names.json
    File namesFile = LittleFS.open("/spacecraft_data/names.json", "w");
    if (namesFile) {
        if (serializeJson(spacecraftNamesJson, namesFile) == 0) {
            Serial.print(DevUtils::termColor("red") + "Failed to write names.json" + DevUtils::termColor("reset") + "\n");
        }
        namesFile.close();
    } else {
        Serial.print(DevUtils::termColor("red") + "Failed to open names.json for writing" + DevUtils::termColor("reset") + "\n");
    }

    // Write blacklist.json
    File blacklistFile = LittleFS.open("/spacecraft_data/blacklist.json", "w");
    if (blacklistFile) {
        if (serializeJson(spacecraftBlacklistJson, blacklistFile) == 0) {
            Serial.print(DevUtils::termColor("red") + "Failed to write blacklist.json" + DevUtils::termColor("reset") + "\n");
        }
        blacklistFile.close();
    } else {
        Serial.print(DevUtils::termColor("red") + "Failed to open blacklist.json for writing" + DevUtils::termColor("reset") + "\n");
    }

    // Write version marker
    File versionFile = LittleFS.open("/spacecraft_data/version.txt", "w");
    if (versionFile) {
        versionFile.print(firmwareVersion);
        versionFile.close();
    } else {
        Serial.print(DevUtils::termColor("red") + "Failed to write version.txt" + DevUtils::termColor("reset") + "\n");
    }

    Serial.print(DevUtils::termColor("green") + "Spacecraft data written to flash" + DevUtils::termColor("reset") + "\n");
}

/* Check Name */
const char* SpacecraftData::callsignToName(const char* key) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;

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
        char buffer[128];
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
