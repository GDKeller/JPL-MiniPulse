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

#ifndef FAST_LED_H
#include <FastLED.h>
#endif

#ifndef SPACECRAFTDATA_H
#define SPACECRAFTDATA_H

class SpacecraftData
{
    public:
    SpacecraftData();
    void loadJson();
    void loadSpacecraftNamesProgmem();
    void loadSpacecraftBlacklistProgmem();
    void loadSpacecraftPlaceholderRatesProgmem();
    const char* callsignToName(const char* key);
    bool checkBlacklist(const char* callsign);
    const char* getPlaceholderRate(const char* key);

    private:
    DynamicJsonDocument spacecraftNamesJson;
    DynamicJsonDocument spacecraftBlacklistJson;
    DynamicJsonDocument spacecraftPlaceholderRatesJson;
};

struct CraftQueueItem
{
    char callsignArray[40] = { 0 };
    const char* callsign = &callsignArray[0];
    char nameArray[100] = { 0 };
    const char* name = &nameArray[0];
    uint nameLength = 0;
    uint upSignal = 0;
    uint downSignal = 0;
};

#endif


// /*
//     Spacecraft Data
//     Created by Grant Keller, November 2022
// */

// #ifndef ARDUINO_H
//     #include <ArduinoJson.h>
// #endif

// #ifndef FILEUTILS_H
//     #include <FileUtils.h>
// #endif

// #ifndef DEVUTILS_H
//     #include <DevUtils.h>
// #endif

// #ifndef FAST_LED_H
//     #include <FastLED.h>
// #endif

// #ifndef SPACECRAFTDATA_H
// #define SPACECRAFTDATA_H

// class SpacecraftData
// {
//     public:
//     SpacecraftData();
//     void loadJson();
//     // void loadSpacecraftNames();
//     void loadSpacecraftNamesRaw();
//     // void loadSpacecraftPlaceholderRates();
//     void loadSpacecraftPlaceholderRatesRaw();
//     // void loadSpacecraftBlacklist();
//     void loadSpacecraftBlacklistRaw();
//     const char* callsignToName(const char* key);
//     const char* getPlaceholderRate(const char* key);
//     bool checkBlacklist(const char* callsign);

//     private:
//     DynamicJsonDocument spacecraftNamesJson;
//     DynamicJsonDocument spacecraftPlaceholderRatesJson;
//     DynamicJsonDocument spacecraftBlacklistJson;
// };

// struct CraftQueueItem
// {
//     char callsignArray[40] = { 0 };
//     const char* callsign = &callsignArray[0];
//     char nameArray[100] = { 0 };
//     const char* name = &nameArray[0];
//     uint nameLength = 0;
//     uint upSignal = 0;
//     uint downSignal = 0;
// };

// #endif