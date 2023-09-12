/*
    Development Utilities
    Created by Grant Keller, September 2023
*/

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef MAP
#include <map>
#endif

#ifndef EXCEPTION
#include <exception>
#endif

#ifndef WIFIMANAGER_H
#include <WiFiManager.h>
#endif

#ifndef DEVUTILS_H
#define DEVUTILS_H


class DevUtils {
    public:
    static String termColor(const char* color);
    static String getFreeHeap();
    void printFreeHeap();
    void handleException();
    static String repeatString(String input, int count);

    class SerialBanners {
        public:
        static void printBootSplashBanner();
        static void printWiFiConfigBanner(const char* apPassword, WiFiManager& wifiManager);
    };
};

#endif