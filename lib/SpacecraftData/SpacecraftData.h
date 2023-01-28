/*
    Spacecraft Data
    Created by Grant Keller, November 2022
*/

#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SpacecraftData_h
#define SpacecraftData_h

class SpacecraftData {
    public:
        StaticJsonDocument<1536> spacecraftNamesJson;
        void loadJson();
        const char* callsignToName(const char* key);
        StaticJsonDocument<100> spacecraftBlacklistJson;
        void blacklist();
        bool checkBlacklist(const char* callsign);
};

#endif