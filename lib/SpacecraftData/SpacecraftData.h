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
        const char* callsignToName(char* key);
};

#endif