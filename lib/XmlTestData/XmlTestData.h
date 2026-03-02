#ifndef XMLTESTDATA_H
#define XMLTESTDATA_H

#include <Arduino.h>

class XmlTestData {
public:
    static void init();
    static bool loadFile(const char* filename, char* buffer, size_t bufferSize);
    static const char* getProgmemData(const char* filename); // Returns PROGMEM pointer for built-in data, or nullptr
    static const char* getFallbackData(); // Returns PROGMEM pointer — valid for device lifetime, do not free

private:
    static void seedFileIfMissing(const char* path, const char* data);
};

#endif
