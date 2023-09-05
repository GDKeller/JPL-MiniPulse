#pragma region -- LIBRARIES

#include <Arduino.h>		// Arduino core
#include <FS.h>				// File system
#include <SPIFFS.h>			// SPIFFS file system
#include <HTTPClient.h>		// HTTP client
#include <FastLED.h>		// FastLED lib
#include <tinyxml2.h>		// XML parser
#include <iostream>			// C++ I/O
#include <cstring>			// C++ string
#include <ArduinoJson.h>	// JSON parser
#include <algorithm>		// C++ algorithms
#include <MathHelpers.h>	// Custom math helpers lib
#include <TextCharacters.h> // Custom LED text lib
#include <AnimationUtils.h> // Custom animation utilities lib
#include <Animate.h>		// Custom animate lib
#include <SpacecraftData.h> // Custom spacecraft data lib
#include <WiFiManager.h>	// WiFi manager lib

#pragma endregion -- END LIBRARIES

#pragma region -- CONFIG STRUCT
struct DebugUtils
{
	bool testCores;
	bool showSerial;
	bool diagMeasure;
	bool disableWiFi;
	bool testLEDs;
};

struct WifiNetwork
{
	const char* apSSID;
	const char* apPass;
	const char* serverName; // DSN XML server URL
	bool startAP;
	int wmTimeout;
	bool forceDummyData;
	int noTargetLimit;
	int retryDataFetchLimit;
};

struct PinsHardware
{
	int outerPin;
	int middlePin;
	int innerPin;
	int bottomPin;
	int wifiRst;
	int outputEnable;
	int potentiometer;
};

struct DisplayLED
{
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

struct TextTypography
{
	int characterWidth;
	int textMeteorGap;
	int meteorOffset;
};

struct TimersDelays
{
	int timerDelay;
};

struct Miscellaneous
{
	int colorTheme;
};

struct Config
{
	DebugUtils debugUtils;
	WifiNetwork wifiNetwork;
	PinsHardware pinsHardware;
	DisplayLED displayLED;
	TextTypography textTypography;
	TimersDelays timersDelays;
	Miscellaneous miscellaneous;
};

Config config;

#pragma endregion -- CONFIG STRUCT

#pragma region -- NAMESPACES
using namespace tinyxml2; // XML parser
using namespace std;	  // C++ I/O
#pragma endregion thing

#pragma region-- ESP32 PIN ASSIGNMENTS
#define FASTLED_ALL_PINS_HARDWARE_SPI
#define OUTER_PIN 17		// Outer ring pin
#define MIDDLE_PIN 18		// Middle ring pin
#define INNER_PIN 19		// Inner ring pin
#define BOTTOM_PIN 16		// Bottom ring pin
#define WIFI_RST 21			// WiFi reset pin
#define OUTPUT_ENABLE 22	// Output enable pin
#define POTENTIOMETER 32	// Brightness potentiometer pin
#pragma endregion


#pragma region -- CLASS INSTANTIATIONS
// Animations
AnimationUtils au(POTENTIOMETER); // Instantiate animation utils
AnimationUtils::Colors mpColors;  // Instantiate colors
Animate animate;				  // Instantiate animate
ColorTheme currentColors;		  // Instantiate LED color theme

// Data
SpacecraftData data; // Instantiate spacecraft data

// Tasks
TaskHandle_t xHandleData;		 // Task for fetching and parsing data
TaskStatus_t xHandleDataDetails; // Task details for HandleData
QueueHandle_t queue;			 // Queue to pass data between tasks

// Networking
WiFiManager wm;	 // Used for connecting to WiFi
HTTPClient http; // Used for fetching data
#pragma endregion


// Placeholder for XML data
// const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
// const char* dummyXmlData2 = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
// const char* dummyXmlData3 = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";

// Test dummy data that cycles through the rate classes
const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
    <dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556">
        <downSignal dataRate="1" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <upSignal dataRate="1" frequency="2090" power="4.804" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="Rate1" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
    <dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556">
        <downSignal dataRate="1025" frequency="8439000000" power="-138.1801" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <upSignal dataRate="1025" frequency="7183" power="0.0000" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <target downlegRange="8.207e+07" id="74" name="Rate2" rtlt="547.5" uplegRange="8.207e+07" />
    </dish>
    <station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" />
    <dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556">
        <downSignal dataRate="5121" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <upSignal dataRate="5121" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <target downlegRange="1.417e+05" id="151" name="Rate3" rtlt="0.9455" uplegRange="1.417e+05" />
    </dish>
    <dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556">
        <downSignal dataRate="102401" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <upSignal dataRate="102401" frequency="2098" power="0.2630" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <target downlegRange="1.389e+06" id="92" name="Rate4" rtlt="9.266" uplegRange="1.389e+06" />
    </dish>
    <station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" />
    <dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556">
        <downSignal dataRate="1048577" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <upSignal dataRate="1048577" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <target downlegRange="4.099e+06" id="210" name="Rate5" rtlt="27.34" uplegRange="4.099e+06" />
    </dish>
    <dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556">
        <upSignal dataRate="1024000001" frequency="2066" power="1.758" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <downSignal dataRate="1024000001" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <target downlegRange="1.331e+06" id="21" name="Rate6" rtlt="8.882" uplegRange="1.331e+06" />
    </dish>
    <timestamp>1670419133000</timestamp>
</dsn>)==--==";

/* STATE TRACKING
* Sets up variables to track state of the program
*/

#pragma region -- DATA FETCHING STATE VARS

bool usingDummyData = false; // If true, use dummy data instead of actual data
bool forceDummyData = true;
uint8_t noTargetFoundCounter = 0;  // Keeps track of how many times target is not found
uint8_t noTargetLimit = 3;		   // After target is not found this many times, switch to dummy XML data
uint8_t retryDataFetchCounter = 0; // Keeps track of how many times data fetch failed
uint8_t retryDataFetchLimit = 10;  // After dummy data is used this many times, try to get actual data again
bool dataStarted = false;
char fetchUrl[50];	// Fixed memory for DSN XML fetch URL - random number is appended when used to prevent caching

#pragma endregion -- END DATA FETCHING STATE VARS

#pragma region -- DATA COUNTERS
// Counters to track during XML parsing
unsigned int stationCount = 0;
unsigned int dishCount = 0;
unsigned int targetCount = 0;
unsigned int signalCount = 0;
int parseCounter = 0;
#pragma endregion DATA COUNTERS

#pragma region -- DATA CURRENT CRAFT

// Holds current craft to be animated
static CraftQueueItem currentCraftBuffer;
const int MAX_ITEMS = 5; // Max number of queue items
CraftQueueItem itemPool[MAX_ITEMS];
static CraftQueueItem* freeList[MAX_ITEMS];
static int freeListTop = MAX_ITEMS; // Points to the top of the free list
SemaphoreHandle_t freeListMutex;	// Mutex to protect the free list

#pragma endregion -- END DATA CURRENT CRAFT

#pragma region -- WIFIMANAGER PORTAL
/** LED Color Themes
 * ID - Name		(Letter/Meteor/Meteor tail)
 * 0  - White		(White/white/white tint)
 * 1  - Cyber		(Teal/teal/blue tint)
 * 2  - Valentine	(pink/pink/purple tint)
 * 3  - Moonlight	(white/white/blue tint)
 */
const uint8_t colorTheme = 0;

WiFiManagerParameter field_color_theme;		   // global param ( for non blocking w params )
WiFiManagerParameter field_meteor_tail_decay;  // global param ( for non blocking w params )
WiFiManagerParameter field_meteor_tail_random; // global param ( for non blocking w params )
WiFiManagerParameter field_global_fps;
unsigned long wmStartTime = millis();
bool portalRunning = false;
bool startAP = false; // If true, start AP and webserver. If false, start only webserver
struct wmParams
{
	String customfieldid;
};

#pragma endregion -- END WIFIMANAGER PORTAL

#pragma region -- TIMERS
/* Time is measured in milliseconds, so long is used */
uint8_t fpsRate = 60;
unsigned long fpsTimer = 0;
unsigned long perfTimer = 0;
unsigned long perfDiff = 0;
unsigned long lastTime = 0; // Init reference variable for timer
const uint16_t timerDelay = 10000; // Timer for how often to fetch data
unsigned long animationTimer = 0;
unsigned long craftDelayTimer = 0;
const uint16_t craftDelay = 8000; // Wait this long after finishing for new craft to be dipslayed
const uint16_t displayMinDuration = 1000; // Minimum time to display a craft before switching to next craft
unsigned long displayDurationTimer = 1000; // Timer to keep track of how long craft has been displayed, set at minimum for no startup delay
const uint16_t textMeteorGap = 4000;
const uint8_t meteorOffset = 32;
const uint8_t offsetHalf = meteorOffset * 0.5;

#pragma endregion -- END TIMERS

#pragma region -- LED HARDWARE CONFIG
// Totaly number of pixels (diodes) in each strip
const int outerPixelsTotal = 800;
const int middlePixelsTotal = 800;
const int innerPixelsTotal = 960;
const int bottomPixelsTotal = 5;

// Total number of LED sticks per strip
const int outerChunks = 10;
const int middleChunks = 10;
const int innerChunks = 12;
const int bottomChunks = 1;

// Calculate lengths of each LED stick
const int innerPixelsChunkLength = innerPixelsTotal / innerChunks;
const int outerPixelsChunkLength = outerPixelsTotal / outerChunks;

// Set up LED strips
CRGB inner_leds[innerPixelsTotal];
CRGB middle_leds[middlePixelsTotal];
CRGB outer_leds[outerPixelsTotal];
CRGB bottom_leds[bottomPixelsTotal];
CRGB* allStrips[4] = {
	inner_leds,	 // ID: Green
	middle_leds, // ID: Red
	outer_leds,	 // ID: Blue
	bottom_leds, // ID: Purple
};
CHSV bottomPixelMap[5] = { CHSV(0, 0, 0) };


#pragma endregion -- END LED HARDWARE CONFIG

#pragma region -- TEXT SETTINGS

TextCharacter textCharacter;
static int characterWidth;
static  int characterHeight = 7;
static int characterKerning = 3;
static int letterSpacing = 7;
static int letterTotalPixels = 28;

#pragma endregion -- END TEXT SETTINGS


/* FUNCTIONS
* Doing things
*/

#pragma region -- ANIMATION UTILITIES

uint8_t fpsInMs = 1000 / fpsRate;
bool meteorTailDecay = false;
bool meteorTailRandom = false;

// State counters to enable fractional animation speeds
// These all start at 1 and get incremented during animation update
uint8_t counterHalfSpeed = 1;
uint8_t counterThirdSpeed = 1;
uint8_t counterQuarterSpeed = 1;

// Keeps track of current animation state
static bool nameScrollDone = true;
static bool animationTypeSetDown = false;
static bool animationTypeSetUp = false;
static uint8_t animationTypeDown = 0;
static uint8_t animationTypeUp = 0;
bool animateFirstCycleDown = true;
bool animateFirstCycleUp = true;

#pragma endregion -- END ANIMATION UTILITIES

#pragma region -- DEV UTILITIES

const String termColor(const char* color)
{
	if (color == "black")
		return "\e[0;30m";
	if (color == "red")
		return "\e[0;31m";
	if (color == "green")
		return "\e[32m";
	if (color == "yellow")
		return "\e[33m";
	if (color == "blue")
		return "\e[34m";
	if (color == "purple")
		return "\e[35m";
	if (color == "cyan")
		return "\e[36m";
	if (color == "white")
		return "\e[37m";
	if (color == "reset")
		return "\e[0m";

	return "\e[0m";
}

void printFreeHeap()
{
	String printString;

	// printString += "\n";
	// printString += termColor("blue");
	printString += "MEM_Free_Heap:";
	printString += ESP.getFreeHeap() * 0.001; // Value being divided for visualization on plotter
	// printString += ESP.getFreeHeap();	// This is the actual value
	printString += "\t";
	// printString += termColor("reset");
	// printString += "\n";

	Serial.print(printString);
}

void handleException()
{
	Serial.print(termColor("red"));
	Serial.println("EXCEPTION CAUGHT:");
	try {
		throw;
	}
	catch (const exception& e) {
		if (&e == nullptr) {
			Serial.println("Exception is null");
			return;
		} else {
			Serial.println(e.what());
		}
	}
	catch (const int i) {
		Serial.println(i);
	}
	catch (const long l) {
		Serial.println(l);
	}
	catch (const char* p) {
		if (p == nullptr) {
			Serial.println("Exception is null");
			return;
		} else {
			Serial.println(p);
		}
	}
	catch (...) {
		Serial.println("Exception unknown");
	}
	Serial.println(termColor("reset"));
}

void printMeteorArray()
{

	String printString = "\n---------------[ METEOR ARRAY ]---------------\n";

	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] == nullptr) {
			printString += "[";
			printString += termColor("red");
			printString += i;
			printString += " = nul";
			printString += "]";
		} else {
			printString += "[";
			printString += termColor("green");
			printString += i;
			printString += " = ";
			printString += animate.ActiveMeteors[i]->firstPixel;
			printString += "]";
		}
		printString += termColor("reset");

		if (i != 0 && i % 10 == 0) {
			printString += "\n";
		}
	}

	printString += "\n------------------------------\n";

	Serial.println(printString);
}

void printCraftInfo(uint listPosition, const char* callsign, const char* name, uint nameLength, uint downSignal, uint upSignal)
{
	String formattedString;
	formattedString = "ITEM #" + String(listPosition) + ": (" + String(callsign) + ") " + String(name) + " [" + String(nameLength) + "]";
	formattedString += " [↓ Sig Dn: " + String(downSignal) + "] [↑ Sig Up: " + String(upSignal) + "]";
	Serial.print(String(formattedString + "\n"));
}

String returnCraftInfo(uint listPosition, const char* callsign, const char* name, uint nameLength, uint downSignal, uint upSignal)
{
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "ITEM #%u: (%s) %s [%u] [↓ Sig Dn: %u] [↑ Sig Up: %u]\n",
		listPosition, callsign, name, nameLength, downSignal, upSignal);
	return String(buffer);
}



void printCurrentQueue(QueueHandle_t queue) {
	if (uxQueueMessagesWaiting(queue) > 0) {
		QueueHandle_t tempQueue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create a temporary queue to hold the items
		if (tempQueue == NULL) {
			Serial.println("Error: Could not create temporary queue.");
			return;
		}
		CraftQueueItem theTempBuffer;
		CraftQueueItem* tempBuffer = &theTempBuffer;

		char buffer[1024];
		// String queueString;
		snprintf(buffer, sizeof(buffer), "\n%s========= QUEUE %d/%d ==========%s\n", termColor("purple"), uxQueueMessagesWaiting(queue), MAX_ITEMS, termColor("reset"));

		int queueCount = uxQueueMessagesWaiting(queue);
		for (int i = 0; i < queueCount; i++) {
			if (xQueueReceive(queue, &tempBuffer, (TickType_t)10)) {
				// printCraftInfo(i, tempBuffer->callsign, tempBuffer->name, tempBuffer->nameLength, tempBuffer->downSignal, tempBuffer->upSignal);
				String craftInfo = returnCraftInfo(i, tempBuffer->callsign, tempBuffer->name, tempBuffer->nameLength, tempBuffer->downSignal, tempBuffer->upSignal);
				strncat(buffer, craftInfo.c_str(), sizeof(buffer) - strlen(buffer) - 1);

				if (!xQueueSend(tempQueue, &tempBuffer, (TickType_t)10)) {
					Serial.println("Error: Could not send data to the temporary queue.");
					// Handle error as necessary, maybe break out of the loop
				}
			} else {
				Serial.println("Error: Could not receive data from the main queue.");
				// Handle error as necessary, maybe break out of the loop
			}
		}

		for (int i = 0; i < queueCount; i++) {
			if (xQueueReceive(tempQueue, &tempBuffer, (TickType_t)10)) {
				if (!xQueueSend(queue, &tempBuffer, (TickType_t)10)) {
					Serial.println("Error: Could not send data back to the main queue.");
					// Handle error as necessary, maybe break out of the loop
				}
			} else {
				Serial.println("Error: Could not receive data from the temporary queue.");
				// Handle error as necessary, maybe break out of the loop
			}
		}

		vQueueDelete(tempQueue); // Delete the temporary queue

		snprintf(buffer, sizeof(buffer), "%s%s===============================%s\n\n", buffer, termColor("purple"), termColor("reset"));
		Serial.print(buffer);
	}
}

void printCurrentCraftBuffer() {
	Serial.print(String(termColor("blue")) + "============= CURRENT DATA BUFFER =============" + String(termColor("reset")) + String("\n"));
	printCraftInfo(0, currentCraftBuffer.callsign, currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	Serial.print(String(termColor("blue")) + "===============================================" + String(termColor("reset")) + "\n\n");
}

void printSemaphoreList() {
	// This function must be used inside a mutex lock
	if (config.debugUtils.showSerial == true) {
		Serial.print("\n=== freeListMutex ===\n");
		for (int i = 0; i < MAX_ITEMS; i++) {
			if (freeList[i] == nullptr) continue; // Skip null items

			String marker = (freeListTop - 1 == i) ? ">" : " "; // Marker for top of freeList
			Serial.print(marker + "[" + String(i) + "] " + String(freeList[i]->callsignArray) + "\n");
		}
	}
}

void freeSempaphoreItem(CraftQueueItem* infoBuffer) {
	xSemaphoreTake(freeListMutex, portMAX_DELAY);
	if (config.debugUtils.diagMeasure == true)
		Serial.print("freeListTop: " + String(freeListTop) + "\t");
	if (freeListTop == MAX_ITEMS) {
		if (config.debugUtils.showSerial == true)
			Serial.print("\n\n" + String(termColor("red")) + "Free list overflow" + String(termColor("reset")) + "\n\n");
	} else {
		if (config.debugUtils.showSerial == true)
			Serial.print("\n>>>   Freeing semaphore item\n\n");
		freeList[freeListTop++] = infoBuffer;
	}
	xSemaphoreGive(freeListMutex);
}

const char* wl_status_to_string(int status)
{
	switch (status) {
		case 0:
			return "Idle";
		case 1:
			return "No networks available";
		case 2:
			return "Scan completed";
		case 3:
			return "Connected";
		case 4:
			return "Connection failed";
		case 5:
			return "Connection lost";
		case 6:
			return "Disconnected";
		case 255:
			return "No Wi-Fi shield detected";
		default:
			return "Unknown status";
	}
}


#pragma endregion -- DEV UTILITIES

#pragma region -- CONFIG LOADING

void loadDefaultConfig(JsonDocument& doc)
{
	if (!SPIFFS.exists("/config_default.json")) {
		Serial.print(String(termColor("red")) + "No default config file found!" + String(termColor("reset")) + "\n");
		return;
	}
	File configFile = SPIFFS.open("/config_default.json", "r");
	if (configFile) {
		Serial.println("Successfully opened config_default.json");
	} else {
		Serial.println("Failed to open default config file");
		return;
	}

	size_t size = configFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);
	deserializeJson(doc, buf.get());
	configFile.close();
}

void loadUserConfig(JsonDocument& doc)
{
	if (!SPIFFS.exists("/config_user.json")) {
		Serial.print("No user config file found\n");
		return;
	}

	File configFile = SPIFFS.open("/config_user.json", "r");

	if (configFile) {
		Serial.println("Successfully opened config_user.json");
	} else {
		Serial.println("Failed to open user config file, falling back to defaults");
		return;
	}

	size_t size = configFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);
	deserializeJson(doc, buf.get());
	configFile.close();
}

void merge(JsonObject& dest, const JsonObject& src)
{
	for (JsonPair kvp : src) {
		if (kvp.value().is<JsonObject>() && dest.containsKey(kvp.key())) {
			JsonObject destChild = dest[kvp.key()].as<JsonObject>();
			merge(destChild, kvp.value().as<JsonObject>());
		} else {
			dest[kvp.key()] = kvp.value();
		}
	}
}

void loadConfig()
{
	DynamicJsonDocument defaultDoc(2048);
	DynamicJsonDocument userDoc(2048);

	loadDefaultConfig(defaultDoc);
	loadUserConfig(userDoc);

	JsonObject defaultObj = defaultDoc.as<JsonObject>();
	JsonObject userObj = userDoc.as<JsonObject>();

	merge(defaultObj, userObj);

	// Use defaultObj to extract values to struct
	// Debug Utilities
	if (defaultObj.containsKey("debugUtils")) {
		JsonObject debugUtils = defaultObj["debugUtils"];
		if (debugUtils.containsKey("testCores"))
			config.debugUtils.testCores = debugUtils["testCores"];
		if (debugUtils.containsKey("showSerial"))
			config.debugUtils.showSerial = debugUtils["showSerial"];
		if (debugUtils.containsKey("diagMeasure"))
			config.debugUtils.diagMeasure = debugUtils["diagMeasure"];
		if (debugUtils.containsKey("disableWiFi"))
			config.debugUtils.disableWiFi = debugUtils["disableWiFi"];
		if (debugUtils.containsKey("testLEDs"))
			config.debugUtils.testLEDs = debugUtils["testLEDs"];
	}

	// WiFi Network
	if (defaultObj.containsKey("wifiNetwork")) {
		JsonObject wifiNetwork = defaultObj["wifiNetwork"];
		if (wifiNetwork.containsKey("apSSID"))
			config.wifiNetwork.apSSID = wifiNetwork["apSSID"];
		if (wifiNetwork.containsKey("apPass"))
			config.wifiNetwork.apPass = wifiNetwork["apPass"];
		if (wifiNetwork.containsKey("serverName"))
			config.wifiNetwork.serverName = wifiNetwork["serverName"];
		if (wifiNetwork.containsKey("startAP"))
			config.wifiNetwork.startAP = wifiNetwork["startAP"];
		if (wifiNetwork.containsKey("wmTimeout"))
			config.wifiNetwork.wmTimeout = wifiNetwork["wmTimeout"];
		if (wifiNetwork.containsKey("forceDummyData"))
			config.wifiNetwork.forceDummyData = wifiNetwork["forceDummyData"];
		if (wifiNetwork.containsKey("noTargetLimit"))
			config.wifiNetwork.noTargetLimit = wifiNetwork["noTargetLimit"];
		if (wifiNetwork.containsKey("retryDataFetchLimit"))
			config.wifiNetwork.retryDataFetchLimit = wifiNetwork["retryDataFetchLimit"];
	}

	// Pins Hardware
	if (defaultObj.containsKey("pinsHardware")) {
		JsonObject pinsHardware = defaultObj["pinsHardware"];
		if (pinsHardware.containsKey("outerPin"))
			config.pinsHardware.outerPin = pinsHardware["outerPin"];
		if (pinsHardware.containsKey("middlePin"))
			config.pinsHardware.middlePin = pinsHardware["middlePin"];
		if (pinsHardware.containsKey("innerPin"))
			config.pinsHardware.innerPin = pinsHardware["innerPin"];
		if (pinsHardware.containsKey("bottomPin"))
			config.pinsHardware.bottomPin = pinsHardware["bottomPin"];
		if (pinsHardware.containsKey("wifiRst"))
			config.pinsHardware.wifiRst = pinsHardware["wifiRst"];
		if (pinsHardware.containsKey("outputEnable"))
			config.pinsHardware.outputEnable = pinsHardware["outputEnable"];
		if (pinsHardware.containsKey("potentiometer"))
			config.pinsHardware.potentiometer = pinsHardware["potentiometer"];
	}

	// Display LED
	if (defaultObj.containsKey("displayLED")) {
		JsonObject displayLED = defaultObj["displayLED"];
		if (displayLED.containsKey("brightness"))
			config.displayLED.brightness = displayLED["brightness"];
		if (displayLED.containsKey("fps"))
			config.displayLED.fps = displayLED["fps"];
		if (displayLED.containsKey("outerPixelsTotal"))
			config.displayLED.outerPixelsTotal = displayLED["outerPixelsTotal"];
		if (displayLED.containsKey("middlePixelsTotal"))
			config.displayLED.middlePixelsTotal = displayLED["middlePixelsTotal"];
		if (displayLED.containsKey("innerPixelsTotal"))
			config.displayLED.innerPixelsTotal = displayLED["innerPixelsTotal"];
		if (displayLED.containsKey("bottomPixelsTotal"))
			config.displayLED.bottomPixelsTotal = displayLED["bottomPixelsTotal"];
		if (displayLED.containsKey("outerChunks"))
			config.displayLED.outerChunks = displayLED["outerChunks"];
		if (displayLED.containsKey("middleChunks"))
			config.displayLED.middleChunks = displayLED["middleChunks"];
		if (displayLED.containsKey("innerChunks"))
			config.displayLED.innerChunks = displayLED["innerChunks"];
		if (displayLED.containsKey("bottomChunks"))
			config.displayLED.bottomChunks = displayLED["bottomChunks"];
		if (displayLED.containsKey("craftDelay"))
			config.displayLED.craftDelay = displayLED["craftDelay"];
		if (displayLED.containsKey("displayMinDuration"))
			config.displayLED.displayMinDuration = displayLED["displayMinDuration"];
	}

	// Text Typography
	if (defaultObj.containsKey("textTypography")) {
		JsonObject textTypography = defaultObj["textTypography"];
		if (textTypography.containsKey("characterWidth"))
			config.textTypography.characterWidth = textTypography["characterWidth"];
		if (textTypography.containsKey("textMeteorGap"))
			config.textTypography.textMeteorGap = textTypography["textMeteorGap"];
		if (textTypography.containsKey("meteorOffset"))
			config.textTypography.meteorOffset = textTypography["meteorOffset"];
	}

	// Timers Delays
	if (defaultObj.containsKey("timersDelays")) {
		JsonObject timersDelays = defaultObj["timersDelays"];
		if (timersDelays.containsKey("timerDelay"))
			config.timersDelays.timerDelay = timersDelays["timerDelay"];
	}

	// Miscellaneous
	if (defaultObj.containsKey("miscellaneous")) {
		JsonObject miscellaneous = defaultObj["miscellaneous"];
		if (miscellaneous.containsKey("colorTheme"))
			config.miscellaneous.colorTheme = miscellaneous["colorTheme"];
	}
}

#pragma endregion -- CONFIG LOADING

#pragma region -- WIFIMANAGER HANDLING

void doWiFiManager()
{
	try {
		// Process Wifi Manager portal
		if (portalRunning) {
			// Serial.println("!!!!!    PORTAL RUNNING          !!!!!");
			wm.process(); // do processing

			// check for timeout
			if ((millis() - wmStartTime) > (config.wifiNetwork.wmTimeout * 1000)) {
				Serial.println("portaltimeout");
				portalRunning = false;
				if (startAP) {
					wm.stopConfigPortal();
				} else {
					wm.stopWebPortal();
				}
			}
		}

		// Force Wifi portal when WiFi reset button is pressed
		if (digitalRead(WIFI_RST) == LOW && (!portalRunning)) {
			if (startAP) {
				Serial.println("Button Pressed, Starting Config Portal");
				wm.setConfigPortalBlocking(false);
				wm.startConfigPortal(config.wifiNetwork.apSSID, config.wifiNetwork.apPass);
			} else {
				Serial.println("Button Pressed, Starting Web Portal");
				wm.startWebPortal();
			}
			portalRunning = true;
			wmStartTime = millis();
		}
	}
	catch (...) {
		handleException();
	}
}

void setColorTheme(uint8_t colorTheme)
{
	switch (colorTheme) {
		case 0:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 0;
			break;
		case 1:
			currentColors.letter = mpColors.aqua.value;
			currentColors.meteor = mpColors.aqua.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 255;
			break;
		case 2:
			currentColors.letter = mpColors.pink.value;
			currentColors.meteor = mpColors.pink.value;
			currentColors.tailHue = 192;
			currentColors.tailSaturation = 255;
			break;
		case 3:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 127;
			break;
		default:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 127;
	}
}

void setAnimationParams(uint8_t newGlobalFps, bool newMeteorTailDecay, bool newMeteorTailRandom)
{
	fpsRate = newGlobalFps;
	meteorTailDecay = newMeteorTailDecay;
	meteorTailRandom = newMeteorTailRandom;
}

String getParam(String name)
{
	// read parameter from server, for customhmtl input
	if (wm.server->hasArg(name) == 0) {
		return "0";
	}

	String value = wm.server->arg(name);
	return value;
}

void saveColorThemeCallback()
{
	try {
		String colorTheme = getParam("customfieldid");
		String inputMeteorTailDecay = getParam("meteorDecay");
		String inputMeteorTailRandom = getParam("meteorRandom");
		String inputGlobalFps = getParam("globalFps");
		const int colorThemeId = atoi(colorTheme.c_str());
		const int inputMeteorTailDecayValue = strcmp(inputMeteorTailDecay.c_str(), "on") == 0 ? 1 : 0;
		const int inputMeteorTailRandomValue = strcmp(inputMeteorTailRandom.c_str(), "on") == 0 ? 1 : 0;
		const int inputGlobalFpsValue = atoi(inputGlobalFps.c_str());

		Serial.println("[CALLBACK] saveColorThemeCallback fired");
		Serial.print("PARAM customfieldid = ");
		Serial.println(colorTheme);
		Serial.print("PARAM meteorDecay = ");
		Serial.println(inputMeteorTailDecay);
		Serial.print("PARAM meteorDecay = ");
		Serial.println(inputMeteorTailDecayValue);
		Serial.print("PARAM meteorDecay = ");
		Serial.println(inputMeteorTailRandom);
		Serial.print("PARAM meteorRandom = ");
		Serial.println(inputMeteorTailRandomValue);
		Serial.print("PARAM globalFps = ");
		Serial.println(inputGlobalFpsValue);

		setColorTheme(colorThemeId);
		setAnimationParams(inputGlobalFpsValue, inputMeteorTailDecayValue, inputMeteorTailRandomValue);
	}
	catch (...) {
		handleException();
	}
}

#pragma endregion -- END WIFIMANAGER HANDLING

#pragma region -- HARDWARE UTILS
int allStripsLength = sizeof(allStrips) / sizeof(allStrips[0]);
void allStripsShow(void)
{
	// for (int i = 0; i < 4; i++)
	// {
	// 	allStrips[i]->show();
	// }

	FastLED.show();
}

void allStripsOff(void)
{
	// for (int i = 0; i < allStripsLength; i++)
	// {
	// 	allStrips[i]->clear();
	// 	allStripsShow();
	// }

	FastLED.clear();
}

#pragma endregion -- HARDWARE UTILS

#pragma region -- ANIMATION FUNCTIONS

// Logic to update speed counters
void updateSpeedCounters()
{
	counterHalfSpeed == 2 ? counterHalfSpeed = 1 : counterHalfSpeed++;
	counterThirdSpeed == 3 ? counterThirdSpeed = 1 : counterThirdSpeed++;
	counterQuarterSpeed == 4 ? counterQuarterSpeed = 1 : counterQuarterSpeed++;
}

// Utility function to reverse elements of an array
void reverseStripsArray(void)
{
	reverse(allStrips, allStrips + 4);
}

// Display letter from array
void doLetterRegions(char theLetter, int regionStart, int startingPixel)
{
	const TextCharacter::TextCharacterInfo ledCharacter = textCharacter.getCharacter(theLetter, characterWidth);
	const uint16_t regionOffset = innerPixelsChunkLength * regionStart;

	int16_t pixel = 0 + startingPixel + regionOffset;

	const int16_t previousPixel = startingPixel + regionOffset - letterSpacing;

	for (int i = 0; i < ledCharacter.characterTotalPixels; i++) {
		int j = i + 1;
		int regionInt = j % characterWidth;
		if (regionInt == 0)
			regionInt = characterWidth;
		--regionInt;

		int drawPixel = (pixel + (innerPixelsChunkLength * regionInt)) % innerPixelsTotal; // Wrapping here
		int drawPreviousPixel = (previousPixel + (innerPixelsChunkLength * regionInt)) % innerPixelsTotal; // Wrapping here

		int regionStart = (innerPixelsChunkLength * regionInt + regionOffset) % innerPixelsTotal;
		int regionEnd = (innerPixelsChunkLength * (regionInt + 1) + regionOffset) % innerPixelsTotal;

		const size_t inner_leds_size = sizeof(inner_leds) / sizeof(inner_leds[0]);

		bool drawPixelInRegion = (regionStart <= drawPixel && drawPixel < regionEnd) ||
			(regionStart > regionEnd && (drawPixel < regionEnd || drawPixel >= regionStart));

		bool drawPrevPixelInRegion = (regionStart <= drawPreviousPixel && drawPreviousPixel < regionEnd) ||
			(regionStart > regionEnd && (drawPreviousPixel < regionEnd || drawPreviousPixel >= regionStart));

		if (drawPrevPixelInRegion) {
			if (drawPreviousPixel >= 0 && drawPreviousPixel < inner_leds_size) {
				inner_leds[drawPreviousPixel] = mpColors.off.value;
			} else {
				if (config.debugUtils.showSerial == true)
					Serial.print(String(termColor("red")) + "drawPreviousPixel out of bounds" + String(termColor("reset")) + "\n");
			}
		}
		if (drawPixelInRegion) {
			if (drawPixel >= 0 && drawPixel < inner_leds_size) {
				if (ledCharacter.characterArray[i] == 1)
					inner_leds[drawPixel] = currentColors.letter;
				else
					inner_leds[drawPixel] = mpColors.off.value;
			} else {
				if (config.debugUtils.showSerial == true)
					Serial.print(String(termColor("red")) + "drawPixel out of bounds" + String(termColor("reset")) + "\n");
			}
		}

		if (regionInt == characterWidth - 1)
			pixel--;
	}
}

// Updates scrolling letters on inner strips
void scrollLetters(const char* spacecraftName, int wordArraySize)
{
	int regionStart = 4;
	static int16_t startPixel = 0;
	int letterPixel = startPixel;

	/* Do each letter in spacecraft name */
	for (int i = 0; i < wordArraySize; i++) {
		char theLetter = spacecraftName[i];

		if (characterWidth == 4) {
			doLetterRegions(theLetter, (innerChunks - 1), letterPixel);	// Start 1 strip to the left of first strip
		} else {
			doLetterRegions(theLetter, 0, letterPixel);
			// doLetterRegions(theLetter, 7, letterPixel);
			// doLetterRegions(theLetter, 8, letterPixel);
		}

		letterPixel = letterPixel - letterSpacing - characterKerning; // Move to next letter
	}

	int wrapPixel = innerPixelsChunkLength + (wordArraySize * (characterHeight + characterKerning)); // Calculate pixel to wrap at
	startPixel++; // Move to next starting pixel

	if (startPixel > wrapPixel) {
		startPixel = 0;
		if (millis() - displayDurationTimer > displayMinDuration) {
			nameScrollDone = true;
			animationTypeSetDown = false;
			animationTypeSetUp = false;
			animateFirstCycleDown = true;
			animateFirstCycleUp = true;
			craftDelayTimer = millis();
		}
	}
}

// Create Meteor object
void createMeteor(int strip, int region, bool directionDown = true, int startPixel = 0, uint8_t meteorSize = 1, bool hasTail = true, float meteorTailDecayValue = 0.85)
{
	CHSV meteorColor = currentColors.meteor;

	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] != nullptr) {
			// Serial.print("Could not create meteor #"); Serial.print(i); Serial.print(" startPixel: "); Serial.println(animate.ActiveMeteors[i]->firstPixel);
			continue;
		}

		animate.ActiveMeteors[i] = new Meteor{
			directionDown,				  // directionDown
			startPixel,					  // firstPixel
			region,						  // region
			(int)outerPixelsChunkLength,  // regionLength
			meteorColor,				  // pColor
			meteorSize,					  // meteorSize
			hasTail,					  // hasTail
			meteorTailDecay,			  // meteorTrailDecay
			meteorTailDecayValue,		  // meteorTrailDecayValue
			meteorTailRandom,			  // meteorRandomDecay
			currentColors.tailHue,		  // tailHueStart
			true,						  // tailHueAdd
			0.75,						  // tailHueExponent
			currentColors.tailSaturation, // tailHueSaturation
			allStrips[strip]			  // rStrip
		};
		// Serial.print("Created new meteor #"); Serial.print(i); Serial.print(" startPixel: "); Serial.println(animate.ActiveMeteors[i]->firstPixel);

		break;
	}

	if (animate.ActiveMeteorsSize > 499)
		animate.ActiveMeteorsSize = 0;
}

void animationMeteorPulseRegion(
	uint8_t strip,
	uint8_t region,
	bool directionDown = true,
	int16_t startPixel = 0,
	uint8_t pulseCount = 2,
	int8_t offset = 10,
	bool randomizeOffset = false,
	uint8_t meteorSize = 1,
	bool hasTail = true,
	float meteorTailDecayValue = 0.85)
{

	// Stagger the starting pixel
	if (randomizeOffset == true)
		startPixel = startPixel - ((rand() % offsetHalf + 1) * 2);

	for (int i = 0; i < pulseCount; i++) {
		int16_t pixel = i + startPixel + (i * offset * -1);
		if (randomizeOffset == true)
			pixel = pixel - (random(0, 6) * 2);
		createMeteor(strip, region, directionDown, pixel, meteorSize, hasTail, meteorTailDecayValue);
	}
}

void animationMeteorPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t pulseCount = 2,
	int8_t offset = 32,
	bool randomizeOffset = false,
	uint8_t meteorSize = 1,
	bool hasTail = true)
{
	for (int i = 0; i < outerChunks; i++) {
		animationMeteorPulseRegion(
			strip,			 // strip
			i,				 // region
			directionDown,	 // directionDown
			0,				 // startPixel
			pulseCount,		 // pulseCount
			offset,			 // offset
			randomizeOffset, // randomizeOffset
			meteorSize,		 // meteorSize
			hasTail			 // hasTail
		);
	}
}

void animationSpiralPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t height = 2,
	uint8_t pulseCount = 2,
	uint8_t spiralMultiplier = 6,
	uint8_t repeats = 4)
{

	for (int i = 0; i < outerChunks; i++) {
		animationMeteorPulseRegion(strip, i, directionDown, (i * spiralMultiplier * -1), 5, ((outerChunks + 1) * spiralMultiplier), false, height, false);
		// animationMeteorPulseRegion(strip, i, directionDown, (i * spiralMultiplier * -1) - height - 6, 5, ((outerChunks + 1) * spiralMultiplier), false, height, true, 0.9);
	}
}

void waveAnimation(uint8_t strip, uint8_t numberOfWaves, uint8_t waveSize)
{
	for (uint8_t wave = 0; wave < numberOfWaves; wave++) {
		for (uint8_t region = 0; region < outerChunks; region++) {
			int startPixel = wave * waveSize;
			if (region < outerChunks / 2) {
				startPixel = startPixel - (8 * region);
			} else {
				startPixel = startPixel - (8 * (outerChunks - region));
			}
			createMeteor(strip, region, true, startPixel, waveSize, true);
		}
	}
}

void zigzagAnimation(uint8_t strip, uint8_t zigzagSize)
{
	for (uint8_t region = 0; region < outerChunks; region++) {
		bool directionDown = region % 2 == 0;
		for (uint8_t i = 0; i < outerPixelsChunkLength; i += zigzagSize) {
			createMeteor(strip, region, directionDown, i, 1, false);
			directionDown = !directionDown;
		}
	}
}

void laserGunAnimation(uint8_t strip, uint16_t chargeTime, uint8_t firingSize)
{
	// Charging up animation
	for (uint8_t i = 0; i < outerPixelsChunkLength; ++i) {
		for (uint8_t region = 0; region < outerChunks; ++region) {
			createMeteor(strip, region, true, i, 1, false);
		}
		delay(chargeTime / outerPixelsChunkLength);
	}

	// Firing animation
	for (uint8_t i = 0; i < outerPixelsChunkLength - firingSize + 1; ++i) {
		for (uint8_t region = 0; region < outerChunks; ++region) {
			createMeteor(strip, region, true, i, firingSize, true);
		}
		delay(chargeTime / (outerPixelsChunkLength * 2));
	}
}

// void animationRadialSpiral(uint8_t strip, uint8_t stepSize) {
//   static uint8_t spiralStep = 0;

//   for (uint8_t region = 0; region < outerChunks; ++region) {
//     int startPixel = (spiralStep + (region * stepSize)) % outerPixelsChunkLength;
//     createMeteor(strip, region, true, startPixel, 1, false);
//   }

//   spiralStep += stepSize;
//   if (spiralStep >= outerPixelsChunkLength) {
//     spiralStep = 0;
//   }
// }

void doRateBasedAnimation(bool isDown, uint8_t rateClass, uint8_t offset, uint8_t type)
{
	// Serial.print("type: "); Serial.println(type);
	// Serial.print("rateClass: "); Serial.println(rateClass);
	int stripId = isDown == true ? 2 : 1;
	int pulseCount = 1;
	// Serial.print("rateClass: "); Serial.println(rateClass);
	// Serial.print("type: "); Serial.println(type);
	// Serial.println("------------");
	// Serial.println();

	uint8_t adjustedType = type;

	// Serial.println("doing rate based animation");

	switch (rateClass) {
		case 6:
		{ // 1gbps
			// If this is the first animation cycle, do a random fancy animation. Otherwise, do meteors.
			if (isDown == true && animateFirstCycleDown == false)
				adjustedType = 0;
			if (isDown == false && animateFirstCycleUp == false)
				adjustedType = 0;

			switch (adjustedType) {
				case (1):
				{
					// Ring
					pulseCount = 8;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 26, false, 6, false);
					break;
				}

				case (2):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 4;
					uint8_t spiralOffset = 4;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				case (3):
				{
					// Wave
					// pulseCount = 8;
					// uint8_t height = 8;
					// uint8_t spiralOffset = 4;
					// uint8_t repeats = 10;
					// animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					waveAnimation(stripId, 4, 8);
					break;
				}

				default:
				{
					// Meteors
					pulseCount = 14;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 32, true, 1, true);
					break;
				}
			}

			break;
		}
		case 5:
		{ // 1mbps
			// If this is the first animation cycle, do a random fancy animation. Otherwise, do meteors.
			if (isDown == true && animateFirstCycleDown == false)
				adjustedType = 0;
			if (isDown == false && animateFirstCycleUp == false)
				adjustedType = 0;

			switch (adjustedType) {
				case (1):
				{
					// Ring
					pulseCount = 6;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 32, false, 4, false);
					break;
				}

				case (2):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 4;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 14;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				case (3):
				{
					// Wave
					// pulseCount = 8;
					// uint8_t height = 6;
					// uint8_t spiralOffset = 6;
					// uint8_t repeats = 10;
					// animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					waveAnimation(stripId, 4, 4);
					break;
				}

				default:
				{
					// Meteors
					pulseCount = 8;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 16, true, 1, true);
					break;
				}
			}

			break;
		}
		case 4:
		{ // 10kbps
			// If this is the first animation cycle, do a random fancy animation. Otherwise, do meteors.
			if (isDown == true && animateFirstCycleDown == false)
				adjustedType = 0;
			if (isDown == false && animateFirstCycleUp == false)
				adjustedType = 0;

			switch (adjustedType) {
				case (1):
				{
					// Ring
					pulseCount = 4;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 32, false, 3, false);
					break;
				}

				case (2):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 4;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				case (3):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 3;
					uint8_t spiralOffset = 8;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				default:
				{
					// Meteors
					pulseCount = 4;
					animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true, 1, true);
					break;
				}
			}

			break;
		}
		case 3:
		{
			// If this is the first animation cycle, do a random fancy animation. Otherwise, do meteors.
			if (isDown == true && animateFirstCycleDown == false)
				adjustedType = 0;
			if (isDown == false && animateFirstCycleUp == false)
				adjustedType = 0;

			switch (adjustedType) {
				case (1):
				{
					// Ring
					pulseCount = 3;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 64, false, 3, false);
					break;
				}

				case (2):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 2;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				case (3):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 4;
					uint8_t spiralOffset = 12;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				default:
				{
					// Meteors
					pulseCount = 2;
					animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true, 1, true);
					break;
				}
			}

			break;
		}
		case 2:
		{

			switch (adjustedType) {
				case (1):
				{
					// Ring
					pulseCount = 2;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 96, false, 2, false);
					break;
				}

				case (2):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 2;
					uint8_t spiralOffset = 8;
					uint8_t repeats = 10;
					if (counterHalfSpeed == 1)
						animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				case (3):
				{
					// Spiral
					pulseCount = 8;
					uint8_t height = 1;
					uint8_t spiralOffset = 8;
					uint8_t repeats = 10;
					if (counterHalfSpeed == 1)
						animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}

				default:
				{
					// Meteors
					pulseCount = 2;
					animationMeteorPulseRing(stripId, isDown, pulseCount, 64, true, 1, true);
					break;
				}
			}

			break;
		}
		case 1:
		{
			// Meteors
			pulseCount = 1;
			if (counterQuarterSpeed == 1)
				animationMeteorPulseRing(stripId, isDown, pulseCount, 32, true, 1, true);
			break;
		}
		case 0:
		{
			FastLED.clear(allStrips[stripId]);
			break;
		}
		default:
		{
			FastLED.clear(allStrips[stripId]);
		}
	}

	// The first animation has fired for this craft
	// The down & up animations happen on separate cycles so we only set the current direction
	isDown == true ? animateFirstCycleDown = false : animateFirstCycleUp = false;

	// if (rateClass == 0) {
	// 	// allStrips[stripId]->clear();
	// 	FastLED.clear(allStrips[stripId]);
	// 	return;
	// }

	switch (type) {
		// Meteor
		case 0:
		{
			switch (rateClass) {
				case 6:
				{ // 1gbps
					pulseCount = 14;
					// Serial.print("[Downsignal Gb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 12, true);
					break;
				}
				case 5:
				{ // 1mbps
					pulseCount = 8;
					// Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 16, true);
					break;
				}
				case 4:
				{ // 10kbps
					pulseCount = 4;
					// Serial.print("[Downsignal kb]");
					uint8_t pickRandom = random8(2);
					Serial.print("rate class 4, anim type: ");
					Serial.println(pickRandom);
					animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
					break;
				}
				case 3:
				{
					pulseCount = 2;
					// Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
					break;
				}
				case 2:
				{
					pulseCount = 1;
					// Serial.print("[Downsignal kb]");
					if (counterHalfSpeed == 1)
						animationMeteorPulseRing(stripId, isDown, pulseCount, 16, true);
					break;
				}
				case 1:
				{
					// Serial.print("[Downsignal slow]");
					// animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, true);
					// animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, true);
					if (counterQuarterSpeed == 1)
						animationMeteorPulseRing(stripId, isDown, 1, 12, true);
					break;
				}
				case 0:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal -- ]");
					break;
				}
				default:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal n/a ]");
				}
			}
		}

		// Ring
		case 1:
		{
			switch (rateClass) {
				case 6:
				{ // 1gbps
					pulseCount = 14;
					// Serial.print("[Downsignal Gb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 10, false);
					break;
				}
				case 5:
				{ // 1mbps
					pulseCount = 8;
					// Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 16, false);
					break;
				}
				case 4:
				{ // 10kbps
					pulseCount = 4;
					// Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 24, false);
					break;
				}
				case 3:
				{
					pulseCount = 2;
					// Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(stripId, isDown, pulseCount, 32, false);
					break;
				}
				case 2:
				{
					pulseCount = 1;
					// Serial.print("[Downsignal kb]");
					if (counterHalfSpeed == 1)
						animationMeteorPulseRing(stripId, isDown, pulseCount, 38, false);
					break;
				}
				case 1:
				{
					// Serial.print("[Downsignal slow]");
					// animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, false);
					// animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, false);
					if (counterQuarterSpeed == 1)
						animationMeteorPulseRing(stripId, isDown, 1, 42, false);
					break;
				}
				case 0:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal -- ]");
					break;
				}
				default:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal n/a ]");
				}
			}
		}

		// Spiral
		case 2:
		{
			switch (rateClass) {
				case 6:
				{ // 1gbps
					// Serial.print("[Downsignal Gb]");
					pulseCount = 8;
					uint8_t height = 6;
					uint8_t spiralOffset = 4;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 5:
				{ // 1mbps
					// Serial.print("[Downsignal kb]");
					pulseCount = 8;
					uint8_t height = 6;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 4:
				{ // 10kbps
					// Serial.print("[Downsignal kb]");
					pulseCount = 8;
					uint8_t height = 4;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 3:
				{
					// Serial.print("[Downsignal kb]");
					pulseCount = 8;
					uint8_t height = 2;
					uint8_t spiralOffset = 6;
					uint8_t repeats = 10;
					animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 2:
				{
					// Serial.print("[Downsignal kb]");
					pulseCount = 8;
					uint8_t height = 2;
					uint8_t spiralOffset = 8;
					uint8_t repeats = 10;
					if (counterHalfSpeed == 1)
						animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 1:
				{
					// Serial.print("[Downsignal slow]");
					pulseCount = 8;
					uint8_t height = 1;
					uint8_t spiralOffset = 10;
					uint8_t repeats = 10;
					if (counterQuarterSpeed == 1)
						animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
					break;
				}
				case 0:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal -- ]");
					break;
				}
				default:
				{
					// allStrips[stripId]->clear();
					FastLED.clear(allStrips[stripId]);
					// Serial.print("[Downsignal n/a ]");
				}
			}

			// pulseCount = 8;
			// uint8_t height = 2;
			// uint8_t spiralOffset = 4;
			// uint8_t repeats = 10;
			// animationSpiralPulseRing(stripId, isDown, height, pulseCount, spiralOffset, repeats);
			// waveAnimation(stripId, 4, 4);
			// zigzagAnimation(stripId, 4);
			// laserGunAnimation(stripId, 4, 4);
			// animationRadialSpiral(stripId, 4);
		}
	}
}

void drawMeteors()
{
	// Update meteor animations
	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] != nullptr)
			animate.animateMeteor(animate.ActiveMeteors[i]);
	}
}

void drawBottomPixels()
{
	// Update bottom pixels
	for (int i = 0; i < 5; i++) {
		bottomPixelMap[i] = CHSV(0, 255 - ((255 / bottomPixelsTotal) * i), 255 - ((255 / bottomPixelsTotal) * i));

		bottom_leds[i] = bottomPixelMap[i];
	}
}

void updateBottomPixels()
{
	for (int i = 0; i < 5; i++) {
		uint8_t newHue = bottomPixelMap[i].hue + 1;
		bottom_leds[i].setHue(newHue);
		bottomPixelMap[i].hue = newHue;
	}
}

void updateMeteors()
{
	// Update first pixel location for all active Meteors in array
	uint16_t activeMeteors = 0;
	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] != nullptr) {
			animate.ActiveMeteors[i]->firstPixel = animate.ActiveMeteors[i]->firstPixel + 2;

			// If meteor is beyond the display region, unallocate memory and remove array item
			if (animate.ActiveMeteors[i]->firstPixel > animate.ActiveMeteors[i]->regionLength * 2) {
				delete animate.ActiveMeteors[i];
				animate.ActiveMeteors[i] = nullptr;
			}
		}

		if (config.debugUtils.diagMeasure == true) {
			// Count the active meteors for diagnostic output
			if (animate.ActiveMeteors[i] != nullptr)
				activeMeteors++;
		}
	}

	if (config.debugUtils.diagMeasure == true) {
		Serial.print("Active_Meteors:");
		Serial.print(activeMeteors);
		Serial.print("\t");
	}
}

/** Main Animation update function
 * Gets called every loop();
 */
void updateAnimation(const char* spacecraftName, int spacecraftNameSize, int downSignalRate, int upSignalRate)
{
	// Serial.println("updateAnimation()");
	if (config.debugUtils.diagMeasure == true) {
		Serial.print("FPS:");
		Serial.print(FastLED.getFPS() * 1);
		Serial.print("\t");
	}

	// Update brightness from potentiometer
	au.updateBrightness();

	/* Update Scrolling letters animation */
	if (nameScrollDone == false && counterHalfSpeed == 1) {
		// Serial.println("---");
		// Serial.println("nameScrollDone == false && counterHalfSpeed == 1");
		// Serial.println("---");
		try {
			// Serial.println("Scrolling letters");
			scrollLetters(spacecraftName, spacecraftNameSize);
		}
		catch (...) {
			Serial.println("Error in scrollLetters()");
		}
	}

	if (animationTypeSetDown == false) {
		// Serial.println("---");
		// Serial.println("animationTypeSetDown == false");
		// Serial.println("---");
		// const uint8_t animationId = random8(0,2);
		// const uint8_t animationId = random8(1, 4);
		const uint8_t animationId = 1;
		// Serial.print("roll animation: "); Serial.println(animationId);

		/**
		 * 0 = Meteor/Rain (default)
		 * 1 = Pulse
		 * 2 = Spiral
		 */

		animationTypeDown = animationId;
		animationTypeUp = animationId;
		animationTypeSetDown = true;
	}

	// Serial.println("After animation roll in main update loop");
	// Serial.print("displayDurationTimer: "); Serial.println(displayDurationTimer);
	// Serial.print("displayMinDuration: "); Serial.println(displayMinDuration);
	// Serial.print("diff: "); Serial.println(millis() - animationTimer);
	// Serial.print("textMeteorGap: "); Serial.println(textMeteorGap);

	// Fire meteors
	if (displayDurationTimer > displayMinDuration && (millis() - animationTimer) > textMeteorGap) {
		// Serial.println("---");
		// Serial.println("displayDurationTimer > displayMinDuration && (millis() - animationTimer) > textMeteorGap");
		// Serial.println("---");
		// printMeteorArray();
		// Serial.println("Firing meteors");

		if (nameScrollDone == false) {
			// Serial.println("doing meteor type thigns");
			try {
				doRateBasedAnimation(true, downSignalRate, meteorOffset, animationTypeDown);
				// doRateBasedAnimation(false, upSignalRate, meteorOffset, animationTypeUp);
			}
			catch (...) {
				Serial.println("Error in signal animation");
			}
		}

		animationTimer = millis(); // Reset meteor animation timer
	}

	drawMeteors(); // Assign new pixels for meteors
	FastLED.delay(1000 / fpsRate);
	// if (counterQuarterSpeed == 1)
	updateMeteors(); // Update first pixel location for all active Meteors in array
	updateBottomPixels();

	FastLED.countFPS();
	updateSpeedCounters();
}

#pragma endregion -- ANIMATION FUNCTIONS

#pragma region -- DATA FUNCTIONS

unsigned int rateLongToRateClass(unsigned long rate)
{

	// Add a check for rate ceiling of 100Gbps to catch erroneous values, set to max rate
	unsigned long cappedRate;
	if (rate > 100000000000)
		cappedRate = (1024 * 1000000) + 1;
	else
		cappedRate = rate;

	if (cappedRate > (1024 * 1000000))
		return 6; // > 1gbps
	else if (cappedRate > (1024 * 1024))
		return 5; // > 1mbps
	else if (cappedRate > (1024 * 100))
		return 4; // > 100kbps
	else if (cappedRate > (1024 * 5))
		return 3; // > 5kbps
	else if (cappedRate > 1024)
		return 2; // > 1kbps
	else if (cappedRate > 0)
		return 1; // > < 1kbps
	else
		return 0; // 0
}

void parseData(const char* payload)
{

	/* XML Parsing */
	XMLDocument xmlDocument; // Create variable for XML document
	const char* charPayload = payload; // Convert payload to char array

	/* Handle XML parsing error */
	try {
		if (xmlDocument.Parse(charPayload) == XML_SUCCESS) {
			if (config.debugUtils.showSerial == true)
				Serial.print("\n" + String(termColor("green")) + "XML Parsed Succcessfully" + String(termColor("reset")) + "\n");
		} else {
			Serial.print(termColor("red"));
			Serial.println("Unable to parse XML");
			Serial.println(termColor("reset"));
			return;
		}
	}
	catch (XMLError error) {
		Serial.print(termColor("red"));
		Serial.print("Problem parsing payload:");
		Serial.println(termColor("reset"));
		Serial.print("XML Error: ");
		Serial.println(error);
		return;
	}
	catch (...) {
		Serial.print(termColor("red"));
		Serial.print("Problem parsing payload:");
		Serial.println(termColor("reset"));
		handleException();
		return;
	}

	/* Find XML elements */
	XMLNode* root = xmlDocument.RootElement();					  // Find document root node
	XMLElement* timestamp = root->FirstChildElement("timestamp"); // Find XML timestamp element

	CraftQueueItem* newCraft = nullptr;

	/* Loop through freeListMutex and print out the callsigns */
	if (xSemaphoreTake(freeListMutex, 100) == pdTRUE) {
		printSemaphoreList();
		if (freeListTop > 0) newCraft = freeList[--freeListTop]; // Get a free CraftQueueItem from the freeList
		xSemaphoreGive(freeListMutex);
	} else {
		// Unable to take freeListMutex
		return;
	}

	if (newCraft != nullptr) {

		bool targetFound = false;

		/* Loop through all XML elements */
		// 100 is an arbitrary number to prevent an infinite loop
		for (int i = 0; i < 100; i++) {
			Serial.print("\n───────────────────────────────────────────────────────────────────\n");
			Serial.print("  Parse Counters -- Station: " + String(stationCount) + " | Dish: " + String(dishCount) + " | Target: " + String(targetCount) + " | Signal: " + String(signalCount));
			Serial.print("\n───────────────────────────────────────────────────────────────────\n\n");

			try {
				int s = 0; // Create station elements counter
				for (XMLElement* xmlStation = root->FirstChildElement("station"); true; xmlStation = xmlStation->NextSiblingElement("station")) {
					Serial.print("== stationCount: " + String(stationCount) + " | s: " + String(s) + "\n");

					if (xmlStation == NULL) {
						Serial.print("xmlStation == NULL\n");
						stationCount = 0;
						dishCount = 0;
						targetCount = 0;
						break;
					}

					if (stationCount > 2) {
						stationCount = 0;
						dishCount = 0;
						targetCount = 0;
					}

					if (s > 2) {
						s = 0;
						dishCount = 0;
						targetCount = 0;
					}
					if (s != stationCount) {
						s++;
						continue;
					}

					bool goToNextStation = false;
					int d = 0; // Create dish elements counter
					for (XMLElement* xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement()) {
						delay(1000);
						Serial.print("===== dishCount: " + String(dishCount) + " | d: " + String(d) + "\n");

						if (d > 9) {
							stationCount++;
							if (stationCount > 2)
								stationCount = 0;

							d = 0;
							break;
						}
						if (d != dishCount) {
							d++;
							continue;
						}

						const char* elValue = xmlDish->Value();
						if (strcmp(elValue, "dish") != 0) {
							goToNextStation = true;
							break;
						}

						bool goToNextDish = false;
						int t = 0; // Create target elements counter
						for (XMLElement* xmlTarget = xmlDish->FirstChildElement("target"); true; xmlTarget = xmlTarget->NextSiblingElement("target")) {
							Serial.print("====== targetCount: " + String(targetCount) + " | t: " + String(t) + "\n");

							if (xmlTarget == NULL) {
								Serial.print("                xmlTarget == NULL\n");
								goToNextDish = true;
								break;
							}

							if (t > 9 || t > targetCount) {
								Serial.print("                t > 9\n");
								goToNextDish = true;
								break;
							}
							if (t != targetCount) {
								Serial.print("                t != targetCount\n");
								t++;
								Serial.print("                t++ = " + String(t) + "\n");
								continue;
							}

							Serial.print(">>>   Finding craft....");
							const char* target = xmlTarget->Attribute("name");

							if (data.checkBlacklist(target) == true) {
								t++;
								continue;
							}

							try {
								strlcpy(newCraft->callsignArray, target, 10);
							}
							catch (...) {
								if (config.debugUtils.showSerial == true) {
									Serial.print(termColor("red"));
									Serial.println("Problem copying callsign to newCraft->callsignArray");
									Serial.println(termColor("reset"));
								}
								handleException();
								return;
							}

							const char* name = data.callsignToName(target);

							if (config.debugUtils.showSerial == true) {
								// print callsign
								Serial.print("\n" + String(termColor("yellow")) + "PARSED: (" + String(newCraft->callsign) + ") " + String(name) + String(termColor("reset")) + "\n");
							}

							try {
								strlcpy(newCraft->nameArray, name, 100);
							}
							catch (...) {
								if (config.debugUtils.showSerial == true) {
									Serial.print(termColor("red"));
									Serial.println("Problem copying name to newCraft->nameArray");
									Serial.println(termColor("reset"));
								}
								handleException();
								return;
							}

							// A target has been found and validated
							newCraft->nameLength = strlen(name);
							targetFound = true;
							targetCount = t;
							break;
						} // End target loop


						if (goToNextDish == true) {
							targetCount = 0;
							dishCount++;
							d++;
							Serial.println("go to next dish!");
							continue;
						}

						if (targetFound == true) {

							// Serial.print("NEW CRAFT NAME: " + String(newCraft->name) + "\n");
							// Serial.print("NEW CRAFT NAME LENGTH: " + String(newCraft->nameLength) + "\n");

							// if (newCraft->nameLength < 1) {
							// 	targetCount = 0;
							// 	dishCount++;
							// 	break; // Bail if target name assignment fails
							// }

							// Loop through XML signal elements and find the one that matches the target
							for (XMLElement* xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("downSignal")) {
								const char* spacecraft = xmlSignal->Attribute("spacecraft");
								const char* signalType = xmlSignal->Attribute("signalType");

								if (config.debugUtils.showSerial == true)
									Serial.print(">> Finding signals for " + String(newCraft->callsign) + "\n");

								try {
									if (strcmp(signalType, "data") != 0) // Skip non-data signals
									{
										// Serial.println("Not a data signal, skipping...");
										continue;
									}
									if (strcmp(spacecraft, newCraft->callsign) != 0) // Skip signals that don't match the target
									{
										// Serial.print(spacecraft); Serial.print(" != "); Serial.println(newCraft->callsign);
										// Serial.println("Not the right spacecraft, skipping...");
										continue;
									}
								}
								catch (...) {
									if (config.debugUtils.showSerial == true) {
										Serial.print(termColor("red"));
										Serial.println("Problem parsing payload:");
										Serial.println(termColor("reset"));
										handleException();
									}
									return;
								}


								const char* downRate = xmlSignal->Attribute("dataRate");

								if (config.debugUtils.showSerial == true)
									Serial.print(String(termColor("yellow")) + "Down rate: " + String(downRate) + String(termColor("reset")));

								unsigned long downRateLong = stol(downRate, nullptr, 10);
								if (downRateLong == 0)
									continue;
								unsigned int downRateClass = rateLongToRateClass(downRateLong);
								if (downRateClass == 0)
									continue;
								newCraft->downSignal = downRateClass;
								break;
							}

							for (XMLElement* xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("upSignal")) {
								const char* spacecraft = xmlSignal->Attribute("spacecraft");
								const char* signalType = xmlSignal->Attribute("signalType");


								/* Skip non-data signals or signals that don't match the target */
								try {
									if ((strcmp(xmlSignal->Attribute("signalType"), "data") != 0) || (strcmp(spacecraft, newCraft->callsign) != 0))
										continue;
								}
								catch (...) {
									if (config.debugUtils.showSerial == true) {
										Serial.print(termColor("red"));
										Serial.println("Problem parsing payload:");
										Serial.println(termColor("reset"));
									}
									handleException();
									return;
								}

								/* Get uplink rate */
								const char* upRate = xmlSignal->Attribute("dataRate");

								if (config.debugUtils.showSerial == true)
									Serial.print(String(termColor("yellow")) + "Up rate:   " + String(upRate) + String(termColor("reset")));

								/* Convert uplink rate to rate class */
								unsigned long upRateLong = stol(upRate, nullptr, 10);
								if (upRateLong == 0) continue;
								unsigned int upRateClass = rateLongToRateClass(upRateLong);
								if (upRateClass == 0) continue;
								newCraft->upSignal = upRateClass;
								break;
							}

							dishCount = d;
							break;
						}

					} // End dish loop

					if (goToNextStation == true) {
						targetCount = 0;
						dishCount = 0;
						stationCount++;
						s++;

						if (stationCount > 2) {
							stationCount = 0;
							s = 0;
						}
						Serial.println("go to next station!");
						continue;
					}
					if (targetFound == true) {
						stationCount = s;
						break;
					}

					s++;
				} // End station loop
			}
			catch (...) {
				if (config.debugUtils.showSerial == true) {
					Serial.print(termColor("red"));
					Serial.println("Problem parsing payload:");
					Serial.println(termColor("reset"));
				}
				handleException();
				parseCounter++;
				return;
			}

			if (targetFound == true) break;
		} // End arbitary loop


		/* Target found */
		if (targetFound) {
			if (strlen(newCraft->name) != 0 && (newCraft->downSignal != 0 || newCraft->upSignal != 0)) {

				if (config.debugUtils.showSerial == true) {
					Serial.print("\n" + String(termColor("yellow")) + ">>--------=> Sending to queue >>--------=>" + String(termColor("reset") + "\n"));
					printCraftInfo(0, newCraft->callsign, newCraft->name, newCraft->nameLength, newCraft->downSignal, newCraft->upSignal);
					Serial.print(String(termColor("yellow")) + "----------------------------------------" + String(termColor("reset")) + "\n");
				}

				if (newCraft != nullptr) {
					// Add data to queue, to be passed to another task
					if (xQueueSend(queue, &newCraft, 100) == pdPASS) {
						if (config.debugUtils.showSerial == true) {
							Serial.print(String(termColor("green")) + "[+] " + String(newCraft->callsign) + " added to queue" + String(termColor("reset") + "\n\n"));
						}
					} else {
						// if (config.debugUtils.showSerial == true) {
						Serial.println(termColor("red"));
						Serial.print("Failed to add to queue");
						Serial.println(termColor("reset"));
						// }
					}
				}
			}
		}

		/* Increment target counters */
		targetCount++;
		if (targetCount > 9) {
			targetCount = 0;
			dishCount++;
		}
		if (dishCount > 9) {
			targetCount = 0;
			dishCount = 0;
			stationCount++;
		}
		if (stationCount > 2) {
			targetCount = 0;
			dishCount = 0;
			stationCount = 0;
		}
	} else {
		// There are no free items in the queue item pool
		Serial.println("No free items in queue item pool");
		return;
	}

	parseCounter++;
	return;
}

void fetchData()
{

	if (config.debugUtils.testCores == true) {
		Serial.print("fetchData() running on core ");
		Serial.println(xPortGetCoreID());
	}

	if (config.debugUtils.showSerial == true) {
		Serial.print("\n\n");
		Serial.print("   ┌─────────────────────────────┐\n");
		Serial.print("───│          FETCH DATA         │───\n");
		Serial.print("   └─────────────────────────────┘\n");
	}

	/* Check for full queue */
	if (uxQueueSpacesAvailable(queue) == 0) {
		if (config.debugUtils.showSerial == true)
			Serial.print(">>>   Queue is full, returning\n");
		return;
	}

	uint16_t httpResponseCode;

	if (forceDummyData == true) usingDummyData = true;

	if (forceDummyData == false) {
		/* Check WiFi connection status */
		if (WiFi.status() != WL_CONNECTED) {
			if (config.debugUtils.showSerial == true)
				Serial.println("WiFi Disconnected");

			usingDummyData = true;
		}

		if (usingDummyData == false) {

			try {
				memset(fetchUrl, 0, sizeof(fetchUrl));	 // set fetchUrl to empty
				strcpy(fetchUrl, config.wifiNetwork.serverName);			 // copy serverName to fetchUrl
				char randBuffer[9];						 // buffer for random number cache buster
				ltoa(random(999999999), randBuffer, 10); // convert random number to string
				strcat(fetchUrl, randBuffer);			 // append random number to fetchUrl
			}
			catch (...) {
				handleException();
			}

			if (config.debugUtils.showSerial == true) {
				// print fetchUrl
				Serial.print(termColor("purple"));
				Serial.print("fetchUrl: ");
				Serial.println(fetchUrl);

				Serial.println(termColor("reset"));
			}

			// Use WiFiClient class to create TCP connections
			if (!http.begin(fetchUrl)) {
				if (config.debugUtils.showSerial == true) {
					Serial.println("Failed to connect to server");
				}
				return;
			}

			// Send HTTP GET request
			http.setTimeout(10000);
			http.addHeader("Content-Type", "text/xml");
			httpResponseCode = http.GET();
		}

		if (config.debugUtils.showSerial == true) {
			Serial.print("Using dummy data? ");

			if (usingDummyData == true) {
				Serial.println("TRUE");
			} else {
				Serial.println("FALSE");
			}
		}
	}

	if (usingDummyData == true) {
		if (config.debugUtils.showSerial == true)
			Serial.print(String(termColor("red")) + "Using dummy xml data" + String(termColor("reset")) + "\n");

		parseData(dummyXmlData);
	}

	if (usingDummyData == false && noTargetFoundCounter > noTargetLimit) {
		Serial.print(String(termColor("red")) + "Target not found limit reach - using dummy xml data" + String(termColor("reset")) + "\n\n");
		usingDummyData = true;
		noTargetFoundCounter = 0;
		parseData(dummyXmlData);
	}

	if (usingDummyData == false && noTargetFoundCounter < noTargetLimit + 1) {
		if (httpResponseCode != 200) {
			Serial.print("HTTP Response: ");
			Serial.print(httpResponseCode);
			Serial.print(" - ");
			Serial.println(http.errorToString(httpResponseCode));
			noTargetFoundCounter++;

			// usingDummyData = true;
			// Serial.println("----------->>> Dummy XML <<<-------------");
			// try {
			// 	parseData(dummyXmlData);

			// } catch(...) {
			// 	Serial.println("nope!");
			// }
		} else {
			try {
				String res = http.getString();
				const char* charRes = res.c_str();

				if (config.debugUtils.showSerial == true)
					Serial.println("HTTP response received");
				usingDummyData = false;
				parseData(charRes);
			}
			catch (...) {
				handleException();
			}
		}
	}

	http.end(); // Free up resources
	if (dataStarted == false)
		dataStarted = true;

	return;
}

// Fetch XML data from HTTP & parse for use in animations
void getData(void* parameter)
{
	// UBaseType_t uxHighWaterMark;
	// uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	// Serial.print("high_water_mark:"); Serial.print(uxHighWaterMark); Serial.print("\t");

	for (;;) {
		try {
			// Send an HTTP POST request every 5 seconds
			if ((millis() - lastTime) > timerDelay) {
				// Serial.println("getData() running");
				if (uxQueueSpacesAvailable(queue) > 0) {
					// Serial.println("getData() queue space available");
					fetchData();
					lastTime = millis(); // Sync reference variable for timer
				}
			}
		}
		catch (...) {
			Serial.println("getData() exception");
			handleException();
		}
	}
}

#pragma endregion -- DATA FUNCTIONS



/* SETUP
* Runs once at startup */
void setup()
{
	Serial.begin(115200); // Begin serial communications, ESP32 uses 115200 rate
	WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP

	Serial.print("\n\n::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::::\n\n");
	Serial.println("  .  ..__. __..__.     ..__ .");
	Serial.println("  |\\ |[__](__ [__]     |[__)|");
	Serial.println("  | \\||  |.__)|  |  \\__||   |___");
	Serial.print("\n");
	Serial.println("   /$$      /$$ /$$           /$$ /$$$$$$$            /$$");
	Serial.println("  | $$$    /$$$|__/          |__/| $$__  $$          | $$");
	Serial.println("  | $$$$  /$$$$ /$$ /$$$$$$$  /$$| $$  \\ $$ /$$   /$$| $$  /$$$$$$$  /$$$$$$");
	Serial.println("  | $$ $$/$$ $$| $$| $$__  $$| $$| $$$$$$$/| $$  | $$| $$ /$$_____/ /$$__  $$");
	Serial.println("  | $$  $$$| $$| $$| $$  \\ $$| $$| $$____/ | $$  | $$| $$|  $$$$$$ | $$$$$$$$");
	Serial.println("  | $$\\  $ | $$| $$| $$  | $$| $$| $$      | $$  | $$| $$ \\____  $$| $$_____/");
	Serial.println("  | $$ \\/  | $$| $$| $$  | $$| $$| $$      |  $$$$$$/| $$ /$$$$$$$/|  $$$$$$$");
	Serial.println("  |__/     |__/|__/|__/  |__/|__/|__/       \\______/ |__/|_______/  \\_______/");
	Serial.print("\n");

	Serial.print("::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::::\n\n");
	Serial.print("Starting...\n\n");

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	// wm.resetSettings();

	if (config.debugUtils.showSerial == false) {
		Serial.setDebugOutput(false);
		wm.setDebugOutput(false);
	}

	if (config.debugUtils.testCores == true) {
		Serial.print("setup() running on core " + String(xPortGetCoreID()) + "\n\n");
	}

	// Make sure we can read the EEPROM
	if (SPIFFS.begin(true)) {
		Serial.println("SPIFFS filesystem mounted successfully");
		loadConfig();
		Serial.print("Config loaded\n\n");
	} else {
		Serial.println("An Error has occurred while mounting SPIFFS");
		Serial.println("Restarting...");
		delay(1000);
		ESP.restart();
		return;
	}

	// Set up pins
	pinMode(OUTER_PIN, OUTPUT);
	pinMode(INNER_PIN, OUTPUT);
	pinMode(MIDDLE_PIN, OUTPUT);
	pinMode(BOTTOM_PIN, OUTPUT);
	pinMode(OUTPUT_ENABLE, OUTPUT);
	pinMode(WIFI_RST, INPUT);
	pinMode(POTENTIOMETER, INPUT);
	digitalWrite(OUTPUT_ENABLE, HIGH);

	// Set initial LED brightness to half
	FastLED.setBrightness(127);
	// 	au.updateBrightness(); // Update brightness from potentiometer

	/* Fast LED Hardware Setup */
	FastLED.addLeds<NEOPIXEL, OUTER_PIN>(outer_leds, outerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, MIDDLE_PIN>(middle_leds, middlePixelsTotal);
	FastLED.addLeds<NEOPIXEL, INNER_PIN>(inner_leds, innerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, BOTTOM_PIN>(bottom_leds, bottomPixelsTotal);
	allStripsOff();

	/* Test strips by filling with different colors */
	if (config.debugUtils.testLEDs == true) {
		fill_solid(outer_leds, outerPixelsTotal, CRGB::Red);
		fill_solid(middle_leds, middlePixelsTotal, CRGB::Green);
		fill_solid(inner_leds, innerPixelsTotal, CRGB::Blue);
		FastLED.show();
		delay(1000);
		allStripsOff();

		for (int i = 0; i < 4; i++) {
			CRGB* strip = allStrips[i];

			for (int dot = 0; dot < outerPixelsTotal; dot++) {
				strip[dot] = CRGB::Purple;
				FastLED.show();
				// clear this led for the next time around the loop
				if (dot > 0) strip[dot - 1] = CRGB::Black;
				// delay(30);
			}
		}
		allStripsOff();
	}


	/* WIFI MANAGER SETUP */
	wm.setConfigPortalBlocking(false);

	const char* custom_radio_str = "<br/><label for='customfieldid'>Color Theme</label><br/><input type='radio' name='customfieldid' value='0' checked> White<br><input type='radio' name='customfieldid' value='1'> Cyber<br><input type='radio' name='customfieldid' value='2'> Valentine<br><input type='radio' name='customfieldid' value='3'> Moonlight";
	const char* meteor_decay_checkbox_str = "<br/><label for='meteorDecay'>Meteor Decay</label><br/><input type='checkbox' name='meteorDecay'>";
	const char* meteor_random_checkbox_str = "<br/><label for='meteorRandom'>Meteor Tail</label><br/><input type='checkbox' name='meteorRandom'>";
	const char* global_fps_str = "<br/><label for='globalFps'>Global FPS</label><br/><input type='number' name='globalFps' min='10' max='120' value='30'>";
	new (&field_color_theme) WiFiManagerParameter(custom_radio_str);				  // custom html input
	new (&field_meteor_tail_decay) WiFiManagerParameter(meteor_decay_checkbox_str);	  // custom html input
	new (&field_meteor_tail_random) WiFiManagerParameter(meteor_random_checkbox_str); // custom html input
	new (&field_global_fps) WiFiManagerParameter(global_fps_str);					  // custom html input

	wm.addParameter(&field_color_theme);
	wm.addParameter(&field_meteor_tail_decay);
	wm.addParameter(&field_meteor_tail_random);
	wm.addParameter(&field_global_fps);
	wm.setSaveParamsCallback(saveColorThemeCallback);
	std::vector<const char*> menu = { "wifi", "info", "param", "sep", "restart", "exit" };
	wm.setMenu(menu);

	// set dark theme
	wm.setClass("invert");

	wm.setCleanConnect(true);
	wm.setConnectRetries(5);
	wm.setConnectTimeout(10); // connect attempt fails after n seconds

	// wm.setSaveConnectTimeout(5);
	// wm.setConfigPortalTimeout(config.wifiNetwork.wmTimeout);

	// wm.startConfigPortal(config.wifiNetwork.apSSID, config.wifiNetwork.apPass);

	// Automatically connect using saved credentials,
	// if connection fails, it starts an access point with the specified name ( config.wifiNetwork.apSSID ),
	// if config.wifiNetwork.apPassWORD is empty it will be a non-protected AP
	// then goes into a blocking loop awaiting configuration and will return success result

	Serial.print("Existing WiFi credentials: ");
	if (wm.getWiFiIsSaved() == true) {
		Serial.print("True\nConnecting to WiFi...\n");
		bool res;
		res = wm.autoConnect(config.wifiNetwork.apSSID, config.wifiNetwork.apPass); // non-password protected ap

		if (!res) // Wifi connection failed
		{
			Serial.print(String(termColor("red")) + "Failed to connect to WiFi" + String(termColor("reset")));
			// ESP.restart();
			// wm.startConfigPortal(config.wifiNetwork.apSSID, config.wifiNetwork.apPass);
		} else // Wifi connection successful
		{
			Serial.print(String(termColor("green")) + "Connected to WiFi successfuly" + String(termColor("reset")));
		}
	} else {
		Serial.print("False\nStarting access portal for configuration at\n");
		wm.startConfigPortal(config.wifiNetwork.apSSID, config.wifiNetwork.apPass);
		wm.startWebPortal();
		portalRunning = true;
		wmStartTime = millis();
	}
	Serial.print("WiFi Status: " + String(wl_status_to_string(WiFi.status())) + "\n\n");

	/* DATA TASK SETUP */
	// Initialize the queue item pool
	freeListMutex = xSemaphoreCreateMutex();	  // Create the mutex
	xSemaphoreTake(freeListMutex, portMAX_DELAY); // Take the mutex before accessing shared data
	freeList[0] = &itemPool[0];
	freeList[1] = &itemPool[1];
	freeList[2] = &itemPool[2];
	freeList[3] = &itemPool[3];
	freeList[4] = &itemPool[4];
	xSemaphoreGive(freeListMutex); // Give the mutex back when done accessing shared data
	// Initialize the data task on the second core
	xTaskCreatePinnedToCore(
		getData,	  /* Function to implement the task */
		"getData",	  /* Name of the task */
		4096,		  /* Stack size in byts */
		NULL,		  /* Task input parameter */
		0,			  /* Priority of the task */
		&xHandleData, /* Task handle. */
		0);			  /* Core where the task should run */

	queue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create queue to pass data between tasks on separate cores

	// Check that queue was created successfully
	if (queue == NULL) {
		if (config.debugUtils.showSerial == true) {
			Serial.print(String(termColor("red")) + "Error creating the queue" + termColor("reset"));
			delay(3000);
		}
		ESP.restart();
	}

	http.setReuse(true);	   // Use persistent connection
	data.loadJson();		   // Load data from json file

	/* Assign config to global state variables */
	characterWidth = config.textTypography.characterWidth;

	setColorTheme(colorTheme); // Set color theme
	drawBottomPixels();		   // Draw initial bottom pixels
	delay(100);				   // Small delay to allow bottom pixels to draw
}


/* MAIN LOOP
 * Runs repeatedly as long as board is on
 */
void loop()
{
	if (config.debugUtils.testCores == true)
		if (millis() - lastTime > 4000 && millis() - lastTime < 4500)
			Serial.print("loop() running on core: " + String(xPortGetCoreID()));

	doWiFiManager();

	try {
		// Serial.println("check to see if time to get data...");
		if ((dataStarted == true && nameScrollDone == true && millis() - displayDurationTimer > displayMinDuration && millis() - craftDelayTimer > craftDelay)) {
			// Serial.println("time to get data...");
			if (config.debugUtils.showSerial == true) {
				Serial.print("\n\n");
				Serial.print("   ┌────────────────────────────────┐\n");
				Serial.print("───│          RETRIEVE QUEUE        │───\n");
				Serial.print("   └────────────────────────────────┘\n");
			}

			CraftQueueItem theInfoBuffer; // Create buffer to hold data from queue
			CraftQueueItem* infoBuffer = &theInfoBuffer;

			/* Print for debug */
			if (config.debugUtils.diagMeasure == true)
				Serial.print("Queue: " + String(uxQueueMessagesWaiting(queue)) + "\t");
			if (config.debugUtils.showSerial == true)
				printCurrentQueue(queue);


			/* Receive from queue (data task on core 1) */
			if (queue != nullptr && infoBuffer != nullptr) {
				if (xQueueReceive(queue, &infoBuffer, 1) == pdPASS) {
					if (config.debugUtils.showSerial == true) {
						// print received queue item
						Serial.print("-------- DATA RECEIVED from Queue ----------\n");
						printCraftInfo(0, infoBuffer->callsign, infoBuffer->name, infoBuffer->nameLength, infoBuffer->downSignal, infoBuffer->upSignal);
						Serial.print("--------------------------------------------\n\n");
					}

					/* Copy data from queue to buffer */
					try {
						if (infoBuffer->name != 0 && strlen(infoBuffer->name) > 0) {
							currentCraftBuffer.callsign = infoBuffer->callsign;
							currentCraftBuffer.name = infoBuffer->name;
							currentCraftBuffer.nameLength = infoBuffer->nameLength;
							currentCraftBuffer.downSignal = infoBuffer->downSignal;
							currentCraftBuffer.upSignal = infoBuffer->upSignal;

							nameScrollDone = false;

							/* Current Craft Buffer after copying */
							if (config.debugUtils.showSerial == true)
								printCurrentCraftBuffer();

						} else {
							if (config.debugUtils.showSerial == true)
								Serial.println("Craft name is empty");
							noTargetFoundCounter++;
						}
					}
					catch (...) {
						if (config.debugUtils.showSerial == true)
							Serial.println("Error copying data from queue to buffer");
						handleException();
					}

					/* Free the queue item */
					freeSempaphoreItem(infoBuffer);

					displayDurationTimer = millis(); // Sync reference variable for timer
				} else {
					if (config.debugUtils.showSerial == true) {
						Serial.print("\n" + String(termColor("red")) + "No data received" + String(termColor("reset")) + "\n");
					}
				}
			}
		}
	}
	catch (...) {
		Serial.println("Error in main loop");
		handleException();
	}

	try {
		updateAnimation(currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	}
	catch (...) {
		if (config.debugUtils.showSerial == true)
			Serial.print(String(termColor("red")) + "Error updating animation" + String(termColor("reset")) + "\n");
		handleException();
	}

	if (config.debugUtils.diagMeasure == true) {
		Serial.print("Duration:" + String((millis() - displayDurationTimer) / 1000) + "\t");
		Serial.print("Delay:" + String((millis() - craftDelayTimer) / 1000) + "\t");
		Serial.print("Queue:" + String(uxQueueMessagesWaiting(queue)) + "\t");
		Serial.print("FreeListTop:" + String(freeListTop) + "\t");
		Serial.print("stationCount:" + String(stationCount) + "\t");
		Serial.print("dishCount:" + String(dishCount) + "\t");
		Serial.print("targetCount:" + String(targetCount) + "\t");
		Serial.print("signalCount:" + String(signalCount) + "\t");
		Serial.print("parseCounter:" + String(parseCounter) + "\t");
		Serial.print("noTargetFoundCounter:" + String(noTargetFoundCounter) + "\t");
		Serial.print("animationTypeDown:" + String(animationTypeDown) + "\t");
		Serial.print("animationTypeUp:" + String(animationTypeUp) + "\t");

		// perfDiff = (millis() - perfTimer) * 10; // Multiplied by 10 for ease of visualization on plotter
		perfDiff = (millis() - perfTimer); // This is the actual value
		// UBaseType_t uxHighWaterMark;
		// uxHighWaterMark = uxTaskGetStackHighWaterMark(xHandleData);
		// Serial.print("high_water_mark:"); Serial.print(uxHighWaterMark); Serial.print("\t");
		printFreeHeap(); // Value might be being multiplied in printFreeHeap() function for ease of visualization on plotter
		Serial.print("PerfTimer:" + String(perfDiff) + "\t");
		Serial.println();
		parseCounter = 0;
		perfTimer = millis();
	}
}
