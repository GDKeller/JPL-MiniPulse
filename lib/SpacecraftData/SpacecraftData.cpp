#include <SpacecraftData.h>

DynamicJsonDocument SpacecraftData::spacecraftNamesJson(8192);
DynamicJsonDocument SpacecraftData::spacecraftBlacklistJson(1024);
DynamicJsonDocument SpacecraftData::spacecraftApprovedJson(2048);

void SpacecraftData::loadJson(const char* firmwareVersion) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    if (showSerial) Serial.println("Initializing spacecraft data...");
    populateNamesInMemory();
    populateBlacklistInMemory();
    populateApprovedInMemory();

    if (shouldWriteFiles(firmwareVersion)) {
        if (showSerial) Serial.println("Firmware version changed — writing spacecraft data to flash");
        writeFilesToFlash(firmwareVersion);
    } else {
        if (showSerial) Serial.println("Spacecraft data unchanged — skipping flash writes");
    }
}

void SpacecraftData::populateNamesInMemory() {
    spacecraftNamesJson.clear();

    // Spacecraft (alphabetical by callsign)
    spacecraftNamesJson["ACE"] = "Adv Composition Explorer";
    spacecraftNamesJson["AGM1"] = "Griffin Lander";
    spacecraftNamesJson["AIRS"] = "AIRS";
    spacecraftNamesJson["B101"] = "Blue Moon 1 SN1";
    spacecraftNamesJson["B102"] = "Blue Moon 1 SN2";
    spacecraftNamesJson["BEPI"] = "BepiColombo";
    spacecraftNamesJson["BIOS"] = "BioSentinel";
    spacecraftNamesJson["CAPS"] = "CAPSTONE";
    spacecraftNamesJson["CGO"] = "Carruthers Observatory";
    spacecraftNamesJson["CHDR"] = "Chandra Xray";
    spacecraftNamesJson["CMA"] = "Caltech Apophis";
    spacecraftNamesJson["DAVINCI"] = "DAVINCI";
    spacecraftNamesJson["DFLY"] = "Dragonfly";
    spacecraftNamesJson["DSCO"] = "DSCOVR";
    spacecraftNamesJson["EM2"] = "Artemis Orion 2";
    spacecraftNamesJson["EM3"] = "Artemis Orion 3";
    spacecraftNamesJson["EMM"] = "Emirates Mars";
    spacecraftNamesJson["ERO"] = "Earth Return Orb";
    spacecraftNamesJson["ESCB"] = "EscaPADE Blue";
    spacecraftNamesJson["ESCG"] = "EscaPADE Gold";
    spacecraftNamesJson["EUCL"] = "Euclid";
    spacecraftNamesJson["EURC"] = "Europa Clipper";
    spacecraftNamesJson["EUS1"] = "Explor Upper Stage";
    spacecraftNamesJson["GWP"] = "Lunar Gateway";
    spacecraftNamesJson["HERA"] = "Hera";
    spacecraftNamesJson["HST"] = "Hubble";
    spacecraftNamesJson["HYB2"] = "Hayabusa 2";
    spacecraftNamesJson["IM3"] = "Intuitive Mach 3";
    spacecraftNamesJson["IM4"] = "Intuitive Mach 4";
    spacecraftNamesJson["IMAP"] = "IMAP";
    spacecraftNamesJson["INTG"] = "INTEGRAL";
    spacecraftNamesJson["JNO"] = "Juno Jupiter";
    spacecraftNamesJson["JUICE"] = "JUICE";
    spacecraftNamesJson["JWST"] = "James Webb Space Telescope";
    spacecraftNamesJson["KPLO"] = "Danuri Korea";
    spacecraftNamesJson["LEMS"] = "Lunar Monitor Stn";
    spacecraftNamesJson["LEV1"] = "Lunar Excur Veh";
    spacecraftNamesJson["LRO"] = "Lunar Recon Orbiter";
    spacecraftNamesJson["LTV"] = "Lunar Terrain Veh";
    spacecraftNamesJson["LUCY"] = "Lucy";
    spacecraftNamesJson["LUPX"] = "LUPEX";
    spacecraftNamesJson["M01O"] = "Mars Odyssey";
    spacecraftNamesJson["M20"] = "Mars Perseverance";
    spacecraftNamesJson["MAX"] = "Multi Asteroid Exp";
    spacecraftNamesJson["MEX"] = "Mars Express";
    spacecraftNamesJson["MMS1"] = "Magneto MultiScale 1";
    spacecraftNamesJson["MMS2"] = "Magneto MultiScale 2";
    spacecraftNamesJson["MMS3"] = "Magneto MultiScale 3";
    spacecraftNamesJson["MMS4"] = "Magneto MultiScale 4";
    spacecraftNamesJson["MMX"] = "Martian Moons Exp";
    spacecraftNamesJson["MOM"] = "Mars Orbiter";
    spacecraftNamesJson["MRO"] = "Mars Recon Orbiter";
    spacecraftNamesJson["MSL"] = "Mars Curiosity";
    spacecraftNamesJson["MVN"] = "MAVEN";
    spacecraftNamesJson["NEOS"] = "NEO Surveyor";
    spacecraftNamesJson["NHPC"] = "New Horizons Pluto";
    spacecraftNamesJson["ORACLE-P"] = "Oracle P";
    spacecraftNamesJson["ORX"] = "OSIRIS APEX";
    spacecraftNamesJson["PLC"] = "Akatsuki";
    spacecraftNamesJson["PSYC"] = "Psyche Asteroid";
    spacecraftNamesJson["RFM"] = "Rosalind Franklin";
    spacecraftNamesJson["RLVM"] = "Rocket Lab Venus";
    spacecraftNamesJson["RST"] = "Roman Telescope";
    spacecraftNamesJson["SMAP"] = "SMAP";
    spacecraftNamesJson["SOHO"] = "Solar Heliospheric Observatory";
    spacecraftNamesJson["SOLAR"] = "SOLAR";
    spacecraftNamesJson["SOLO"] = "Solar Orbiter";
    spacecraftNamesJson["SPP"] = "Parker Solar Probe";
    spacecraftNamesJson["SRBB"] = "SunRISE Bebop";
    spacecraftNamesJson["SRED"] = "SunRISE Edward";
    spacecraftNamesJson["SREI"] = "SunRISE Ein";
    spacecraftNamesJson["SRFY"] = "SunRISE Faye";
    spacecraftNamesJson["SRJT"] = "SunRISE Jet";
    spacecraftNamesJson["SRL"] = "Sample Return Lndr";
    spacecraftNamesJson["SRSP"] = "SunRISE Spike";
    spacecraftNamesJson["STA"] = "STEREO A";
    spacecraftNamesJson["SWFO"] = "SWFO L1";
    spacecraftNamesJson["SX01"] = "Starship Uncrewed";
    spacecraftNamesJson["SX02"] = "Starship Crewed";
    spacecraftNamesJson["TESS"] = "TESS Exoplanet Survey";
    spacecraftNamesJson["TGO"] = "ExoMars TGO";
    spacecraftNamesJson["THB"] = "Themis B";
    spacecraftNamesJson["THC"] = "Themis C";
    spacecraftNamesJson["VERITAS"] = "VERITAS";
    spacecraftNamesJson["VGR1"] = "Voyager 1";
    spacecraftNamesJson["VGR2"] = "Voyager 2";
    spacecraftNamesJson["VPR"] = "VIPER";
    spacecraftNamesJson["WIND"] = "Wind";
    spacecraftNamesJson["XMM"] = "XMM Newton";

    // Blacklist / utility (alphabetical by callsign)
    spacecraftNamesJson["ATOT"] = "Advanced Tracking and Observational Techniques";
    spacecraftNamesJson["DSSR"] = "DSN Solar System Radar";
    spacecraftNamesJson["EGS"] = "European and Global VLBI Systems";
    spacecraftNamesJson["GBRA"] = "Ground Based Radio Astronomy";
    spacecraftNamesJson["GSSR"] = "Goldstone Solar System Radar";
    spacecraftNamesJson["GVRT"] = "Goldstone Apple Valley Radio Telescope";
    spacecraftNamesJson["HCRA"] = "Host Country Radio Astronomy";
    spacecraftNamesJson["RFC"] = "Reference Frame Calibration";
    spacecraftNamesJson["SGP"] = "Space Geodesy Program";

    // TDRS (numerical order)
    spacecraftNamesJson["TDR6"] = "TDRS 6";
    spacecraftNamesJson["TDR7"] = "TDRS 7";
    spacecraftNamesJson["TDR8"] = "TDRS 8";
    spacecraftNamesJson["TDR9"] = "TDRS 9";
    spacecraftNamesJson["TD10"] = "TDRS 10";
    spacecraftNamesJson["TD11"] = "TDRS 11";
    spacecraftNamesJson["TD12"] = "TDRS 12";
    spacecraftNamesJson["TD13"] = "TDRS 13";

    // Test rates
    spacecraftNamesJson["Rate1"] = "Test Rate 1";
    spacecraftNamesJson["Rate2"] = "Test Rate 2";
    spacecraftNamesJson["Rate3"] = "Test Rate 3";
    spacecraftNamesJson["Rate4"] = "Test Rate 4";
    spacecraftNamesJson["Rate5"] = "Test Rate 5";
    spacecraftNamesJson["Rate6"] = "Test Rate 6";
}

void SpacecraftData::populateBlacklistInMemory() {
    spacecraftBlacklistJson.clear();
    spacecraftBlacklistJson["DSN"] = true;
    spacecraftBlacklistJson["DSSR"] = true;
    spacecraftBlacklistJson["GO19"] = true;
    spacecraftBlacklistJson["HCRA"] = true;
    spacecraftBlacklistJson["RFC"] = true;
    spacecraftBlacklistJson["RFC(VLBI)"] = true;
    spacecraftBlacklistJson["TEST"] = true;
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
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    FileUtils::createDir("spacecraft_data");

    // Write names.json
    File namesFile = LittleFS.open("/spacecraft_data/names.json", "w");
    if (namesFile) {
        if (serializeJson(spacecraftNamesJson, namesFile) == 0) {
            if (showSerial) Serial.print(DevUtils::termColor("red") + "Failed to write names.json" + DevUtils::termColor("reset") + "\n");
        }
        namesFile.close();
    } else {
        if (showSerial) Serial.print(DevUtils::termColor("red") + "Failed to open names.json for writing" + DevUtils::termColor("reset") + "\n");
    }

    // Write blacklist.json
    File blacklistFile = LittleFS.open("/spacecraft_data/blacklist.json", "w");
    if (blacklistFile) {
        if (serializeJson(spacecraftBlacklistJson, blacklistFile) == 0) {
            if (showSerial) Serial.print(DevUtils::termColor("red") + "Failed to write blacklist.json" + DevUtils::termColor("reset") + "\n");
        }
        blacklistFile.close();
    } else {
        if (showSerial) Serial.print(DevUtils::termColor("red") + "Failed to open blacklist.json for writing" + DevUtils::termColor("reset") + "\n");
    }

    // Write version marker
    File versionFile = LittleFS.open("/spacecraft_data/version.txt", "w");
    if (versionFile) {
        versionFile.print(firmwareVersion);
        versionFile.close();
    } else {
        if (showSerial) Serial.print(DevUtils::termColor("red") + "Failed to write version.txt" + DevUtils::termColor("reset") + "\n");
    }

    if (showSerial) Serial.print(DevUtils::termColor("green") + "Spacecraft data written to flash" + DevUtils::termColor("reset") + "\n");
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
    bool isBlacklisted = spacecraftBlacklistJson[key] != nullptr;

    if (showSerial && isBlacklisted) {
        char buffer[128];
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

    return isBlacklisted;
}

/* Check Approved */
bool SpacecraftData::checkApproved(const char* key) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    bool isApproved = spacecraftApprovedJson[key] != nullptr;

    if (showSerial && !isApproved) {
        char buffer[128];
        snprintf(
            buffer,
            sizeof(buffer),
            "%s%s is not approved, skipping...%s\n",
            DevUtils::termColor("purple"),
            key,
            DevUtils::termColor("reset")
        );
        Serial.print(buffer);
    }

    return isApproved;
}

void SpacecraftData::populateApprovedInMemory() {
    spacecraftApprovedJson.clear();
    spacecraftApprovedJson["ACE"] = true;
    spacecraftApprovedJson["BEPI"] = true;
    spacecraftApprovedJson["BIOS"] = true;
    spacecraftApprovedJson["CAPS"] = true;
    spacecraftApprovedJson["CGO"] = true;
    spacecraftApprovedJson["CHDR"] = true;
    spacecraftApprovedJson["DSCO"] = true;
    spacecraftApprovedJson["EMM"] = true;
    spacecraftApprovedJson["ESCB"] = true;
    spacecraftApprovedJson["ESCG"] = true;
    spacecraftApprovedJson["EURC"] = true;
    spacecraftApprovedJson["HYB2"] = true;
    spacecraftApprovedJson["IMAP"] = true;
    spacecraftApprovedJson["JNO"] = true;
    spacecraftApprovedJson["JWST"] = true;
    spacecraftApprovedJson["KPLO"] = true;
    spacecraftApprovedJson["LRO"] = true;
    spacecraftApprovedJson["LUCY"] = true;
    spacecraftApprovedJson["M01O"] = true;
    spacecraftApprovedJson["M20"] = true;
    spacecraftApprovedJson["MEX"] = true;
    spacecraftApprovedJson["MMS1"] = true;
    spacecraftApprovedJson["MMS2"] = true;
    spacecraftApprovedJson["MMS3"] = true;
    spacecraftApprovedJson["MMS4"] = true;
    spacecraftApprovedJson["MRO"] = true;
    spacecraftApprovedJson["MSL"] = true;
    spacecraftApprovedJson["MVN"] = true;
    spacecraftApprovedJson["NHPC"] = true;
    spacecraftApprovedJson["ORX"] = true;
    spacecraftApprovedJson["PSYC"] = true;
    spacecraftApprovedJson["SOHO"] = true;
    spacecraftApprovedJson["SOLAR"] = true;
    spacecraftApprovedJson["SPP"] = true;
    spacecraftApprovedJson["STA"] = true;
    spacecraftApprovedJson["SWFO"] = true;
    spacecraftApprovedJson["TESS"] = true;
    spacecraftApprovedJson["TGO"] = true;
    spacecraftApprovedJson["THB"] = true;
    spacecraftApprovedJson["THC"] = true;
    spacecraftApprovedJson["VGR1"] = true;
    spacecraftApprovedJson["VGR2"] = true;
    spacecraftApprovedJson["WIND"] = true;
    spacecraftApprovedJson["XMM"] = true;
}
