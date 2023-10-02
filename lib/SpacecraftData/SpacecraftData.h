/*
    Spacecraft Data
    Created by Grant Keller, November 2022
*/

#ifndef ARDUINO_H
    #include <ArduinoJson.h>
#endif

#ifndef FILEUTILS_H
    #include <FileUtils.h>
#endif

#ifndef DEVUTILS_H
    #include <DevUtils.h>
#endif

#ifndef SPACECRAFTDATA_H
#define SPACECRAFTDATA_H

class SpacecraftData
{
    public:
    SpacecraftData();
    void loadJson();
    const char* callsignToName(const char* key);
    void loadSpacecraftBlacklist();
    bool checkBlacklist(const char* callsign);

    private:
    DynamicJsonDocument spacecraftNamesJson;
    DynamicJsonDocument spacecraftBlacklistJson;
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