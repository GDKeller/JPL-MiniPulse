/*
    Spacecraft Data
    Created by Grant Keller, November 2022
*/

#include <ArduinoJson.h>

#ifndef SPACECRAFTDATA_H
#define SPACECRAFTDATA_H

class SpacecraftData
{
    public:
    StaticJsonDocument<4096> spacecraftNamesJson;
    void loadJson();
    const char* callsignToName(const char* key);
    StaticJsonDocument<1024> spacecraftBlacklistJson;
    void loadSpacecraftBlacklist();
    bool checkBlacklist(const char* callsign);
};

struct CraftQueueItem
{
    char callsignArray[10] = { 0 };
    const char* callsign = &callsignArray[0];
    char nameArray[100] = { 0 };
    const char* name = &nameArray[0];
    uint nameLength = 0;
    uint upSignal = 0;
    uint downSignal = 0;
};

#endif