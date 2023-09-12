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

#ifndef SPIFFS_H
#include <SPIFFS.h>
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

    static void merge(JsonObject& dest, const JsonObject& src);

    public:
    struct Config {
        DebugUtils debugUtils;
        WifiNetwork wifiNetwork;
        PinsHardware pinsHardware;
        DisplayLED displayLED;
        TextTypography textTypography;
        TimersDelays timersDelays;
        Miscellaneous miscellaneous;
    };
    static Config config;


    static void loadConfig();
    static void loadDefaultConfig(JsonDocument& doc);
    static void loadUserConfig(JsonDocument& doc);
    static void saveUserConfig(const std::vector<std::pair<String, JsonVariant>>& updates);

};

#endif