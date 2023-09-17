/*
    Filesystem Utilities
    Created by Grant Keller, September 2023
*/

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef VECTOR_H
#include <vector>
#endif

#ifndef UTILITY_H
#include <utility>
#endif

#ifndef LITTLEFS_H
#include <LittleFS.h>
#endif

#ifndef JSON_H
#include <ArduinoJson.h>
#endif

// #ifndef DEVUTILS_H
// #include <DevUtils.h>
// #endif

#ifndef FILEUTILS_H
#define FILEUTILS_H

class DevUtils;

class FileUtils {
    private:
    struct DebugUtils {
        bool testCores;
        bool showSerial;
        bool diagMeasure;
        bool disableWiFi;
        bool testLEDs;
    };

    struct WifiNetwork {
        char apSSID[32];
        char apPass[32];
        char serverName[128]; // DSN XML server URL
        bool forceDummyData;
        int noTargetLimit;
        int retryDataFetchLimit;
    };

    struct PinsHardware {
        int outerPin;
        int middlePin;
        int innerPin;
        int bottomPin;
        int wifiRst;
        int outputEnable;
        int potentiometer;
    };

    struct DisplayLED {
        int brightness;
        int fps;
        int outerPixelsTotal;
        int middlePixelsTotal;
        int innerPixelsTotal;
        int bottomPixelsTotal;
        int outerChunks;
        int middleChunks;
        int innerChunks;
        int bottomChunks;
        int craftDelay;
        int displayMinDuration;
    };

    struct TextTypography {
        int characterWidth;
        int textMeteorGap;
        int meteorOffset;
    };

    struct TimersDelays {
        int timerDelay;
    };

    struct Miscellaneous {
        int colorTheme;
    };

    struct Config {
        DebugUtils debugUtils;
        WifiNetwork wifiNetwork;
        PinsHardware pinsHardware;
        DisplayLED displayLED;
        TextTypography textTypography;
        TimersDelays timersDelays;
        Miscellaneous miscellaneous;
    };


    public:
    static Config config;

    static bool checkConfigFileExists();
    static void createConfigFile();
    static void initConfigFile();
    static void printAllConfigFileKeys();
    static void updateDebugUtilsField(const char* key, const JsonVariant& value);
    static void updateWifiNetworkField(const char* key, const JsonVariant& value);
    static void updatePinsHardwareField(const char* key, const JsonVariant& value);
    static void updateDisplayLEDField(const char* key, const JsonVariant& value);
    static void updateTextTypographyField(const char* key, const JsonVariant& value);
    static void updateTimersDelaysField(const char* key, const JsonVariant& value);
    static void updateMiscellaneousField(const char* key, const JsonVariant& value);
    static void writeConfigFileBool(const char* key, bool value);
    static void writeConfigFileInt(const char* key, int value);
    static void writeConfigFileString(const char* key, const char* value);
};

#endif