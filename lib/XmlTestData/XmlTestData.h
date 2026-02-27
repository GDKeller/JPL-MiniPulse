#ifndef XMLTESTDATA_H
#define XMLTESTDATA_H

#include <Arduino.h>

class XmlTestData {
public:
    static void init(const char* firmwareVersion);
    static bool loadFile(const char* filename, char* buffer, size_t bufferSize);
    static const char* getFallbackData(); // Returns PROGMEM pointer — valid for device lifetime, do not free

private:
    static void seedFileIfMissing(const char* path, const char* data);
    static bool shouldReseed(const char* firmwareVersion);
    static void writeVersionMarker(const char* firmwareVersion);
};

#endif
