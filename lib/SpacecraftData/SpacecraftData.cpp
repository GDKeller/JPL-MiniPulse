#include <SpacecraftData.h>

const char* filename = "/spacecraftNames.json";


SpacecraftData::SpacecraftData()
  : spacecraftNamesJson(5120), spacecraftBlacklistJson(1024) {}


/**
 * Spacecraft Callsign to Name
 */
void SpacecraftData::loadJson()
{
  /* Memory warning!
   * The values cannot be longer than 100 characters, or there will be a fatal error.
   * The memory for this variable for animation function is statically allocated
   */
  static const char file[] = PROGMEM R"=---=(
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

  char buffer[5120];
  strcpy_P(buffer, file);

  DeserializationError error = deserializeJson(spacecraftNamesJson, buffer);

  if (error) {
    Serial.print(DevUtils::termColor("red") + "Spacecraft callsigns list loading failed: " + error.c_str() + DevUtils::termColor("reset") + "\n");
    return;
  }
  Serial.println("Spacecraft callsigns loaded");
}

const char* SpacecraftData::callsignToName(const char* key) {
  // Serial.print("\n\n>>>>>>>>>>>>>>>>>-------------------------\nLooking for spacecraft name for callsign: " + String(key) + "\n");
  if (spacecraftNamesJson.containsKey(key)) {
    const char* spacecraftName = spacecraftNamesJson[key];
    // Serial.println(String("Extracted Value: ") + spacecraftName);
    // Serial.println("Key found: " + String(key) + " = " + String(spacecraftName));
    // Serial.print(">>>>>>>>>>>>>>>>>-------------------------\n\n");
    return spacecraftName;
  }

  // Serial.println("Key not found: " + String(key));
  // Serial.print(">>>>>>>>>>>>>>>>>-------------------------\n\n");
  return key;
}



/**
 * Spacecraft Blacklist
 *
 * Returns true if spacecraft is on blacklist
 */
void SpacecraftData::loadSpacecraftBlacklist() {
  // "RFC(VLBI)": "Reference Frame Calibration"

  static const char file[] = PROGMEM R"=---=(
      {
        "TEST": true,
        "DSN": true,
        "RFC(VLBI)": "Reference Frame Calibration"
      }
    )=---=";

  char buffer[1024];
  strcpy_P(buffer, file);

  DeserializationError error = deserializeJson(spacecraftBlacklistJson, file);

  if (error) {
    if (FileUtils::config.debugUtils.showSerial)
      Serial.print(DevUtils::termColor("red") + "Spacecraft blacklist loading failed: " + error.c_str() + DevUtils::termColor("reset") + "\n");

    return;
  }
  Serial.println("Spacecraft blacklist loaded");
}



/* Check Blacklist */
bool SpacecraftData::checkBlacklist(const char* key) {
  bool isBlacklisted = spacecraftBlacklistJson[key] != nullptr ? true : false; // If the key is not null, it is blacklisted

  if (FileUtils::config.debugUtils.showSerial)
    Serial.println(DevUtils::termColor("bg_bright_black") + ">>> Checking blacklist for " + String(key) + ": " + (isBlacklisted ? "true" : "false") + DevUtils::termColor("reset") + "\n");


  return spacecraftBlacklistJson[key] != nullptr ? true : false;
}