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

struct CraftQueueItem {
    char callsignArray[10] = {0};
    const char* callsign = &callsignArray[0];
    char nameArray[100] = {0};
    const char* name = &nameArray[0];
    uint nameLength = 0;
    uint upSignal = 0;
    uint downSignal = 0;
};



#endif