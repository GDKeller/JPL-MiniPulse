#include <SD.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SpacecraftData.h>

const char *filename = "/spacecraftNames.json";

void SpacecraftData::loadJson() {

    char file[] = "{\"ACE\":\"Advanced Composition Explorer\",\"PLC\":\"Akatsuki\",\"ARGO\":\"ArgoMoon\",\"BIOS\":\"BioSentinel\",\"CHDR\":\"Chandra X-ray Observatory (CXO)\",\"CH2\":\"Chandrayaan-2\",\"CUE3\":\"CU Earth Escape Explorer\",\"CuSP\":\"CubeSat for the Observation of Solar Particles\",\"DART\":\"Double Asteroid Redirection Test(DART)\",\"unknown10\":\"Dragonfly\",\"DSCO\":\"Deep Space Climate Observatory (DSCOVR) Mission\",\"EMM\":\"Emirates Mars Mission\",\"EQUL\":\"EQUilibriUm Lunar-Earth point 6U Spacecraft (EQUULEUS)\",\"EURC\":\"Europa Clipper Mission\",\"RSP\":\"ExoMars Rover and Surface Platform\",\"GAIA\":\"Gaia\",\"GTL\":\"Geotail\",\"HYB2\":\"Hayabusa-2\",\"EM1\":\"Human Space Flight (HSF): Artemis 1\",\"EM2\":\"Human Space Flight (HSF): Artemis 2\",\"EM3\":\"Human Space Flight (HSF): Artemis 3\",\"NSYT\":\"Interior Exploration using Seismic Investigations, Geodesy and Heat Transport (InSight)\",\"JWST\":\"James Webb Space Telescope\",\"JNO\":\"Juno\",\"KPLO\":\"Korea Pathfinder Lunar Orbiter\",\"LICI\":\"LICIA Cube\",\"LND1\":\"Lunar Node 1\",\"LUCY\":\"Lucy\",\"LFL\":\"Lunar Flashlight\",\"HMAP\":\"Lunar Hydrogen Mapper\",\"LRO\":\"Lunar Reconnaissance Orbiter (LRO)\",\"MMS1,2,3,4\":\"Magnetospheric MultiScale (MMS) Formation Flyers\",\"M01O\":\"Mars 01 Odyssey\",\"M20\":\"Mars 2020\",\"MVN\":\"Mars Atmosphere and Volatile EvolutioN (MAVEN)\",\"MEX\":\"Mars Express (MEX)\",\"MOM\":\"Mars Orbiter Mission\",\"MRO\":\"Mars Reconnaissance Orbiter\",\"MSL\":\"Mars Science Laboratory (Curiosity)\",\"MLI\":\"Morehead Lunar Ice Cube\",\"NEAS\":\"Near Earth Asteroid Scout\",\"NHPC\":\"New Horizons Pluto-Charon\",\"ORX\":\"Origins-Spectral Interpretation-Resource Identification-Security-Regolith Explorer(OSIRIS-REx)\",\"OMOT\":\"Outstanding Moonexploration Technologies NanoSemi - Hard Impactor(OMOTENASHI)\",\"PSYC\":\"Psyche\",\"SOHO\":\"Solar and Heliospheric Observatory Mission (SOHO)\",\"SPP\":\"Parker Solar Probe\",\"STA\":\"Solar-TErrestrial RElations Observatory (STEREOAhead,Behind)\",\"TESS\":\"Transiting Exoplanet Survey Satellite\",\"TGO\":\"ExoMars 2016 Trace Gas Orbiter (TGO)\",\"THB,C\":\"THEMISB,C Spacecraft/ARTEMISMission(TimeHistoryofEventsandMacroscaleInteractionsDuringSubstorms)\",\"TM\":\"TeamMiles\",\"VGR1\":\"Voyager 1 Interstellar Mission\",\"VGR2\":\"Voyager2 Interstellar Mission\",\"WIND\":\"Wind\",\"XMM\":\"X-ray Multi-Mirror Mission (XMM-Newton)\",\"ATOT\":\"Advanced Tracking and Observational Techniques\",\"EGS\":\"EVN and Global Sevices\",\"GBRA\":\"Ground Based Radio Astronomy\",\"GSSR\":\"Goldstone Solar System Radar X-Band\",\"GVRT\":\"Goldstone Apple Valley Radio Telescope S-Band (GAVRT)\",\"RFC(VLBI)\":\"Reference Frame Calibration\",\"SGP\":\"Space Geodesy Program\",\"TDR6\":\"Tracking and Data Relay Satellites (TDRS)\",\"TDR7\":\"Tracking and Data Relay Satellites (TDRS)\",\"TDR8\":\"Tracking and Data Relay Satellites (TDRS)\",\"TDR9\":\"Tracking and Data Relay Satellites (TDRS)\",\"TD10\":\"Tracking and Data Relay Satellites (TDRS)\",\"TD11\":\"Tracking and Data Relay Satellites (TDRS)\",\"TD12\":\"Tracking and Data Relay Satellites (TDRS)\",\"TD13\":\"Tracking and Data Relay Satellites (TDRS)\"}";

    DeserializationError error = deserializeJson(spacecraftNamesJson, file);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    Serial.println("JSON loaded");
}

const char* SpacecraftData::callsignToName(char* key) {
    const char* result = spacecraftNamesJson[key];
    return result;
}