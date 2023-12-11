const char* currentFirmwareVersion = "1.0.2"; // Current firmware version

#pragma region -- LIBRARIES
#include <Arduino.h>		// Arduino core
#include <esp_task_wdt.h>	// ESP32 task watchdog
#include "soc/timer_group_struct.h" // Timer group struct
#include "soc/timer_group_reg.h"  // Timer group register
#include <FS.h>				// File system
#include <LittleFS.h>		// LittleFS file system
#include <HTTPClient.h>		// HTTP client
#include <HTTPUpdate.h> 	// HTTP update
#include <FastLED.h>		// FastLED lib
#include <tinyxml2.h>		// XML parser
#include <iostream>			// C++ I/O
#include <cstring>			// C++ string
#include <ArduinoJson.h>	// JSON parser
#include <algorithm>		// C++ algorithms
#include <WiFiManager.h>	// WiFi manager lib

/* Custom libs */
#include <DevUtils.h>		// Custom dev utils lib
#include <FileUtils.h>		// Custom file utilities lib
#include <MathHelpers.h>	// Custom math helpers lib
#include <TextCharacters.h> // Custom LED text lib
#include <AnimationUtils.h> // Custom animation utilities lib
#include <Animate.h>		// Custom animate lib

#include <SpacecraftData.h> // Custom spacecraft data lib


#pragma endregion -- END LIBRARIES

#pragma region -- NAMESPACES
using namespace tinyxml2; // XML parser
using namespace std;	  // C++ I/O
#pragma endregion thing

#pragma region-- ESP32 PIN ASSIGNMENTS
#define OUTER_PIN 17		// Outer ring pin
#define MIDDLE_PIN 18		// Middle ring pin
#define INNER_PIN 19		// Inner ring pin
#define BOTTOM_PIN 16		// Bottom ring pin
#define WIFI_RST 21			// WiFi reset pin
#define OUTPUT_ENABLE 22	// Output enable pin
#define POTENTIOMETER 32	// Brightness potentiometer pin
#pragma endregion


#pragma region -- CLASS INSTANTIATIONS
// Utils
// FileUtils fileUtils;			  // Instantiate file utils
DevUtils dev;					  // Instantiate term color


// Animations
AnimationUtils au(POTENTIOMETER); // Instantiate animation utils
AnimationUtils::Colors mpColors;  // Instantiate colors
Animate animate;				  // Instantiate animate
ColorTheme currentColors;		  // Instantiate LED color theme

// Tasks
TaskHandle_t xHandleData;		 // Task for fetching and parsing data
TaskStatus_t xHandleDataDetails; // Task details for HandleData
QueueHandle_t queue;			 // Queue to pass data between tasks

// Networking
WiFiManager wm;	 // Used for connecting to WiFi
HTTPClient http; // Used for fetching data
#pragma endregion

#define FORMAT_LITTLEFS_IF_FAILED true
#define MAX_XML_SIZE 20480

// Placeholder for XML data
// const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
// const char* dummyXmlData2 = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
// const char* dummyXmlData3 = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";

// Backup data
const char* data_Sept6 = PROGMEM R"==--==(<dsn>
<station name="gdscc" friendlyName="Goldstone" timeUTC="1694004780000" timeZoneOffset="-25200000"/>
<dish name="DSS24" azimuthAngle="157" elevationAngle="79" windSpeed="1" isMSPA="false" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="8.500e+06" frequency="0" band="X" power="-4.7e+02" spacecraft="KPLO" spacecraftID="-155"/>
<downSignal active="false" signalType="none" dataRate="0.000e+00" frequency="0" band="S" power="-1.5e+02" spacecraft="KPLO" spacecraftID="-155"/>
<target name="KPLO" id="155" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS25" azimuthAngle="84" elevationAngle="22" windSpeed="1" isMSPA="false" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="3.205e+04" frequency="0" band="X" power="-1.3e+02" spacecraft="LFL" spacecraftID="-164"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="1.8e+01" spacecraft="LFL" spacecraftID="-164"/>
<target name="LFL" id="164" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS26" azimuthAngle="156" elevationAngle="79" windSpeed="1" isMSPA="false" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="2.0e-01" spacecraft="KPLO" spacecraftID="-155"/>
<downSignal active="true" signalType="data" dataRate="8.192e+03" frequency="0" band="S" power="-1.1e+02" spacecraft="KPLO" spacecraftID="-155"/>
<downSignal active="false" signalType="none" dataRate="8.500e+06" frequency="0" band="X" power="-9.0e+01" spacecraft="KPLO" spacecraftID="-155"/>
<target name="KPLO" id="155" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS14" azimuthAngle="224" elevationAngle="64" windSpeed="0" isMSPA="false" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="2.000e+05" frequency="0" band="X" power="-1.3e+02" spacecraft="JNO" spacecraftID="-61"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="1.8e+01" spacecraft="JNO" spacecraftID="-61"/>
<target name="JNO" id="61" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<station name="mdscc" friendlyName="Madrid" timeUTC="1694004780000" timeZoneOffset="7200000"/>
<dish name="DSS65" azimuthAngle="239" elevationAngle="51" windSpeed="9" isMSPA="false" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="2.5e-01" spacecraft="WIND" spacecraftID="-8"/>
<downSignal active="true" signalType="data" dataRate="6.400e+03" frequency="0" band="S" power="-1.3e+02" spacecraft="WIND" spacecraftID="-8"/>
<downSignal active="true" signalType="data" dataRate="7.351e+04" frequency="0" band="S" power="-1.2e+02" spacecraft="WIND" spacecraftID="-8"/>
<target name="WIND" id="8" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS53" azimuthAngle="163" elevationAngle="46" windSpeed="9" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="1.163e+01" frequency="0" band="X" power="-1.6e+02" spacecraft="MVN" spacecraftID="-202"/>
<downSignal active="false" signalType="none" dataRate="1.422e+04" frequency="0" band="X" power="-4.8e+02" spacecraft="M01O" spacecraftID="-53"/>
<downSignal active="true" signalType="data" dataRate="1.000e+06" frequency="0" band="X" power="-1.2e+02" spacecraft="MRO" spacecraftID="-74"/>
<downSignal active="false" signalType="none" dataRate="0.000e+00" frequency="0" band="X" power="-4.8e+02" spacecraft="M20" spacecraftID="-168"/>
<target name="M01O" id="53" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
<target name="MVN" id="202" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
<target name="M20" id="168" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
<target name="MRO" id="74" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS54" azimuthAngle="199" elevationAngle="50" windSpeed="9" isMSPA="false" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="1.042e+03" frequency="0" band="X" power="-1.5e+02" spacecraft="SPP" spacecraftID="-96"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="1.8e+01" spacecraft="SPP" spacecraftID="-96"/>
<target name="SPP" id="96" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<dish name="DSS55" azimuthAngle="170" elevationAngle="49" windSpeed="9" isMSPA="false" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="5.0e+00" spacecraft="LUCY" spacecraftID="-49"/>
<downSignal active="true" signalType="data" dataRate="6.250e+04" frequency="0" band="X" power="-1.3e+02" spacecraft="LUCY" spacecraftID="-49"/>
<target name="LUCY" id="49" uplegRange="-1.00e+00" downlegRange="-1.00e+00" rtlt="-1.000"/>
</dish>
<station name="cdscc" friendlyName="Canberra" timeUTC="1694004780000" timeZoneOffset="36000000"/>
<timestamp>1694004780000</timestamp>
</dsn>)==--==";

// Test dummy data that cycles through all rate classes
const char* data_animation_test = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="TEST" spacecraftID="-170" />
        <upSignal dataRate="0" frequency="2090" power="4.804" signalType="data" spacecraft="TEST" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="TEST" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="Rate1" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
    <dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556">
        <downSignal dataRate="6.400e+03" frequency="8439000000" power="-138.1801" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <upSignal dataRate="6.400e+03" frequency="7183" power="0.0000" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <target downlegRange="8.207e+07" id="74" name="Rate2" rtlt="547.5" uplegRange="8.207e+07" />
    </dish>
    <station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" />
    <dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556">
        <downSignal dataRate="5.000e+04" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <upSignal dataRate="5.000e+04" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <target downlegRange="1.417e+05" id="151" name="Rate3" rtlt="0.9455" uplegRange="1.417e+05" />
    </dish>
    <dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556">
        <downSignal dataRate="2.458e+05" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <upSignal dataRate="2.458e+05" frequency="2098" power="0.2630" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <target downlegRange="1.389e+06" id="92" name="Rate4" rtlt="9.266" uplegRange="1.389e+06" />
    </dish>
    <station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" />
    <dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556">
        <downSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <upSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <target downlegRange="4.099e+06" id="210" name="Rate5" rtlt="27.34" uplegRange="4.099e+06" />
    </dish>
    <dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556">
        <downSignal dataRate="2.621e+06" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <upSignal dataRate="2.621e+06" frequency="2066" power="1.758" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <target downlegRange="1.331e+06" id="21" name="Rate6" rtlt="8.882" uplegRange="1.331e+06" />
    </dish>
    <timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test dummy data that cycles through specific rate classes
const char* data_animation_test_single = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
    <dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556">
        <downSignal dataRate="2.458e+05" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <upSignal dataRate="2.458e+05" frequency="2098" power="0.2630" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <target downlegRange="1.389e+06" id="92" name="Rate4" rtlt="9.266" uplegRange="1.389e+06" />
    </dish>
	<dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556">
        <downSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <upSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <target downlegRange="4.099e+06" id="210" name="Rate5" rtlt="27.34" uplegRange="4.099e+06" />
    </dish>
    <dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556">
        <downSignal dataRate="2.621e+06" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <upSignal dataRate="2.621e+06" frequency="2066" power="1.758" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <target downlegRange="1.331e+06" id="21" name="Rate6" rtlt="8.882" uplegRange="1.331e+06" />
    </dish>
    <timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test font by displaying all alphanumberic characters
const char* data_font_test = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS2" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="abcdefghijklmnopqrstuvwxyz 0123456789" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS2" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="abcdefghijklmnopqrstuvwxyz 0123456789" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test DSN Maintenance mode XML
const char* data_dsn_maintenance = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
	<station name="gdscc" friendlyName="Goldstone" timeUTC="1698785318000" timeZoneOffset="-25200000"/>
	<station name="mdscc" friendlyName="Madrid" timeUTC="1698785318000" timeZoneOffset="3600000"/>
	<station name="cdscc" friendlyName="Canberra" timeUTC="1698785318000" timeZoneOffset="39600000"/>
	<timestamp>1698785318000</timestamp>
</dsn>)==--==";


// Set the data to use
const char* dummyXmlData = data_Sept6;


/* STATE TRACKING
* Sets up variables to track state of the program
*/

#pragma region -- DATA FETCHING STATE VARS
bool otaUpdateTriggered = false; // Flag to indicate OTA update has been triggered
bool usingDummyData = false; // If true, use dummy data instead of actual data
bool forceDummyData = false;
uint8_t noTargetFoundCounter = 0;  // Keeps track of how many times target is not found
uint8_t noTargetLimit = 3;		   // After target is not found this many times, switch to dummy XML data
uint8_t retryDataFetchCounter = 0; // Keeps track of how many times data fetch failed
uint8_t retryDataFetchLimit = 3;  // After dummy data is used this many times, try to get actual data again
bool dataStarted = false;
// char fetchUrl[64];	// Fixed memory for DSN XML fetch URL - random number is appended when used to prevent caching

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
static int freeListTop = MAX_ITEMS - 1; // Points to the top of the free list
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

// WiFiManagerParameter field_color_theme;		   // global param ( for non blocking w params )
// WiFiManagerParameter field_xml_data;
WiFiManagerParameter param_brightness;		   // global param ( for non blocking w params )
// WiFiManagerParameter field_force_dummy_data;		   // global param ( for non blocking w params )
// WiFiManagerParameter field_meteor_tail_decay;  // global param ( for non blocking w params )
// WiFiManagerParameter field_meteor_tail_random; // global param ( for non blocking w params )
// WiFiManagerParameter field_global_fps;
WiFiManagerParameter param_force_dummy_data;
WiFiManagerParameter param_show_serial;
WiFiManagerParameter param_show_diagnostics;
WiFiManagerParameter param_force_animation_type;
// WiFiManagerParameter param_update_firmware;
// WiFiManagerParameter field_scroll_letters_delay;
// WiFiManagerParameter field_show_serial("show_serial", "Show Serial", FileUtils::config.debugUtils.showSerial, 1);
bool portalRunning = false;
// struct wmParams
// {
// 	String customfieldid;
// 	String xml_data_radio;
// 	String brightness;
// 	String show_serial;
// 	String show_diagnostics;
// 	String scroll_letters_delay;
// };




/* WIFI MANAGER CUSTOM FIELD PARAMETERS */
// const char* color_theme_str = "<br/><label for='customfieldid'>Color Theme</label><br/><input type='radio' name='customfieldid' value='0' checked> White<br><input type='radio' name='customfieldid' value='1'> Cyber<br><input type='radio' name='customfieldid' value='2'> Valentine<br><input type='radio' name='customfieldid' value='3'> Moonlight";
// const char* xml_data_radio_str = "<br/><label for='xml_data_radio'>XML Data</label><br/><input type='radio' name='xml_data_radio' value='0' checked> Live<br><input type='radio' name='xml_data_radio' value='1'> Dummy<input type='radio' name='xml_data_radio' value='2'> Animation Test";
// const char* global_brightness_str = "<br/><label for='global_brightness'>Global Brightness</label><br/><input type='number' name='global_brightness' min='1' max='160' value='160'>";
// const char* scroll_letters_delay_str = "<br/><label for='scroll_letters_delay'>Scroll Letter Delay (ms)</label><br/><input type='number' name='scroll_letters_delay' min='0' max='300' value='33'>";

// const char* meteor_decay_checkbox_str = "<br/><label for='meteorDecay'>Meteor Decay</label><br/><input type='checkbox' name='meteorDecay'>";
// const char* meteor_random_checkbox_str = "<br/><label for='meteorRandom'>Meteor Tail</label><br/><input type='checkbox' name='meteorRandom'>";
// const char* global_fps_str = "<br/><label for='globalFps'>Global FPS</label><br/><input type='number' name='globalFps' min='10' max='120' value='30'>";
// const char* force_dummy_data_str = "<br/><p>Force Dummy Data: " + String(forceDummyData) + "</p><br/>";


// char show_serial_str_buffer[128];
// snprintf(show_serial_str_buffer, sizeof(show_serial_str_buffer), "<br/><label for 'show_serial'>Show Serial</label><br/><input type='checkbox' name='show_serial'%s>",
// FileUtils::config.debugUtils.showSerial ? " checked" : "");


// // new (&field_color_theme) WiFiManagerParameter(color_theme_str);				  // custom html input
// new (&field_xml_data) WiFiManagerParameter(xml_data_radio_str);				  	 // custom html input
// new (&field_global_brightness) WiFiManagerParameter(global_brightness_str);		 // custom html input
// new (&field_scroll_letters_delay) WiFiManagerParameter(scroll_letters_delay_str); // custom html input
// // new (&field_force_dummy_data) WiFiManagerParameter(force_dummy_data_str);		 // custom html input
// // new (&field_meteor_tail_decay) WiFiManagerParameter(meteor_decay_checkbox_str);	  // custom html input
// // new (&field_meteor_tail_random) WiFiManagerParameter(meteor_random_checkbox_str); // custom html input
// new (&field_global_fps) WiFiManagerParameter(global_fps_str);					  // custom html input
// // new (&field_show_serial) WiFiManagerParameter(show_serial_str);					 // custom html input
// new (&field_show_serial) WiFiManagerParameter(show_serial_str_buffer);
// // wm.addParameter(&show_serial_param);


// // wm.addParameter(&field_color_theme);
// wm.addParameter(&field_xml_data);
// wm.addParameter(&field_global_brightness);
// wm.addParameter(&field_scroll_letters_delay);
// // wm.addParameter(&field_force_dummy_data);
// // wm.addParameter(&field_meteor_tail_decay);
// // wm.addParameter(&field_meteor_tail_random);
// wm.addParameter(&field_global_fps);
// wm.addParameter(&field_show_serial);



#pragma endregion -- END WIFIMANAGER PORTAL

#pragma region -- TIMERS
/* Time is measured in milliseconds, so long is used */
uint8_t fpsRate = 60;
unsigned long fpsTimer = 0;
unsigned long perfTimer = 0;
unsigned long perfDiff = 0;
unsigned long lastTime = 0; // Init reference variable for timer
bool firstStartupAnimation = true;
const uint16_t timerDelay = 10000; // Timer for how often to fetch data
unsigned long animationTimer = 0;
unsigned long craftDelayTimer = 0;
const uint16_t craftDelay = 8000; // Wait this long after finishing for new craft to be dipslayed
const uint16_t displayMinDuration = 20000; // Minimum time to display a craft before switching to next craft
unsigned long displayDurationTimer = 20000; // Timer to keep track of how long craft has been displayed, set at minimum for no startup delay
const uint16_t textMeteorGap = 6000;
const uint16_t animationRepeatGap = 3000;
unsigned long animationCleanupTimer = 0;
const uint16_t animationCleanupDelay = 8000;
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
static  int characterHeight = 9;
static int characterKerning = 4;
static int letterSpacing = 9;

#pragma endregion -- END TEXT SETTINGS


/* FUNCTIONS
* Doing things
*/

void update_started() {
	Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
	Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
	Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
	Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void setupOtaUpdate() {
	Serial.println("Setting up OTA update...");
	httpUpdate.onStart(update_started);
	httpUpdate.onEnd(update_finished);
	httpUpdate.onProgress(update_progress);
	httpUpdate.onError(update_error);
}

const char* getRemoteFirmwareVersion() {
	bool showSerial = FileUtils::config.debugUtils.showSerial;

	static char buffer[16];
	HTTPClient httpFirmware;
	httpFirmware.begin("http://develop.kellerdigital.com/minipulse/latest_version.txt");
	int httpCode = httpFirmware.GET();
	if (showSerial) Serial.print("HTTP code: " + String(httpCode) + "\n");

	if (httpCode == HTTP_CODE_OK) {
		String latestVersion = httpFirmware.getString();
		if (showSerial) Serial.println("Remote version: " + latestVersion);
		strncpy(buffer, latestVersion.c_str(), sizeof(buffer) - 1);
	} else {
		Serial.println("Failed to fetch remote version");
		buffer[0] = '\0';
	}

	httpFirmware.end();
	return buffer;
}

bool checkFirmwareUpdateAvailable() {
	// char buffer[2048];
	// int offset = 0;

	const char* latestVersion = getRemoteFirmwareVersion();

	// offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Remote version: %s\n", latestVersion);

	if (strcmp(latestVersion, "") == 0) {
		// offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Unable to fetch latest version from remote server\n");
		// Serial.print(buffer);
		return false;
	}

	if (strcmp(latestVersion, currentFirmwareVersion) != 0) {
		// offset += snprintf(buffer + offset, sizeof(buffer) - offset, "New firmware available!\n");
		// Serial.print(buffer);
		return true;
	} else {
		// offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Firmware is up to date\n");
	}

	// Serial.print(buffer);
	return false;
}

void updateFirmwareOta() {
	if (!checkFirmwareUpdateAvailable()) return;

	char buffer[2048];
	int offset = 0;
	offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\n--------\nAttempting to update firmware from remote server...\n");


	WiFiClient client;
	t_httpUpdate_return ret = httpUpdate.update(client, "http://develop.kellerdigital.com/minipulse/firmware.bin");
	// Or:
	//t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "/file.bin");

	switch (ret) {
		case HTTP_UPDATE_FAILED:
			offset += snprintf(buffer + offset, sizeof(buffer) - offset, ">> HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			offset += snprintf(buffer + offset, sizeof(buffer) - offset, ">> HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			offset += snprintf(buffer + offset, sizeof(buffer) - offset, ">> HTTP_UPDATE_OK");
			break;

		default:
			offset += snprintf(buffer + offset, sizeof(buffer) - offset, ">> HTTP_UPDATE Unknown error");
			break;
	}

	offset += snprintf(buffer + offset, sizeof(buffer) - offset, "OTA update complete\n--------\n\n");
	Serial.print(buffer);
}

#pragma region -- ANIMATION UTILITIES

uint8_t fpsInMs = 1000 / fpsRate;
bool meteorTailDecay = true;
bool meteorTailRandom = true;

// State counters to enable fractional animation speeds
// These all start at 1 and get incremented during animation update
uint8_t counterHalfSpeed = 1;
uint8_t counterThirdSpeed = 1;
uint8_t counterQuarterSpeed = 1;
uint8_t counterEighthSpeed = 1;

// Keeps track of current animation state
static bool nameScrollDone = true;
static bool animationInProgress = false;
static bool animationTypeSetDown = false;
static bool animationTypeSetUp = false;
static uint8_t animationTypeDown = 0;
static uint8_t animationTypeUp = 0;
bool animateFirstCycleDown = true;
bool animateFirstCycleUp = true;

#pragma endregion -- END ANIMATION UTILITIES

#pragma region -- DEV UTILITIES

void feedWatchdog() {
	// // Timer Group 0 reset
	// TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	// TIMERG0.wdt_feed = 1;
	// TIMERG0.wdt_wprotect = 0;

	// // Timer Group 1 reset
	// TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	// TIMERG1.wdt_feed = 1;
	// TIMERG1.wdt_wprotect = 0;

	esp_task_wdt_reset();
}


void printMeteorArray()
{

	String printString = "\n---------------[ METEOR ARRAY ]---------------\n";

	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] == nullptr) {
			printString += "[";
			printString += DevUtils::termColor("red");
			printString += i;
			printString += " = nul";
			printString += "]";
		} else {
			printString += "[";
			printString += DevUtils::termColor("green");
			printString += i;
			printString += " = ";
			// printString += animate.ActiveMeteors[i]->firstPixel;
			printString += animate.ActiveMeteors[i]->animationType;
			printString += "]";
		}
		printString += DevUtils::termColor("reset");

		if (i != 0 && i % 10 == 0) {
			printString += "\n";
		}
	}

	printString += "\n------------------------------\n";

	Serial.println(printString);
}

void printCraftInfo(uint listPosition, const char* callsign, const char* name, uint nameLength, uint downSignal, uint upSignal)
{
	char buffer[256];
	// Serial.print()
	if ( callsign == nullptr || name == nullptr) {
		snprintf(
			buffer,
			sizeof(buffer),
			"ITEM #%u: Invalid\n",
			listPosition
		);
		return;
    }

	int written = snprintf(
		buffer,
		sizeof(buffer),
		"ITEM #%u: (%s) %s [%u] [↓ Sig Dn: %u] [↑ Sig Up: %u]\n",
		listPosition,
		callsign,
		name,
		nameLength,
		downSignal,
		upSignal
	);
	if (written > 0 && written < sizeof(buffer)) {
		Serial.print(buffer);
	} else {
		Serial.print("ITEM #E: Invalid\n");
	}
}

String returnCraftInfo(uint listPosition, const char* callsign, const char* name, uint nameLength, uint downSignal, uint upSignal)
{
	if (callsign == nullptr) {
		callsign = "NULL";
	}
	if (name == nullptr) {
		name = "NULL";
	}

	char buffer[256];
	snprintf(buffer, sizeof(buffer), "ITEM #%u: (%s) %s [%u] [↓ Sig Dn: %u] [↑ Sig Up: %u]\n",
		listPosition, callsign, name, nameLength, downSignal, upSignal);
	return String(buffer);
}

void printCurrentQueue(QueueHandle_t queue) {
	char buffer[1024]; // Create a buffer to hold the serial output
	snprintf(
		buffer,
		sizeof(buffer),
		"\n%s========= QUEUE %d/%d ==========%s\n",
		dev.termColor("purple"),
		uxQueueMessagesWaiting(queue),
		MAX_ITEMS,
		dev.termColor("reset")
	);

	if (uxQueueMessagesWaiting(queue) > 0) {
		QueueHandle_t tempQueue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create a temporary queue to hold the items
		if (tempQueue == NULL) {
			Serial.println("Error: Could not create temporary queue.");
			return;
		}



		CraftQueueItem theTempBuffer;
		CraftQueueItem* tempBuffer = &theTempBuffer;

		int queueCount = uxQueueMessagesWaiting(queue);
		for (int i = 0; i < queueCount; i++) {
			if (xQueueReceive(queue, &tempBuffer, (TickType_t)10)) {
				String craftInfo = returnCraftInfo(i, tempBuffer->callsign, tempBuffer->name, tempBuffer->nameLength, tempBuffer->downSignal, tempBuffer->upSignal);
				strncat(buffer, craftInfo.c_str(), sizeof(buffer) - strlen(buffer) - 1);

				if (!xQueueSend(tempQueue, &tempBuffer, (TickType_t)10)) {
					Serial.println("Error: Could not send data to the temporary queue.");
					// Handle error as necessary, maybe break out of the loop
				}
			} else {
				Serial.println("Error: Could not receive data from the main queue.");
				break;
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
	} else {
		strncat(buffer, "Queue is empty.\n", sizeof(buffer) - strlen(buffer) - 1);
	}

	snprintf(buffer, sizeof(buffer), "%s%s===============================%s\n\n", buffer, dev.termColor("purple"), dev.termColor("reset"));
	Serial.print(buffer);
}

void printCurrentCraftBuffer() {
	Serial.print(
		DevUtils::termColor("blue") +
		"============= CURRENT DATA BUFFER =============" + DevUtils::termColor("reset") +
		"\n");
	printCraftInfo(0, currentCraftBuffer.callsign, currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	Serial.print(
		DevUtils::termColor("blue") + "===============================================" + DevUtils::termColor("reset") + "\n\n");
}

void printSemaphoreList() {
	// This function must be used inside a mutex lock
	if (FileUtils::config.debugUtils.showSerial) {
		Serial.print(
			"\n=== freeListMutex [" +
			String(freeListTop + 1) +
			"/" +
			String(MAX_ITEMS) +
			"] ===\n");

		for (int i = 0; i < MAX_ITEMS; i++) {
			if (freeList[i] == nullptr) continue; // Skip null items

			String marker = (i == freeListTop) ? ">" : " "; // Marker for top of freeList
			Serial.print(
				marker +
				"[" +
				String(i) +
				"] " +
				String(freeList[i]->callsignArray) +
				"\n");
		}
		Serial.print("\n");
	}
}

void freeSemaphoreItem(CraftQueueItem* infoBuffer) {
	feedWatchdog();
	if (xSemaphoreTake(freeListMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

		if (freeListTop < 0) {
			Serial.println("Error: freeListTop is " + String(freeListTop) + "\n");
			freeListTop = 0;
		}

		if (freeListTop > MAX_ITEMS - 1) {
			Serial.println("Error: freeListTop is greater than MAX_ITEMS");
			freeListTop = MAX_ITEMS - 1;
		}

		freeList[freeListTop] = infoBuffer;
		freeListTop++;


		char semaphoreStatusMessage[128];
		snprintf(
			semaphoreStatusMessage,
			sizeof(semaphoreStatusMessage),
			"%sSemaphore freed: %s [%d/%d]%s\n",
			DevUtils::termColor("green"),
			String(infoBuffer->callsignArray),
			freeListTop,
			MAX_ITEMS,
			DevUtils::termColor("reset")
		);
		infoBuffer = nullptr;
		xSemaphoreGive(freeListMutex);
	}
}

#pragma endregion -- DEV UTILITIES

#pragma region -- WIFIMANAGER HANDLING

void doWiFiManager()
{
	try {
		bool portalActive = wm.getConfigPortalActive();
		// Process Wifi Manager portal
		if (portalActive) {
			wm.process(); // do processing
		} else {
			Serial.print("\n\n----------STARTING CONFIG PORTAL\n\n");
			wm.setConfigPortalBlocking(false);
			wm.startConfigPortal(FileUtils::config.wifiNetwork.apSSID, FileUtils::config.wifiNetwork.apPass);
			portalRunning = true;
		}


		// Force Wifi portal when WiFi reset button is pressed
		if (digitalRead(WIFI_RST) == LOW) {
			if (!portalRunning) {
				Serial.println("Button Pressed: Starting Config Portal");
				wm.setConfigPortalBlocking(false);
				wm.startConfigPortal(FileUtils::config.wifiNetwork.apSSID, FileUtils::config.wifiNetwork.apPass);
				portalRunning = true;
			} else {
				char buffer[256];
				snprintf(buffer, sizeof(buffer), "Button Pressed: Config Portal already running\n"
					"Connect to the %s WiFi network\n"
					"Then go to http://%s in your web browser",
					wm.getConfigPortalSSID(),
					WiFi.softAPIP().toString());
			}
		}
	}
	catch (...) {
		Serial.println("Error: doWiFiManager() failed");
		dev.handleException();
	}
}

void configPortalCallback() {
	Serial.print(
		"\n" +
		DevUtils::termColor("green") +
		"[CALLBACK] configPortalCallback fired" +
		DevUtils::termColor("reset") + "\n\n");
	portalRunning = true;
}

void webServerCallback() {
	Serial.print("\n" + DevUtils::termColor("blue") + "Web server started" + DevUtils::termColor("reset") + "\n\n");
	portalRunning = true;

	wm.server->on("/get-remote-version-number", HTTP_GET, []() {
		const char* remoteFirmwareVersion = getRemoteFirmwareVersion();
		String response = "Remote Version: " + String(remoteFirmwareVersion);

		wm.server->send(200, "text/plain", response);
		});

	wm.server->on("/get-latest-version-number", HTTP_GET, []() {
		const char* remoteFirmwareVersion = getRemoteFirmwareVersion();
		bool updateAvailable = checkFirmwareUpdateAvailable();
		const char* updateAvailableText = updateAvailable ? "true" : "false";
		String response = "Current version: " + String(currentFirmwareVersion) + "<br>Remote Version: " + String(remoteFirmwareVersion) + "<br>Update available: " + String(updateAvailableText);

		wm.server->send(200, "text/plain", response);

		});

	wm.server->on("/trigger-firmware-update", HTTP_GET, []() {
		bool updateAvailable = checkFirmwareUpdateAvailable();
		updateFirmwareOta();
		if (updateAvailable) {
			wm.server->send(200, "text/plain", "New firmware available, updating...");
		} else {
			wm.server->send(200, "text/plain", "Firmware is up to date");
		}
		});
}

void setColorTheme(uint8_t colorTheme)
{
	switch (colorTheme) {
		case 0:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 0;
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

void setXmlData(uint8_t selection) {
	switch (selection) {
		case 0:
			forceDummyData = false;
			break;
		case 1:
			forceDummyData = true;
			dummyXmlData = data_Sept6;
			break;
		case 2:
			forceDummyData = true;
			dummyXmlData = data_animation_test;
			break;

	}
}

void setGlobalBrightness(uint8_t newBrightness)
{
	FastLED.setBrightness(newBrightness);
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

void saveParamsCallback() {
	Serial.print("\n\n<--------- PORTAL FORM SUBMITTED --------->\n\n");

	/* SHOW SERIAL - Set serial show config key from input */
	// Get input value
	String showSerialValue = getParam("show_serial");
	Serial.print("---\nshow_serial input: " + showSerialValue + "\n");

	// Convert to bool
	bool showSerialValueBool = strcmp(showSerialValue.c_str(), "1") == 0 ? true : false;
	Serial.print("show serial bool: " + String(showSerialValueBool) + "\n");

	// Set program config
	Serial.print("Previous config showSerial: " + String(FileUtils::config.debugUtils.showSerial) + "\n");
	FileUtils::config.debugUtils.showSerial = showSerialValueBool;
	Serial.print("New config showSerial: " + String(FileUtils::config.debugUtils.showSerial) + "\n");

	// wm.setDebugOutput(showSerialValueBool); // Turn on/off WiFiManager debug output

	// Set file config
	FileUtils::writeConfigFileBool("showSerial", FileUtils::config.debugUtils.showSerial);
	FileUtils::readFile("/config.json");
	Serial.println();


	/* SHOW DIAGNOSTICS - Set show diagnostics config key from input */
	// Get input value
	String showDiagnosticsValue = getParam("show_diagnostics");
	Serial.print("---\nshow_diagnostics input: " + showDiagnosticsValue + "\n");

	// Convert to bool
	bool showDiagnosticsValueBool = strcmp(showDiagnosticsValue.c_str(), "1") == 0 ? true : false;
	Serial.print("show diagnostics bool: " + String(showDiagnosticsValueBool) + "\n");

	// Set program config
	Serial.print("Previous config showDiagnostics: " + String(FileUtils::config.debugUtils.diagMeasure) + "\n");
	FileUtils::config.debugUtils.diagMeasure = showDiagnosticsValueBool;
	Serial.print("New config showDiagnostics: " + String(FileUtils::config.debugUtils.diagMeasure) + "\n");

	// Set file config
	FileUtils::writeConfigFileBool("diagMeasure", FileUtils::config.debugUtils.diagMeasure);
	FileUtils::readFile("/config.json");
	Serial.println();


	/* FORCE ANIMATION TYPE - Set forced animaiton type from input */
	// Get input value
	String forcedAnimationTypeValue = getParam("force_animation_type");
	Serial.print("---\nforced_animation_type input: " + forcedAnimationTypeValue + "\n");

	// Set program variable
	Serial.println("Previous forcedAnimationType: " + String(animate.forcedAnimationType));
	animate.forcedAnimationType = atoi(forcedAnimationTypeValue.c_str());
	Serial.println("New forcedAnimationType: " + String(animate.forcedAnimationType));


	/* BRIGHTNESS - Set brightness config key from input */
	// Get input value
	String brightnessValue = getParam("brightness");
	Serial.print("---\nbrightness input: " + brightnessValue + "\n");

	// Convert to int
	int brightnessInt = atoi(brightnessValue.c_str());

	// Map brightness
	int brightnessMapped = MathHelpers::map(brightnessInt, 0, 100, 8, 160);

	// Set program config
	Serial.print("Previous config brightness: " + String(FileUtils::config.displayLED.brightness) + "\n");
	FileUtils::config.displayLED.brightness = brightnessInt;
	Serial.print("New config brightness: " + String(FileUtils::config.displayLED.brightness) + "\n");

	// Set file conf                                                                                                                         ConfigFileInt("brightness", FileUtils::config.displayLED.brightness);
	FileUtils::writeConfigFileInt("brightness", FileUtils::config.displayLED.brightness);
	FileUtils::readFile("/config.json");
	Serial.print("\n");

	// Set LEDs brightness	
	setGlobalBrightness(FileUtils::config.displayLED.brightness);


	/* FORCE DUMMY DATA - Set forceDummyData config key from input */
	// Get input value
	String forceDummyDataInputValue = getParam("force_dummy_data");
	Serial.print("---\nforceDummyData input: " + forceDummyDataInputValue + "\n");

	// Convert to int
	int forceDummyDataInt = atoi(forceDummyDataInputValue.c_str());

	// Set program config
	Serial.print("Previous config forceDummyData: " + String(FileUtils::config.wifiNetwork.forceDummyData) + "\n");
	FileUtils::config.wifiNetwork.forceDummyData = forceDummyDataInt;
	Serial.print("New config forceDummyData: " + String(FileUtils::config.wifiNetwork.forceDummyData) + "\n");

	// Set file config
	FileUtils::writeConfigFileBool("forceDummyData", FileUtils::config.wifiNetwork.forceDummyData);
	FileUtils::readFile("/config.json");
	Serial.println();

	// Set forceDummyData
	forceDummyData = FileUtils::config.wifiNetwork.forceDummyData;


	Serial.print("\n<--------- END PORTAL FORM CALLBACK --------->\n\n");
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
	counterEighthSpeed == 8 ? counterEighthSpeed = 1 : counterEighthSpeed++;
}

// Utility function to reverse elements of an array
void reverseStripsArray(void)
{
	reverse(allStrips, allStrips + 4);
}

// Utility to wrap meteor region if its out of bounds
uint8_t regionWrap(int8_t region, bool isDown)
{
	if (isDown) {
		if (region == outerChunks) return  0; // Wrap around to first strip, 1st pixel
		if (region == outerChunks + 1) return  1; // Wrap around to first strip, 2nd pixel
		if (region < 0) return outerChunks - 1; // Wrap around to last strip
	} else {
		if (region == middleChunks) return  0; // Wrap around to first strip, 1st pixel
		if (region == middleChunks + 1) return  1; // Wrap around to first strip, 2nd pixel
		if (region < 0) return middleChunks - 1; // Wrap around to last strip
	}

	return region;
}

// Display letter from array
void doLetterRegions(char theLetter, int regionStart, int startingPixel)
{
	const TextCharacter::TextCharacterInfo ledCharacter = textCharacter.getCharacter(theLetter, characterWidth);

	// Check if characterWidth is zero, if so, return from the function to avoid division by zero
	if (characterWidth == 0) {
		characterWidth = 5;
		Serial.println("Error: characterWidth is zero - setting to 5");
		// return;
	}


	const uint16_t regionOffset = innerPixelsChunkLength * regionStart;

	int16_t pixel = startingPixel + regionOffset;
	const int16_t previousPixel = pixel - letterSpacing;

	const size_t inner_leds_size = sizeof(inner_leds) / sizeof(inner_leds[0]);

	for (int i = 0; i < ledCharacter.characterTotalPixels; i++) {
		int j = i + 1;
		// Serial.print("j: " + String(j) + "\n");
		// Serial.println("characterWidth: " + String(characterWidth) + "\n");

		int regionInt = (j % characterWidth) - 1;
		if (regionInt < 0)
			regionInt = characterWidth - 1;

		int drawPixel = (pixel + (innerPixelsChunkLength * regionInt)) % innerPixelsTotal;
		int drawPreviousPixel = (previousPixel + (innerPixelsChunkLength * regionInt)) % innerPixelsTotal;

		int regionStart = (innerPixelsChunkLength * regionInt + regionOffset) % innerPixelsTotal;
		int regionEnd = (innerPixelsChunkLength * (regionInt + 1) + regionOffset) % innerPixelsTotal;

		bool drawPixelInRegion = (regionStart <= drawPixel && drawPixel < regionEnd) ||
			(regionStart > regionEnd && (drawPixel < regionEnd || drawPixel >= regionStart));

		bool drawPrevPixelInRegion = (regionStart <= drawPreviousPixel && drawPreviousPixel < regionEnd) ||
			(regionStart > regionEnd && (drawPreviousPixel < regionEnd || drawPreviousPixel >= regionStart));


		if (drawPrevPixelInRegion) {
			if (drawPreviousPixel >= 0 && drawPreviousPixel < inner_leds_size) {
				inner_leds[drawPreviousPixel] = mpColors.off.value;
			} else {
				// Serial.print(DevUtils::termColor("red")) + "drawPreviousPixel out of bounds" + DevUtils::termColor("reset")) + "\n");
			}
		}

		if (drawPixelInRegion) {
			if (drawPixel >= 0 && drawPixel < inner_leds_size) {
				inner_leds[drawPixel] = ledCharacter.characterArray[i] == 1 ? currentColors.letter : mpColors.off.value;
			} else {
				// Serial.print(DevUtils::termColor("red")) + "drawPixel out of bounds" + DevUtils::termColor("reset")) + "\n");
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
			doLetterRegions(theLetter, (FileUtils::config.displayLED.innerChunks - 1), letterPixel);	// Start 1 strip to the left of first strip
		} else if (characterWidth == 5) {
			doLetterRegions(theLetter, (FileUtils::config.displayLED.innerChunks - 2), letterPixel);	// Start 2 strip to the left of first strip
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
		nameScrollDone = true;
		animationInProgress = false;
		animationTypeSetDown = false;
		animationTypeSetUp = false;
		animateFirstCycleDown = true;
		animateFirstCycleUp = true;
		craftDelayTimer = millis();

		// if (millis() - displayDurationTimer > displayMinDuration) {
		// 	nameScrollDone = true;
		// 	animationInProgress = false;
		// 	animationTypeSetDown = false;
		// 	animationTypeSetUp = false;
		// 	animateFirstCycleDown = true;
		// 	animateFirstCycleUp = true;
		// 	craftDelayTimer = millis();
		// }
	}
}

// Create Meteor object
void createMeteor(
	int strip,
	int region,
	bool directionDown = true,
	int startPixel = 0,
	uint8_t meteorSize = 1,
	bool hasTail = true,
	float meteorTailDecayValue = 0.92,
	int rateClass = 5,
	uint8_t animationType = 0)
{
	CHSV meteorColor = currentColors.meteor;

	region = regionWrap(region, directionDown);

	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] == nullptr) {
			animate.ActiveMeteors[i] = new Meteor{
				directionDown,					// directionDown
				startPixel,						// firstPixel
				region,							// region
				(int)outerPixelsChunkLength,	// regionLength
				meteorColor,					// pColor
				meteorSize,						// meteorSize
				hasTail,						// hasTail
				meteorTailDecay,				// meteorTrailDecay
				meteorTailDecayValue,			// meteorTrailDecayValue
				meteorTailRandom,				// meteorRandomDecay
				currentColors.tailHue,			// tailHueStart
				false,							// tailHueAdd
				0.75,							// tailHueExponent
				currentColors.tailSaturation,	// tailHueSaturation
				allStrips[strip],				// rStrip
				rateClass,						// rateClass
				animationType					// animationType
			};
			break;
		}
	}
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
	float meteorTailDecayValue = 0.93,
	int rateClass = 5,
	uint8_t animationType = 0
)
{
	// Stagger the starting pixel
	if (randomizeOffset == true)
		startPixel = startPixel - ((rand() % (offset / 2) + 1) * 2);

	for (int i = 0; i < pulseCount; i++) {
		int16_t pixel = i + startPixel + (i * offset * -1);
		if (randomizeOffset == true)
			pixel = pixel - (random(0, 6) * 2);
		createMeteor(
			strip,
			region,
			directionDown,
			pixel,
			meteorSize,
			hasTail,
			meteorTailDecayValue,
			rateClass,
			animationType
		);
	}
}

void animationMeteorPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t pulseCount = 2,
	int8_t offset = 32,
	bool randomizeOffset = false,
	uint8_t meteorSize = 1,
	bool hasTail = true,
	float meteorTailDecayValue = 0.93,
	int rateClass = 5,
	uint8_t animationType = 0)
{
	for (int i = 0; i < outerChunks; i++) {

		animationMeteorPulseRegion(
			strip,			 		// strip
			i,				 		// region
			directionDown,	 		// directionDown
			0,				 		// startPixel
			pulseCount,		 		// pulseCount
			offset,			 		// offset
			randomizeOffset, 		// randomizeOffset
			meteorSize,		 		// meteorSize
			hasTail,		 		// hasTail
			meteorTailDecayValue, 	// meteorTailDecay
			rateClass,		 		// rateClass
			animationType	 		// animationType
		);
	}
}

void animationSpiralPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t height = 2,
	uint8_t pulseCount = 2,
	uint8_t offset = 12,
	uint8_t spiralMultiplier = 6,
	uint8_t repeats = 4,
	bool hasTail = true,
	float meteorTailDecayValue = 0.93,
	int rateClass = 5,
	uint8_t animationType = 0)
{
	for (int p = 0; p < pulseCount; p++) {
		for (int i = 0; i < outerChunks; i++) {
			int16_t startPixel = ((i * spiralMultiplier) + (p * offset)) * -1;

			animationMeteorPulseRegion(
				strip,
				i,
				directionDown,
				startPixel,
				5,
				((outerChunks + 1) * spiralMultiplier),
				false,
				height,
				hasTail,
				meteorTailDecayValue,
				rateClass,
				animationType
			);
		}
	}
}


void waveAnimation(
	uint8_t strip,
	bool isDown,
	uint8_t numberOfWaves = 1,
	uint8_t offset = 12,
	uint8_t waveSize = 2,
	uint8_t interval = 4,
	bool hasTail = true,
	float meteorTailDecayValue = 0.93,
	int rateClass = 5,
	uint8_t animationType = 0)
{
	uint8_t intervalAdjusted = interval * 2; // Interval is doubled because the LEDs are in front/back pairs
	uint8_t startRegion;

	if (isDown) {
		startRegion = random8(0, outerChunks);
	} else {
		startRegion = random8(0, middleChunks);
	}

	for (uint8_t wave = 0; wave < numberOfWaves; wave++) {
		if (isDown) {
			startRegion += 2;
		} else {
			startRegion -= 2;
		}

		startRegion = regionWrap(startRegion, isDown);

		for (uint8_t region = 0; region < (isDown ? outerChunks : middleChunks); region++) {
			int startPixel = 0;

			if (region < outerChunks / 2) {
				startPixel -= (intervalAdjusted * region) + (offset * wave);
			} else {
				startPixel -= (intervalAdjusted * (outerChunks - region)) + (offset * wave);
			}

			createMeteor(
				strip,
				startRegion,
				isDown,
				startPixel,
				waveSize,
				hasTail,
				meteorTailDecayValue,
				rateClass,
				animationType
			);

			startRegion++;
			startRegion = regionWrap(startRegion, isDown);
		}
	}
}

void zigzagAnimation(
	uint8_t strip,
	bool isDown,
	uint8_t pulseCount,
	uint8_t zigzagSize,
	uint8_t pulseOffset,
	uint8_t height,
	uint8_t interval,
	bool hasTail,
	float meteorTailDecayValue,
	int rateClass,
	uint8_t animationType = 0)
{
	zigzagSize--;

	for (int pulse = 0; pulse < pulseCount; pulse++) {
		// Serial.print("pulse: " + String(pulse) + "\n");

		for (uint8_t region = 0; region < outerChunks; region++) {
			// Serial.print("region: " + String(region) + "\n");

			int startPixel;
			int position = region % (zigzagSize * 2);

			if (position <= zigzagSize) {
				startPixel = (position * (interval * 2)) * -1;
			} else {
				startPixel = ((zigzagSize - (position - zigzagSize)) * (interval * 2)) * -1;
			}

			/* Offset pulses */
			// pulseOffset is multiplied by 2 because the LEDs are in front/back pairs
			startPixel = startPixel - (pulse * (pulseOffset * 2));

			// Serial.print("startPixel: " + String(startPixel) + "\n");

			createMeteor(
				strip,
				region,
				isDown,
				startPixel,
				height,
				hasTail,
				meteorTailDecayValue,
				rateClass,
				animationType
			);
		}
	}
}

void laserGunAnimation(uint8_t strip, uint16_t chargeTime, uint8_t firingSize, int rateClass, uint8_t animationType = 0)
{
	// Charging up animation
	for (uint8_t i = 0; i < outerPixelsChunkLength; ++i) {
		for (uint8_t region = 0; region < outerChunks; ++region) {
			createMeteor(strip, region, true, i, 1, false, rateClass);
		}
		delay(chargeTime / outerPixelsChunkLength);
	}

	// Firing animation
	for (uint8_t i = 0; i < outerPixelsChunkLength - firingSize + 1; ++i) {
		for (uint8_t region = 0; region < outerChunks; ++region) {
			createMeteor(strip, region, true, i, firingSize, true, rateClass);
		}
		delay(chargeTime / (outerPixelsChunkLength * 2));
	}
}




void doInnerCoreMeteors(bool isDown = true, int pulseCount = 1, int offset = 8, int meteorCount = 6, float meteorTailDecayValue = 0.93, int rateClass = 5) {
	for (int i = 0; i < meteorCount; i++) {
		animationMeteorPulseRegion(0, random8(4, 9), isDown, 0, pulseCount, offset, true, 1, true, meteorTailDecayValue, rateClass);
	}
}

struct InnerCoreMeteorSettings {
	bool isDown;
	uint8_t pulseCount;
	uint8_t offset;
	uint8_t meteorCount;
	float meteorTailDecayValue;
};

// Define constant settings for the different rate classes
struct RateClassSettings {
	uint8_t pulseCount;
	uint8_t offset;
	uint8_t height;
	uint8_t numberOfWaves;
	uint8_t waveSize;
	uint8_t zigzagSize;
	uint8_t interval;
	uint8_t spiralMultiplier;
	uint8_t repeats;
	bool randomizeOffset;
	uint8_t meteorSize;
	bool hasTail;
	uint8_t meteorCount;
	float meteorTailDecayValue;
};


const InnerCoreMeteorSettings innerCoreSettings[6] = {
	{pulseCount: 1, offset : 32, meteorCount : 2, meteorTailDecayValue : 0.88},	// Rate class 1
	{pulseCount: 1, offset : 32, meteorCount : 4, meteorTailDecayValue : 0.90},	// Rate class 2
	{pulseCount: 1, offset : 32, meteorCount : 6, meteorTailDecayValue : 0.92},	// Rate class 3
	{pulseCount: 1, offset : 24, meteorCount : 6, meteorTailDecayValue : 0.93},	// Rate class 4
	{pulseCount: 3, offset : 24, meteorCount : 6, meteorTailDecayValue : 0.93},	// Rate class 5
	{pulseCount: 5, offset : 32, meteorCount : 6, meteorTailDecayValue : 0.97,}	// Rate class 6	
};

const RateClassSettings rateClass6Settings[] = {
	/* Meteors */
	{pulseCount: 4, offset : 32, randomizeOffset : true, meteorSize : 2, hasTail : true, meteorTailDecayValue : 0.97},

	/* Ring */
	{pulseCount: 6, offset : 18, randomizeOffset : false, meteorSize : 4, hasTail : true, meteorTailDecayValue : 0.97},

	/* Spiral */
	{pulseCount: 3, offset : 14, height : 2, spiralMultiplier : 2, repeats : 1, hasTail : true, meteorTailDecayValue : 0.96},

	/* Wave */
	{offset: 40, numberOfWaves : 3, waveSize : 4, interval : 2, hasTail : true, meteorTailDecayValue : 0.96},

	/* Zigzag */
	{pulseCount: 3, offset : 18, height : 5, zigzagSize : 3, interval : 4, hasTail : true, meteorTailDecayValue : 0.97},
};

const RateClassSettings rateClass5Settings[] = {
	/* Meteors */
	{pulseCount: 3, offset : 48, randomizeOffset : true, meteorSize : 2, hasTail : true, meteorTailDecayValue : 0.96},

	/* Ring */
	{pulseCount: 5, offset : 32, randomizeOffset : false, meteorSize : 2, hasTail : true, meteorTailDecayValue : 0.96},

	/* Spiral */
	{pulseCount: 2, offset : 14, height : 2, spiralMultiplier : 3, repeats : 1, hasTail : true, meteorTailDecayValue : 0.96},

	/* Wave */
	{offset: 40, numberOfWaves : 3, waveSize : 3, interval : 2, hasTail : true, meteorTailDecayValue : 0.96},

	/* Zigzag */
	{pulseCount: 2, offset : 28, height : 4, zigzagSize : 3, interval : 3, hasTail : true, meteorTailDecayValue : 0.96},
};

const RateClassSettings rateClass4Settings[] = {
	/* Meteor */
	{pulseCount: 2, offset : 64, randomizeOffset : true, meteorSize : 1, hasTail : true, meteorTailDecayValue : 0.96},

	/* Ring */
	{pulseCount: 3, offset : 48, randomizeOffset : false, hasTail : true, meteorTailDecayValue : 0.96},

	/* Spiral */
	{pulseCount: 1, offset : 16, height : 1, spiralMultiplier : 4, repeats : 1, hasTail : true, meteorTailDecayValue : 0.96},

	/* Wave */
	{offset: 32, numberOfWaves : 1, waveSize : 2, interval : 2, hasTail : true, meteorTailDecayValue : 0.96},

	/* Zigzag */
	{},
};

const RateClassSettings rateClass3Settings[] = {
	/* Meteors */
	{pulseCount: 1, offset : 64, randomizeOffset : true, meteorSize : 1, hasTail : true, meteorTailDecayValue : 0.95},

	/* Ring */
	{pulseCount: 1, offset : 32, randomizeOffset : false, meteorSize : 1, hasTail : true, meteorTailDecayValue : 0.95},

	/* Spiral */
	{pulseCount: 1, offset : 14, height : 1, spiralMultiplier : 5, repeats : 1, hasTail : true, meteorTailDecayValue : 0.95},

	/* Wave */
	{},

	/* Zigzag */
	{},
};

const RateClassSettings rateClass2Settings[] = {
	/* Meteors */
	{pulseCount: 1, offset : 64, randomizeOffset : true, meteorSize : 1, hasTail : true, meteorTailDecayValue : 0.97},

	/* Ring */
	{},

	/* Spiral */
	{},

	/* Wave */
	{},

	/* Zigzag */
	{},
};

const RateClassSettings rateClass1Settings[] = {
	/* Meteors */
	{pulseCount: 1, offset : 64, randomizeOffset : true, meteorSize : 1, hasTail : true, meteorTailDecayValue : 0.97},

	/* Ring */
	{},

	/* Spiral */
	{},

	/* Wave */
	{},

	/* Zigzag */
	{},
};

const RateClassSettings* rateClassSettings[] = {
	rateClass1Settings,
	rateClass2Settings,
	rateClass3Settings,
	rateClass4Settings,
	rateClass5Settings,
	rateClass6Settings
};


void doRateBasedAnimation(bool isDown, uint8_t rateClass, uint8_t offset, uint8_t type) {
	if (rateClass < 1 || rateClass > 6) return; // Invalid rate class

	const bool showSerial = FileUtils::config.debugUtils.showSerial;
	const uint8_t stripId = isDown ? 2 : 1;


	// Serial.println("isDown: " + String(isDown) + " | strip: " + String(stripId) + " | rateClass: " + String(rateClass));


	// Determine animation type
	// uint8_t randomTypeAny = (rateClass <= 2) ? 0 : random8(0, 5);
	uint8_t randomTypeAny;
	// Serial.println("random roll: " + String(randomTypeAny));

	if ((isDown && animateFirstCycleDown) || (!isDown && animateFirstCycleUp)) {
		if (rateClass == 5 || rateClass == 6) {
			randomTypeAny = random8(1, 5);
		} else if (rateClass == 3 || rateClass == 4) {
			randomTypeAny = 0;
		}
	} else if (rateClass == 5 || rateClass == 6) {
		randomTypeAny = random8(0, 5);
	} else if (rateClass == 4) {
		randomTypeAny = random8(0, 4);
	} else if (rateClass == 3) {
		randomTypeAny = random8(0, 3);
	} else {
		randomTypeAny = 0;
	}

	if (animate.forcedAnimationType > 0)
		randomTypeAny = animate.forcedAnimationType - 1;

	// Debug log
	if (showSerial) {
		const char* direction = isDown ? "(↓)" : "(↑)";
		char buffer[256];	

		snprintf(
			buffer,
			sizeof(buffer),
			"%s·.·.·.·.·.·★ %s Rate Class: %d | Type: %d\n",
			DevUtils::termColor("cyan"),
			direction,
			rateClass,
			randomTypeAny,
			DevUtils::termColor("reset")
		);
		Serial.print(buffer);
	}

	// Lookup settings
	const RateClassSettings* currentRateSettings = rateClassSettings[rateClass - 1];

	/* Assign animation types */
	if (currentRateSettings) {
		const RateClassSettings& settings = currentRateSettings[randomTypeAny];

		if (rateClass == 1) {
			// Only one meteor for down and one for up
			animationMeteorPulseRegion(stripId, random8(0, 11), isDown, 0, settings.pulseCount, settings.offset, settings.randomizeOffset, settings.meteorSize, settings.hasTail, settings.meteorTailDecayValue, rateClass, randomTypeAny);
		} else {
			switch (randomTypeAny) {
				case 0:
				case 1:
					animationMeteorPulseRing(stripId, isDown, settings.pulseCount, settings.offset, settings.randomizeOffset, settings.meteorSize, settings.hasTail, settings.meteorTailDecayValue, rateClass, randomTypeAny);
					break;
				case 2:
					animationSpiralPulseRing(stripId, isDown, settings.height, settings.pulseCount, settings.offset, settings.spiralMultiplier, settings.repeats, settings.hasTail, settings.meteorTailDecayValue, rateClass, randomTypeAny);
					break;
				case 3:
					waveAnimation(stripId, isDown, settings.numberOfWaves, settings.offset, settings.waveSize, settings.interval, settings.hasTail, settings.meteorTailDecayValue, rateClass, randomTypeAny);
					break;
				case 4:
					zigzagAnimation(stripId, isDown, settings.pulseCount, settings.zigzagSize, settings.offset, settings.height, settings.interval, settings.hasTail, settings.meteorTailDecayValue, rateClass, randomTypeAny);
					break;
			}
		}
	}


	const InnerCoreMeteorSettings& ics = innerCoreSettings[rateClass - 1];

	// Inner Core meteors
	if (isDown) {
		// doInnerCoreMeteors(true, ics.pulseCount, ics.offset, ics.meteorCount, ics.meteorTailDecayValue, rateClass);

		// Inlined doCoreMeteors() here for performance		
		for (uint8_t i = 0; i < ics.meteorCount; i++) {
			animationMeteorPulseRegion(0, random8(4, 9), isDown, 0, ics.pulseCount, offset, true, 1, true, ics.meteorTailDecayValue, rateClass);
		}

		animateFirstCycleDown = false;
	} else {
		animateFirstCycleUp = false;
	}
}


void drawMeteors()
{
	// Update meteor animations
	for (uint16_t i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] != nullptr)
			animate.animateMeteor(animate.ActiveMeteors[i]);
	}
}

void setBottomPixelToMax(int pixel) {
	CHSV color = CHSV(currentColors.meteor.hue, currentColors.meteor.sat, 255);
	bottomPixelMap[pixel] = color;
	bottom_leds[pixel] = color;
}

void updateBottomPixels()
{
	// Update bottom pixels
	for (int i = 0; i < 5; i++) {
		int pixelBrightness = bottomPixelMap[i].value;
		if (pixelBrightness > 0) {
			CHSV color = CHSV(currentColors.meteor.hue, currentColors.meteor.sat, dim8_raw(pixelBrightness - 1));
			bottomPixelMap[i] = color;
			bottom_leds[i] = color;
		}
	}
}

void updateMeteors()
{
	bool debugMeasure = FileUtils::config.debugUtils.diagMeasure;

	// Rate class update timing
	int8_t timingTable[6] = {
		60, // Rate class 1
		50, // Rate class 2
		40, // Rate class 3
		35, // Rate class 4
		20, // Rate class 5
		10  // Rate class 6
	};

	bool animationTypeCanSpiralTable[5] = {
		false,
		false,
		false,
		true,
		false,
	};

	bool updateTable[6] = { false };
	bool spiralUpdateTable[6] = { false };


	/* Update rate classes at different intervals */
	EVERY_N_MILLISECONDS(timingTable[0]) {
		updateTable[0] = true; // Rate class 1
	}
	EVERY_N_MILLISECONDS(timingTable[1]) {
		updateTable[1] = true; // Rate class 2
	}
	EVERY_N_MILLISECONDS(timingTable[2]) {
		updateTable[2] = true; // Rate class 3
	}
	EVERY_N_MILLISECONDS(timingTable[3]) {
		updateTable[3] = true; // Rate class 4
	}
	EVERY_N_MILLISECONDS(timingTable[4]) {
		updateTable[4] = true; // Rate class 5
	}
	updateTable[5] = true; // Rate 6 is not slowed, it runs as fast as possible


	EVERY_N_MILLISECONDS(timingTable[0] * 4) {
		spiralUpdateTable[0] = true;
	}
	EVERY_N_MILLISECONDS(timingTable[1] * 4) {
		spiralUpdateTable[1] = true;
	}
	EVERY_N_MILLISECONDS(timingTable[2] * 4) {
		spiralUpdateTable[2] = true;
	}
	EVERY_N_MILLISECONDS(timingTable[3] * 4) {
		spiralUpdateTable[3] = true;
	}
	EVERY_N_MILLISECONDS(timingTable[4] * 4) {
		spiralUpdateTable[4] = true;
	}
	EVERY_N_MILLISECONDS(timingTable[5] * 4) {
		spiralUpdateTable[5] = true;
	}




	// // Print the updateTable array
	// Serial.print("updateTable: ");
	// for (int i = 0; i < 6; i++) {
	// 	Serial.print("[" + String(updateTable[i]) + "]");
	// }
	// Serial.print("\n");


	// Update first pixel location for all active Meteors in array
	uint16_t activeMeteors = 0;

	for (int i = 0; i < animate.ActiveMeteorArraySize; i++) {
		if (animate.ActiveMeteors[i] != nullptr) {
			Meteor* thisMeteor = animate.ActiveMeteors[i];
			const int rateClass = thisMeteor->rateClass;
			const uint8_t animationType = thisMeteor->animationType;
			const int region = thisMeteor->region;
			const bool directionDown = thisMeteor->directionDown;

			if (debugMeasure) {
				// Count the active meteors for diagnostic output
				activeMeteors++;
			}


			/* Handline horizontal region changes */
			bool doSpiral = spiralUpdateTable[rateClass - 1];
			bool shouldSpiral = animationTypeCanSpiralTable[animationType];

			if (shouldSpiral == true && doSpiral == true) {
				int8_t newRegion = region;
				directionDown == true ? newRegion -= 1 : newRegion += 1; // Move meteor region based on direction
				newRegion = regionWrap(newRegion, directionDown); // Check for wrap around 
				thisMeteor->region = newRegion; // Update region in meteor object
			}

			// Not ready to update meteor location, skip
			if (updateTable[rateClass - 1] == false) {
				continue;
			}


			// Update meteor location
			thisMeteor->firstPixel += 2; // Move meteor 2 pixels (visually moves by 1 due to back/front LED pairs)


			// When meteor reaches the end of the region, animate corresponding bottom pixel
			if (thisMeteor->firstPixel == thisMeteor->regionLength + 2 && thisMeteor->region % 2 == 1) {
				int bottomPixel = ((thisMeteor->region - 1) / 2) % 5;
				setBottomPixelToMax(bottomPixel);
			}

			// If meteor is beyond the display region, unallocate memory and remove array item
			if (thisMeteor->firstPixel > thisMeteor->regionLength * 2) {
				delete thisMeteor;
				animate.ActiveMeteors[i] = nullptr;
			}
		}
	}

	if (debugMeasure)
		Serial.print("Active_Meteors:" + String(activeMeteors) + ",");
}

/** Main Animation update function
 * Gets called every loop();
 */
void updateAnimation(const char* spacecraftName, int spacecraftNameSize, int downSignalRate, int upSignalRate)
{
	feedWatchdog();
	const bool showDiagnostics = FileUtils::config.debugUtils.diagMeasure;

	if (showDiagnostics == true) {
		Serial.print("FPS:");
		Serial.print(FastLED.getFPS() * 1);
		Serial.print(",");
	}

	unsigned long currentMillis = millis(); // Store the current time
	const unsigned long currentDisplayDuration = currentMillis - displayDurationTimer;

	if (showDiagnostics)
		Serial.print("animationCleanup:" + String(displayMinDuration + animationCleanupDelay) + ",");

	/* Update Scrolling letters animation */
	if (nameScrollDone == false && currentDisplayDuration < displayMinDuration + animationCleanupDelay) {
		// Serial.println("-------- scroll letters: " + String(spacecraftName));
		scrollLetters(spacecraftName, spacecraftNameSize);
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

	if (showDiagnostics)
		Serial.print(
			"displayDurationTimer:" + String(displayDurationTimer) +
			", displayMinDuration:" + String(displayMinDuration) +
			", display_diff:" + String(currentMillis - displayDurationTimer) +
			", textMeteorGap:" + String(textMeteorGap) +
			", animationTimer" + String(animationTimer) +
			", animationRepeatGap:" + String(animationRepeatGap) +
			", animation_diff" + String(currentMillis - animationTimer) +
			", animationInProgress:" + String(animationInProgress) +
			", nameScrollDone:" + String(nameScrollDone) +
			", currentDisplayDuration:" + String(currentDisplayDuration) +
			","
		);

	if (
		currentDisplayDuration < displayMinDuration
		&& currentDisplayDuration > textMeteorGap
		) {
		bool shouldAnimate = false;

		// if (animationInProgress && (currentMillis - animationTimer) > animationRepeatGap) {
		// 	shouldAnimate = true;
		// } else if (!animationInProgress && (currentMillis - animationTimer) > textMeteorGap) {
		// 	shouldAnimate = true;
		// }

		if ((currentMillis - animationTimer) > animationRepeatGap) {
			shouldAnimate = true;
		} else if ((currentMillis - animationTimer) > textMeteorGap) {
			shouldAnimate = true;
		}

		// if (shouldAnimate && nameScrollDone == false) {
		if (shouldAnimate) {
			if (downSignalRate > 0 || upSignalRate > 0) {

				if (FileUtils::config.debugUtils.showSerial == true) {
					char animateStatusBuffer[256];
					snprintf(
						animateStatusBuffer,
						sizeof(animateStatusBuffer),
						"%s ★★★ ANIMATE: [%s] (↓) %d | (↑) %d%s \n",
						DevUtils::termColor("bg_blue"),
						spacecraftName,
						downSignalRate,
						upSignalRate,
						DevUtils::termColor("reset")
					);
					Serial.print(animateStatusBuffer);
				}

				if (downSignalRate > 0)
					// printMeteorArray();
					doRateBasedAnimation(true, downSignalRate, meteorOffset, animationTypeDown); // Down animation

				if (upSignalRate > 0)
					doRateBasedAnimation(false, upSignalRate, meteorOffset, animationTypeUp); // Up animation

				animationTimer = currentMillis; // Reset meteor animation timer
			}
		}
	}

	drawMeteors(); // Assign new pixels for meteors
	EVERY_N_MILLISECONDS(100) updateBottomPixels();
	FastLED.show(); // Update LEDs
	updateMeteors(); // Update first pixel location for all active Meteors in array

	if (showDiagnostics)
		FastLED.countFPS();
}

#pragma endregion -- ANIMATION FUNCTIONS

#pragma region -- DATA FUNCTIONS


const unsigned long ONE_Kbps = 1000;
const unsigned long TEN_Kbps = 10 * 1000;
const unsigned long FIFTY_Kbps = 50 * 1000;
const unsigned long HUNDRED_Kbps = 100 * 1000;
const unsigned long TWO_FIFTY_Kbps = 250 * 1000;
const unsigned long FIVE_HUNDRED_Kbps = 500 * 1000;
const unsigned long ONE_Mbps = 1000 * 1000;
const unsigned long TWO_Mbps = 2 * ONE_Mbps;
const unsigned long TWO_POINT_FIVE_Mbps = 2.5 * ONE_Mbps;
const unsigned long MAX_RATE = 1000 * ONE_Mbps; // 1 Gbps

/* Convert long rate to rate class
 * Returns rate class as an unsigned int
 */
unsigned int rateLongToRateClass(unsigned long rate) {
	// Cap the rate at just above 10 Gbps to catch erroneous values
	unsigned long cappedRate = (rate > MAX_RATE) ? MAX_RATE : rate;

	// Classify the rate into one of the 7 rate classes
	if (cappedRate == 0) return 0; // No Data
	else if (cappedRate < ONE_Kbps) return 1; // <1 Kbps
	else if (cappedRate < TEN_Kbps) return 2; // <50 Kbps
	else if (cappedRate < HUNDRED_Kbps) return 3; // <250 Kbps
	else if (cappedRate < FIVE_HUNDRED_Kbps) return 4; // <1 Mbps
	else if (cappedRate < TWO_POINT_FIVE_Mbps) return 5; // <2.5 Mbps
	else return 6; // 2.5 Mbps and above
}


struct FoundSignals {
	unsigned int downSignal;
	unsigned int upSignal;
};

FoundSignals findSignals(XMLElement* xmlDish, CraftQueueItem* tempNewCraft) {
	bool showSerial = FileUtils::config.debugUtils.showSerial;

	FoundSignals foundSignals = { 0, 0 };

	if (FileUtils::config.debugUtils.showSerial == true) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%s>>> Finding signals for %s%s\n", DevUtils::termColor("yellow"), tempNewCraft->callsign, DevUtils::termColor("reset"));
		Serial.print(buffer);
	}

	// Loop through XML signal elements and find the one that matches the target
	for (XMLElement* xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement()) {
		feedWatchdog();
		if (xmlDish == nullptr) continue; // Error finding next element

		const char* elValue = xmlSignal->Value();
		if (elValue == nullptr) continue; // Error getting value

		const char* signalDirection = nullptr;
		if (strcmp(elValue, "downSignal") == 0) {
			signalDirection = "down";
		} else if (strcmp(elValue, "upSignal") == 0) {
			signalDirection = "up";
		} else {
			continue;
		}

		bool isDown;
		if (strcmp(signalDirection, "down") == 0) {
			isDown = true;
		} else if (strcmp(signalDirection, "up") == 0) {
			isDown = false;
		} else {
			continue;
		}

		if (isDown != true && isDown != false) continue;

		feedWatchdog();

		const char* spacecraft = xmlSignal->Attribute("spacecraft");
		if (spacecraft == nullptr) continue;
		// Serial.println("spacecraft: " + String(spacecraft));


		const char* signalType = xmlSignal->Attribute("signalType");
		if (signalType == nullptr) continue;
		// Serial.println("signalType: " + String(signalType));


		if (strcmp(signalType, "data") != 0) continue;

		if (strcmp(spacecraft, tempNewCraft->callsign) != 0) continue;


		const char* rate = xmlSignal->Attribute("dataRate");
		if (rate == nullptr) continue;

		double rateDouble = stod(rate);
		unsigned long rateLong = static_cast<unsigned long>(rateDouble);

		// if (rateLong == 0) continue;
		if (rateLong == 0) {
			if (isDown) {
				// if (showSerial)
				// 	Serial.println("Downsignal rate is 0, skipping");
				continue;
			} else {
				// if (showSerial)
				// 	Serial.println("Upsignal rate is 0 - using placeholder");

				const char* placeholderRate = SpacecraftData::getPlaceholderRate(tempNewCraft->callsign);
				rateDouble = stod(placeholderRate);
				rateLong = static_cast<unsigned long>(rateDouble);
			}
		}

		unsigned int rateClass = rateLongToRateClass(rateLong);

		if (rateClass == 0) continue;

		if (isDown == true) {
			tempNewCraft->downSignal = rateClass;
			foundSignals.downSignal = rateClass;
		} else if (isDown == false) {
			tempNewCraft->upSignal = rateClass;
			foundSignals.upSignal = rateClass;
		}

		if (tempNewCraft->downSignal != 0 && tempNewCraft->upSignal != 0) {
			break;
		}
	}

	// Serial.print("Down signal: " + String(tempNewCraft->downSignal) + "\n");
	// Serial.print("Up signal:   " + String(tempNewCraft->upSignal) + "\n");

	if (tempNewCraft->downSignal == 0 && tempNewCraft->upSignal == 0) {
		if (FileUtils::config.debugUtils.showSerial == true)
			Serial.print(
				DevUtils::termColor("red") +
				"No signals found for " +
				String(tempNewCraft->callsign) +
				DevUtils::termColor("reset") +
				"\n");
	}

	return foundSignals;
}

void sendCrafToQueue(CraftQueueItem* newCraft) {

	if (strlen(newCraft->name) != 0 && (newCraft->downSignal != 0 || newCraft->upSignal != 0)) {

		if (FileUtils::config.debugUtils.showSerial == true) {
			Serial.print(
				"\n" +
				DevUtils::termColor("yellow") +
				">>--------=> Sending to queue >>--------=>" + DevUtils::termColor("reset") +
				"\n");
			printCraftInfo(0, newCraft->callsign, newCraft->name, newCraft->nameLength, newCraft->downSignal, newCraft->upSignal);
			Serial.print(
				DevUtils::termColor("yellow") + "----------------------------------------" + DevUtils::termColor("reset") +
				"\n");
		}

		if (newCraft != nullptr) {
			// Add data to queue, to be passed to another task
			if (xQueueSend(queue, &newCraft, 100) == pdPASS) {
				if (FileUtils::config.debugUtils.showSerial == true) {
					Serial.print(
						DevUtils::termColor("green") +
						"[+] " +
						String(newCraft->callsign) +
						" added to queue" +
						DevUtils::termColor("reset") +
						"\n\n");
				}
			} else {
				if (FileUtils::config.debugUtils.showSerial == true) {
					char failedAddingQueueBuffer[256];
					snprintf(
						failedAddingQueueBuffer,
						sizeof(failedAddingQueueBuffer),
						"%s[-] %s Failed to add to queue%s\n\n",
						DevUtils::termColor("red"),
						newCraft->callsign,
						DevUtils::termColor("reset")
					);
					Serial.print(failedAddingQueueBuffer);
				}
			}
		}
	}
	// freeSemaphoreItem(newCraft);
}

void incrementDataParseCounter() {
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
}

CraftQueueItem* assignValuesToCraftSemaphore(CraftQueueItem* tempNewCraft) {
	CraftQueueItem* newCraft;

	// Get a free item from the freeList
	// Serial.println("freeListTop: " + String(freeListTop));
	
	if (freeListTop < 0) freeListTop = 0;
	if (freeListTop >= MAX_ITEMS) freeListTop = MAX_ITEMS - 1;

	for (uint8_t i = MAX_ITEMS - 1; i > 0; i--) {
		if (freeList[i] != nullptr) {
			// Serial.print("is not nullptr");
		
			newCraft = freeList[freeListTop];

			// Copy callsign
			strlcpy(newCraft->callsignArray, tempNewCraft->callsignArray, sizeof(newCraft->callsignArray));
			newCraft->callsign = newCraft->callsignArray;  // Point to the new data

			// Copy name
			strlcpy(newCraft->nameArray, tempNewCraft->nameArray, sizeof(newCraft->nameArray));
			newCraft->name = newCraft->nameArray;  // Point to the new data

			// Copy name length
			newCraft->nameLength = tempNewCraft->nameLength;

			// Copy down signal
			newCraft->downSignal = tempNewCraft->downSignal;

			// Copy up signal
			newCraft->upSignal = tempNewCraft->upSignal;
		}else {
			Serial.print("freeList[" + String(i) + "] is nullptr\n");
		}
	}

	if (freeListTop > 0) freeListTop--;

	return newCraft;
}

bool isValidCraftQueueItem(CraftQueueItem* item) {
	// Check if callsign and name are not empty
	if (strlen(item->callsignArray) == 0 || strlen(item->nameArray) == 0) {
		return false;
	}

	// Check if nameLength matches the actual length of the name in nameArray
	if (item->nameLength != strlen(item->nameArray)) {
		return false;
	}

	// Check if upSignal and downSignal are within expected ranges
	// (Adjust these checks as necessary based on your specific requirements)
	if (item->upSignal < 0 || item->downSignal < 0) {
		return false;
	}

	// All checks passed
	return true;
}

void parseData(const char* payload)
{
	bool showSerial = FileUtils::config.debugUtils.showSerial;
	feedWatchdog();

	/* XML Parsing */
	XMLDocument xmlDocument; // Create variable for XML document
	const char* charPayload = payload; // Convert payload to char array

	/* Handle XML parsing error */
	try {
		if (xmlDocument.Parse(charPayload) == XML_SUCCESS) {
			if (showSerial == true)
				Serial.print(
					"\n" +
					DevUtils::termColor("green") +
					"XML Parsed Succcessfully" +
					DevUtils::termColor("reset") +
					"\n");
		} else {
			if (showSerial) {
				char unableToParseBuffer[256];
				snprintf(
					unableToParseBuffer,
					sizeof(unableToParseBuffer),
					"%sUnable to parse XML%s\n",
					DevUtils::termColor("red"),
					DevUtils::termColor("reset")
				);
				Serial.print(unableToParseBuffer);
			}

			return;
		}
	}
	catch (XMLError error) {
		if (showSerial) {
			char xmlErrorBuffer[256];
			snprintf(
				xmlErrorBuffer,
				sizeof(xmlErrorBuffer),
				"%sXML Error: %d%s\n",
				DevUtils::termColor("red"),
				error,
				DevUtils::termColor("reset")
			);
			Serial.print(xmlErrorBuffer);
		}	

		return;
	}
	catch (...) {
		if (showSerial) {
			char xmlErrorBuffer[256];
			snprintf(
				xmlErrorBuffer,
				sizeof(xmlErrorBuffer),
				"%sProblem parsing payload%s\n",
				DevUtils::termColor("red"),
				DevUtils::termColor("reset")
			);
			Serial.print(xmlErrorBuffer);
		}
		dev.handleException();
		return;
	}

	/* Find XML elements */
	XMLNode* root = xmlDocument.RootElement();					  // Find document root node
	XMLElement* timestamp = root->FirstChildElement("timestamp"); // Find XML timestamp element


	/* Parse the XML file */
	if (xSemaphoreTake(freeListMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
		printSemaphoreList();
		if (freeListTop >= 0) {
			CraftQueueItem tempNewCraft = {};
			bool breakParseLoop = false;

			/* Loop through all XML elements */
			// 10 is an arbitrary number to prevent an infinite loop
			for (int i = 0; i < 10; i++) {
				// Serial.println("Parsing loop: " + String(i));
				feedWatchdog();
				// if (showSerial == true) {
				// 	Serial.print("\n───────────────────────────────────────────────────────────────────\n");
				// 	Serial.print("  Parse Counters -- Station: " + String(stationCount) + " | Dish: " + String(dishCount) + " | Target: " + String(targetCount) + " | Signal: " + String(signalCount));
				// 	Serial.print("\n───────────────────────────────────────────────────────────────────\n\n");
				// }
				try {
					int s = 0; // Create station elements counter
					for (XMLElement* xmlStation = root->FirstChildElement("station"); true; xmlStation = xmlStation->NextSiblingElement("station")) {
						feedWatchdog();
						vTaskDelay(pdMS_TO_TICKS(100)); // Slow the station loop

						// if (showSerial == true)
						// 	Serial.print("── stationCount: " + String(stationCount) + " | s: " + String(s) + "\n");

						if (xmlStation == NULL) {
							// if (showSerial == true)
							// 	Serial.print("xmlStation == NULL\n");
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
							feedWatchdog();
							vTaskDelay(pdMS_TO_TICKS(100)); // Slow the dish loop

							// if (showSerial == true)
							// 	Serial.print("   └── dishCount: " + String(dishCount) + " | d: " + String(d) + "\n");

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
								feedWatchdog();
								vTaskDelay(pdMS_TO_TICKS(100)); // Slow the target loop

								// if (showSerial == true)
								// 	Serial.print("      └── targetCount: " + String(targetCount) + " | t: " + String(t) + "\n");

								if (xmlTarget == NULL) {
									// if (showSerial == true)
									// 	Serial.print("                xmlTarget == NULL\n");
									goToNextDish = true;
									break;
								}

								if (t > 9 || t > targetCount) {
									// if (showSerial == true)
									// 	Serial.print("                t > 9\n");
									goToNextDish = true;
									break;
								}
								if (t != targetCount) {
									// if (showSerial == true)
									// 	Serial.print("                t != targetCount\n");
									t++;
									continue;
								}

								// if (showSerial == true)
								// 	Serial.print(
								// 		DevUtils::termColor("yellow") +
								// 		">>> Finding craft...." +
								// 		DevUtils::termColor("reset") +
								// 		"\n"
								// 	);


								const char* target = xmlTarget->Attribute("name"); // Get target name

								// Validate target name
								if (target == nullptr) {
									t++;
									continue;
								}

								size_t targetLength = strlen(target);
								if (targetLength >= sizeof(tempNewCraft.callsignArray)) {
									if (showSerial == true) {
										Serial.print(DevUtils::termColor("red"));
										Serial.println("Error: target length exceeds buffer size");
										Serial.println(DevUtils::termColor("reset"));
									}
									t++;
									continue;
								}

								/* Check Blacklist */
								feedWatchdog();
								if (SpacecraftData::checkBlacklist(target) == true) {
									// if (showSerial == true)
									// 	Serial.print(DevUtils::termColor("red") + "Blacklisted target, skipping..." + DevUtils::termColor("reset") + "\n");
									vTaskDelay(pdMS_TO_TICKS(100)); // Delay to allow other tasks to run
									t++;
									continue;
								}

								/* Get the craft name to compare in finding signal */
								int callsignArrayLength = sizeof(CraftQueueItem::callsignArray);
								size_t result = strlcpy(tempNewCraft.callsignArray, target, callsignArrayLength);

								if (result >= callsignArrayLength) {
									if (showSerial == true) {
										Serial.print(DevUtils::termColor("red"));
										Serial.print("Problem copying callsign to tempNewCraft.callsignArray: truncated copy");
										Serial.print(DevUtils::termColor("reset") + "\n");
									}
									t++;
									continue;
								}
								tempNewCraft.callsign = tempNewCraft.callsignArray; // Set the callsign

								feedWatchdog();
								const char* name = SpacecraftData::callsignToName(target);

								if (name == nullptr) {
									if (showSerial == true)
										Serial.print(
											DevUtils::termColor("red") +
											"Error: name is null" +
											DevUtils::termColor("reset") +
											"\n");
									t++;
									continue;
								}

								size_t nameLength = strlen(name);
								if (nameLength >= sizeof(tempNewCraft.nameArray)) {
									if (showSerial == true) {
										Serial.print(DevUtils::termColor("red"));
										Serial.println("Error: name length exceeds buffer size");
										Serial.println(DevUtils::termColor("reset"));
									}
									t++;
									continue;
								}

								result = strlcpy(tempNewCraft.nameArray, name, 100);

								if (result >= 100) {
									if (showSerial == true) {
										Serial.print(DevUtils::termColor("red"));
										Serial.println("Problem copying name to newCraft->nameArray: truncated copy");
										Serial.println(DevUtils::termColor("reset"));
									}
									t++;
									continue;
								}

								tempNewCraft.name = tempNewCraft.nameArray; // Set the name
								tempNewCraft.nameLength = strlen(name); // Set the name length



								if (showSerial == true) {
									char validatedBuffer [256];
									snprintf(
										validatedBuffer,
										sizeof(validatedBuffer),
										"%sValidated: (%s) %s%s\n", DevUtils::termColor("green"), tempNewCraft.callsign,
										name, DevUtils::termColor("reset")
									);
									Serial.print(validatedBuffer);
								}



								/* Look for craft signals */
								feedWatchdog();
								FoundSignals foundSignals = findSignals(xmlDish, &tempNewCraft);

								// No signals found
								if (foundSignals.downSignal == 0 && foundSignals.upSignal == 0) {
									// if (showSerial == true)
									// 	Serial.print(
									// 		DevUtils::termColor("red") +
									// 		"No signals found for " +
									// 		String(tempNewCraft.callsign) +
									// 		DevUtils::termColor("reset") +
									// 		"\n");
									t++;
									continue;
								} else {
									tempNewCraft.downSignal = foundSignals.downSignal;
									tempNewCraft.upSignal = foundSignals.upSignal;
								}


								/** TARGET HAS BEEN FULLY VALIDATED
								 * We hav all of the info needed for this new craft
								 * **/
								// if (showSerial == true) {
								// 	Serial.print("======== TEMP NEW CRAFT ========\n");
								// 	Serial.print("callsign: " + String(tempNewCraft.callsign) + "\n");
								// 	Serial.print("name: " + String(tempNewCraft.name) + "\n");
								// 	Serial.print("nameLength: " + String(tempNewCraft.nameLength) + "\n");
								// 	Serial.print("downSignal: " + String(tempNewCraft.downSignal) + "\n");
								// 	Serial.print("upSignal: " + String(tempNewCraft.upSignal) + "\n");
								// 	Serial.print("================================\n");
								// }

								if (showSerial) {
									char validatedBuffer [256];
									snprintf(
										validatedBuffer,
										sizeof(validatedBuffer),
										"%sValidated: (%s) %s%s\n", DevUtils::termColor("green"), tempNewCraft.callsign, name, DevUtils::termColor("reset")
									);
									Serial.print(validatedBuffer);			
								}


								feedWatchdog();
								CraftQueueItem* newCraft = assignValuesToCraftSemaphore(&tempNewCraft);


								if (isValidCraftQueueItem(newCraft)) {
									sendCrafToQueue(newCraft);

								// if (showSerial == true)
								// 	Serial.print("craft item is valid\n");

									breakParseLoop = true; // Break out of all loops
									targetCount = t; // Set the global target counter to the current target number
									dishCount = d; // Set the global dish counter to the current dish number
									stationCount = s; // Set the global station counter to the current station number

									// Serial.println("free semaphore in target loop");
									xSemaphoreGive(freeListMutex);
									feedWatchdog();

									break;
								} else {
									if (showSerial == true)
										Serial.print(
											DevUtils::termColor("red") +
											"Invalid craft queue item" +
											DevUtils::termColor("reset") +
											"\n");
									t++;
									continue;
								}

								feedWatchdog();
								break; // If somehow we got here, break out of the target loop
							} // End target loop


							if (goToNextDish == true) {
								targetCount = 0;
								dishCount++;
								d++;
								// if (showSerial == true)
								// 	Serial.println("go to next dish!");
								continue;
							}

							if (breakParseLoop == true) break;
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
							// if (FileUtils::config.debugUtils.showSerial == true)
							// 	Serial.println("go to next station!");
							continue;
						}

						if (breakParseLoop == true) break;

						s++;
					} // End station loop
				}

				catch (...) {
					if (FileUtils::config.debugUtils.showSerial == true) {
						Serial.print(DevUtils::termColor("red"));
						Serial.println("Problem parsing payload:");
						Serial.println(DevUtils::termColor("reset"));
					}
					dev.handleException();
					parseCounter++;
					xSemaphoreGive(freeListMutex);
					return;
				}

				if (breakParseLoop == true) break;
			} // End arbitary loop

			incrementDataParseCounter();
			xSemaphoreGive(freeListMutex); //Line 2753 throwing error
			feedWatchdog();
			return;
		} else {
			// There are no free items in the queue item pool
			Serial.println("No free items in queue item pool");
			xSemaphoreGive(freeListMutex);
			return;
		}


		xSemaphoreGive(freeListMutex);

		if (FileUtils::config.debugUtils.showSerial == true)
			Serial.print("XML Parse attempts: " + String(parseCounter) + "\n");
		parseCounter++;

		feedWatchdog();
		return;
	}
	// Serial.println("fetch done");
	feedWatchdog();
}

void logOutput(const char* color, const String& message) {
	const bool showSerial = FileUtils::config.debugUtils.showSerial;
	if (showSerial == true) {
		String resetColorString = DevUtils::termColor("reset");
		String coloredMessageString = DevUtils::termColor(color);

		const char* resetColor = resetColorString.c_str();
		const char* coloredMessage = coloredMessageString.c_str();

		int bufferSize = strlen(coloredMessage) + message.length() + strlen(resetColor) + 2; // +1 for newline, +1 for null terminator
		
		const char* messageCharPtr = message.c_str();

		char* buffer = (char*)malloc(bufferSize);
		if (buffer) {
			snprintf(
				buffer,
				bufferSize,
				"%s%s%s\n",
				coloredMessage,
				messageCharPtr,
				resetColor
			);
			Serial.print(buffer);
			free(buffer);
		} else {
			// handle memory allocation failure
			if (showSerial == true)
				Serial.print("Memory allocation failed\n");
		}
	}
}

bool isWiFiConnected() {
	if (WiFi.status() == WL_CONNECTED) {
		logOutput("green", "WiFi Connected");
		return true;
	} else {
		logOutput("red", "WiFi Disconnected");
		return false;
	}
}

char* generateFetchUrl() {
	static char fetchUrl[64];  // Use static storage duration
	memset(fetchUrl, 0, sizeof(fetchUrl));
	try {
		if (FileUtils::config.wifiNetwork.serverName != NULL) {
			strncpy(fetchUrl, FileUtils::config.wifiNetwork.serverName, sizeof(fetchUrl) - 1);
			char randBuffer[10];
			ltoa(random(999999999), randBuffer, 10);
			strncat(fetchUrl, randBuffer, sizeof(fetchUrl) - strlen(fetchUrl) - 1);
		}
	}
	catch (...) {
		Serial.println("Error: generateFetchUrl() failed");
		dev.handleException();
	}
	return fetchUrl;
}



bool handleHttpResponse(uint16_t httpResponseCode) {
	if (httpResponseCode != 200) {
		Serial.print("HTTP Response: " + String(httpResponseCode) + " - " + http.errorToString(httpResponseCode) + "\n");
		return false;
	} else {
		try {
			// String res = http.getString();
			// const char* charRes = res.c_str();
			logOutput("green", "HTTP response received:" + String(httpResponseCode));
			usingDummyData = false;
			return true;
		}
		catch (...) {
			Serial.println("Error: handleHttpResponse() failed");
			dev.handleException();
			return false;
		}
	}
}

bool attemptHTTPConnection(const String& url) {
	if (!http.begin(url.c_str())) {
		logOutput("red", "[!] Failed to connect to server");
		return false;
	}
	feedWatchdog();
	http.setTimeout(5000);
	http.addHeader("Content-Type", "text/xml");
	int httpResponseCode = http.GET();
	feedWatchdog();
	return handleHttpResponse(httpResponseCode);
}

bool fetchHTTPData(const String& url) {
	bool showSerial = FileUtils::config.debugUtils.showSerial;

	const int maxHttpRetries = 3;
	for (int retry = 0; retry < maxHttpRetries; retry++) {
		feedWatchdog(); // HTTP connection can sometimes be slow
		
		char conectionAttemptBuffer[128];
		snprintf(
			conectionAttemptBuffer,
			sizeof(conectionAttemptBuffer),
			"%sHTTP connection attempt %d of %d%s\n",
			DevUtils::termColor("yellow"),
			retry + 1,
			maxHttpRetries,
			DevUtils::termColor("reset")
		);

		try {
			if (attemptHTTPConnection(url) == true) {
				if (showSerial) {
					Serial.print("Getting string from API response...\n");
				}

				try {
					String res = http.getString();

					if (res.length() == 0) {
						Serial.println("Response timeout");
						http.end();
						return false;
					}

					if (res == nullptr or res == "") {
						Serial.print("Error: API response is empty\n");
						http.end();
						return false;
					}

					feedWatchdog(); // The response may be large, just in case
					
					if (showSerial) {
						Serial.print("Writing XML data to file...\n");
					}

					File xmlFile = LittleFS.open("/temp.xml", "w");
					if (!xmlFile) {
						logOutput("red", "Failed to open XML file for writing");
						http.end();
						return false;
					}

					size_t bytesWritten = xmlFile.print(res);
					xmlFile.close();
					feedWatchdog(); // Just in case writing to the file is slow
				}
				catch (...) {
					Serial.print("Error getting string from API response\n");
				}

				http.end();
				return true; // XML data saved to LittleFS
			} else {
				http.end();
				if (FileUtils::config.debugUtils.showSerial == true)
					Serial.println("[fetchHTTPData] Failed to fetch data");
			}

			// vTaskDelay(pdMS_TO_TICKS(100)); // Wait to allow other tasks to run
		}
		catch (...) {
			Serial.println("Error: fetchHTTPData() failed");
			dev.handleException();
			http.end();  // Close connection in case of an exception
			vTaskDelay(pdMS_TO_TICKS(100)); // Replace delay with vTaskDelay
		}
	}
	http.end();  // Ensure connection is closed even if no successful fetch
	return false;  // No data fetched
}


void fetchData() {
	feedWatchdog();

	const bool showSerial = FileUtils::config.debugUtils.showSerial;

	if (FileUtils::config.debugUtils.testCores == true) {
		Serial.print("fetchData() running on core " + String(xPortGetCoreID()) + "\n\n");
	}

	if (showSerial) {
		Serial.print("\n\n");
		Serial.print("   ┌─────────────────────────────┐\n");
		Serial.print("───│          FETCH DATA         │───\n");
		Serial.print("   └─────────────────────────────┘\n");
	}

	if (uxQueueSpacesAvailable(queue) == 0) {
		logOutput("red", ">>>   Queue is full, returning");
		return;
	}


	bool dataFetched = false;

	if (!forceDummyData && isWiFiConnected()) {
		String url = generateFetchUrl();
		
		if (showSerial) {
			char deviceIPBuffer[128];
			String deviceIP = WiFi.localIP().toString();
			String connectedNetwork = WiFi.SSID();
			snprintf(
				deviceIPBuffer,
				sizeof(deviceIPBuffer),
				"%s on %s\n",
				deviceIP,
				connectedNetwork
			);
			Serial.print(deviceIPBuffer);

			Serial.print("URL: " + url + "\n");			

			// char urlBuffer[512];
			// snprintf(
			// 	urlBuffer,
			// 	sizeof(urlBuffer),
			// 	"Generated URL: %s\n",
			// 	url
			// );
			// Serial.print(urlBuffer);
		}

		dataFetched = fetchHTTPData(url);

		if (showSerial) {
			const char* dataFetchedStatusString = dataFetched ? "Success" : "Failure";
			const char* dataStatusColor = dataFetched ? "green" : "red";

			char dataStatusBuffer[128];
			snprintf(
				dataStatusBuffer,
				sizeof(dataStatusBuffer),
				"%sData Fetch: %s %s\n",
				DevUtils::termColor(dataStatusColor),
				dataFetchedStatusString,
				DevUtils::termColor("reset")
			);

			// Print the data status buffer
			Serial.print(dataStatusBuffer);
		}
	}

	File xmlFile;

	static char xmlDataBuffer[20480];  // 20KB buffer

	if (dataFetched) {
		feedWatchdog();

		if (showSerial) {
			char dataStatusBuffer[128]; 
			snprintf(
				dataStatusBuffer,
				sizeof(dataStatusBuffer),
				"%s %s %s\n",
				DevUtils::termColor("bg_blue"),
				"Using live data",
				DevUtils::termColor("reset")
			);
			Serial.print(dataStatusBuffer);
		}

		xmlFile = LittleFS.open("/temp.xml", "r");
		if (!xmlFile) {
			if (showSerial)
				Serial.println("[fetchData] Failed to open XML file for reading.");

			return;
		}
		size_t xmlFileSize = xmlFile.size();

		if (showSerial)
			Serial.println("[fetchData] XML file size: " + String(xmlFileSize));

		if (xmlFileSize > sizeof(xmlDataBuffer) - 1) {
			if (showSerial)
				Serial.println(DevUtils::termColor("red") + "[fetchData] XML file is too large for buffer." + DevUtils::termColor("reset"));

			xmlFile.close();
			return;
		}

		feedWatchdog();

		// Read the entire file into the buffer
		xmlFile.readBytes(xmlDataBuffer, xmlFileSize);
		xmlDataBuffer[xmlFileSize] = '\0'; // Null-terminate the string

		xmlFile.close();
		LittleFS.remove("/temp.xml"); // Optionally delete the file after reading

		if (showSerial)
			Serial.println("[fetchData] XML file read");

		feedWatchdog();
		parseData(xmlDataBuffer); // Parse live data
	} else {
		// If no data was fetched, use dummy data
		if (showSerial) {
			char dataStatusBuffer[128];
			snprintf(
				dataStatusBuffer,
				sizeof(dataStatusBuffer),
				"%s %s %s\n",
				DevUtils::termColor("bg_purple"),
				"Using dummy data",
				DevUtils::termColor("reset")
			);
			Serial.print(dataStatusBuffer);
		}

		parseData(dummyXmlData); // Parse dummy data
	}

	if (!dataStarted) {
		dataStarted = true;  // Set dataStarted to true after first data fetch
	}

	if (showSerial)
		Serial.print("(* END fetchData *)\n\n");

	feedWatchdog();

	if (!isWiFiConnected && wm.getWiFiIsSaved() == true) {
		Serial.print("Trying to reconnect to WiFi \"" + String(wm.getWiFiSSID()) + "\"\n");

		bool res = wm.autoConnect(FileUtils::config.wifiNetwork.apSSID, FileUtils::config.wifiNetwork.apPass);

		if (!res) { // Wifi connection failed
			Serial.print(
				DevUtils::termColor("red") +
				"Failed to reconnect to WiFi" +
				DevUtils::termColor("reset"));
		} else { // Wifi connection successful
			String deviceIP = WiFi.localIP().toString();
			String connectedNetwork = WiFi.SSID();
			char successStringBuffer[256];
			snprintf(
				successStringBuffer,
				sizeof(successStringBuffer),
				"%sConnected to WiFi '%s' successfully%s\n",
				DevUtils::termColor("green"),
				connectedNetwork != nullptr ? connectedNetwork : "NULL",
				DevUtils::termColor("reset")
			);
			Serial.print(successStringBuffer);
		}
	}

	return;
}



// Fetch XML data from HTTP & parse for use in animations
void getData(void* parameter) {
	// UBaseType_t uxHighWaterMark;
	// uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	// Serial.print("high_water_mark:"); Serial.print(uxHighWaterMark); Serial.print("\n");

	for (;;) {
		feedWatchdog();
		if (otaUpdateTriggered) {
			vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to allow other tasks to run
			continue;
		}
		try {
			// Send an HTTP POST request every so many seconds
			// Serial.println(String(millis()) + " - " + String(lastTime) + " - " + String(timerDelay));
			// Serial.println("= " + String(millis() - lastTime));
			if ((millis() - lastTime) > timerDelay) {
				// Serial.println("getData() running");
				if (uxQueueSpacesAvailable(queue) > 0) {
					// Serial.println("getData() queue space available");

					vTaskDelay(pdMS_TO_TICKS(100)); // Delay to allow other tasks to run
					
					fetchData();
				} else {
					if (FileUtils::config.debugUtils.showSerial == true)
						Serial.print("Craft Queue is full, waiting...\n");
					vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to allow other tasks to run
				}

				lastTime = millis(); // Sync reference variable for timer
			}
		}
		catch (...) {
			Serial.println("Error: getData() failed");
			dev.handleException();
		}

		vTaskDelay(pdMS_TO_TICKS(100)); // delay for 100 milliseconds to allow other tasks to run
	}
}

#pragma endregion -- DATA FUNCTIONS







/* SETUP
* Runs once at startup */
void setup()
{
	esp_task_wdt_init(60, true); // Enable watchdog timer with 60 second timeout

	Serial.begin(115200); // Begin serial communications, ESP32 uses 115200 rate

	DevUtils::SerialBanners::printBootSplashBanner(); // Display fancy splash ASCII graphics
	Serial.print("VERSION: " + String(currentFirmwareVersion) + "\n\n");
	Serial.print("Starting...\n\n");

	// Initialize LittleFS
	if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
		Serial.println("LittleFS Mount Failed");
		return;
	}


	// Make sure we can read the EEPROM
	if (LittleFS.begin(true)) {
		Serial.println("LittleFs filesystem mounted successfully");
		Serial.print("\n");
		Serial.print("┌────────────────────────────────┐\n");
		Serial.print("│ ./data  |   ROOT FILESYSTEM    │\n");
		Serial.print("└────────────────────────────────┘\n");
		FileUtils::listFilesystem("/", 0);
		Serial.print("\n");
		Serial.print("└────────────────────────────────┘\n\n");

		FileUtils::initConfigFile();
		Serial.print("Config loaded\n\n");
		FileUtils::printAllConfigFileKeys();

		// if (FileUtils::config.wifiNetwork.apPass != nullptr) {
		// 	Serial.print("password: " + String(FileUtils::config.wifiNetwork.apPass) + "\n");
		// } else {
		// 	Serial.print("password: NULL\n");
		// }
		Serial.print("serverName: " + String(FileUtils::config.wifiNetwork.serverName) + "\n\n");
	} else {
		Serial.println("An Error has occurred while mounting LittleFS filesystem");
		Serial.println("Using default config, settings will not be saved");
	}

	// Reset settings - wipe stored credentials for testing
	// wm.resetSettings();
	// wm.setDebugOutput(true);

	// if (FileUtils::config.debugUtils.showSerial == false) {
	// 	Serial.setDebugOutput(false);
	// 	wm.setDebugOutput(false);
	// }
	wm.setDebugOutput(false);
	Serial.setDebugOutput(false);

	if (FileUtils::config.debugUtils.testCores == true) {
		Serial.print("setup() running on core " + String(xPortGetCoreID()) + "\n\n");
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
	FastLED.setBrightness(FileUtils::config.displayLED.brightness);

	/* Fast LED Hardware Setup */
	FastLED.addLeds<NEOPIXEL, OUTER_PIN>(outer_leds, outerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, MIDDLE_PIN>(middle_leds, middlePixelsTotal);
	FastLED.addLeds<NEOPIXEL, INNER_PIN>(inner_leds, innerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, BOTTOM_PIN>(bottom_leds, bottomPixelsTotal);
	allStripsOff();

	/* Test strips by filling with different colors */
	if (FileUtils::config.debugUtils.testLEDs == true) {
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
	WiFi.mode(WIFI_AP_STA);  // explicitly set mode, esp defaults to STA+AP
	wm.setCountry("US");	  // setting wifi country seems to improve OSX soft ap connectivity
	wm.setConfigPortalBlocking(false);
	wm.setCleanConnect(true);
	wm.setConnectRetries(3);
	wm.setConnectTimeout(10); // connect attempt fails after n seconds	
	// wm.setAPStaticIPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0)); // set static ip
	wm.setHostname("JPL_MiniPulse");
	// wm.setEnableConfigPortal(false); // Disable auto AP, it will be started manually
	// wm.setDisableConfigPortal(true); // Disable auto-closing config portal
	wm.setCaptivePortalEnable(false);
	wm.setWebServerCallback(webServerCallback);
	wm.setTitle("JPL MiniPulse");

	// wm.setPreSaveConfigCallback(saveParamsCallback);
	// wm.setPreSaveParamsCallback(saveParamsCallback);
	wm.setSaveParamsCallback(saveParamsCallback);
	std::vector<const char*> menu = { "wifi", "info", "param", "sep", "restart" };
	wm.setMenu(menu);

	// set dark theme
	wm.setClass("invert");

	int brightnessMapped = MathHelpers::map(FileUtils::config.displayLED.brightness, 8, 160, 0, 100);


	/* User Settings */
	new (&param_brightness) WiFiManagerParameter("brightness", "Brightness", String(FileUtils::config.displayLED.brightness).c_str(), 3, "type='range' min='8' max='160' step='1'");
	wm.addParameter(&param_brightness);

	new (&param_force_dummy_data) WiFiManagerParameter("force-dummy-data", "Force placeholder data", String(FileUtils::config.displayLED.brightness).c_str(), 3, "type='range' min='8' max='160' step='1'");
	wm.addParameter(&param_force_dummy_data);

	/* Developer Settings */
	// Brightness
	new (&param_show_serial) WiFiManagerParameter("show_serial", "Show Serial", FileUtils::config.debugUtils.showSerial ? "1" : "0", 1, "type='number' min='0' max='1' step='1'");
	wm.addParameter(&param_show_serial);

	// Show graphing diagnostics
	new (&param_show_diagnostics) WiFiManagerParameter("show_diagnostics", "Show Diagnostics", FileUtils::config.debugUtils.diagMeasure ? "1" : "0", 1, "type='number' min='0' max='1' step='1'");
	wm.addParameter(&param_show_diagnostics);

	// Force override animation type
	new (&param_force_animation_type) WiFiManagerParameter("force_animation_type", "Force Animation Type", String(animate.forcedAnimationType).c_str(), 1, "type='number' min='0' max='5' step='1'");
	wm.addParameter(&param_force_animation_type);

	/* Custom */
	const char* update_button_html = R"---(
		<div>
			<div style="display: inline-block; border: 1px solid gray;padding:5px 20px;">
				<p id="firmwareStatus" style="margin-top:0;></p>
				<p id="updateResponse"></p>
				<button id="updateButton">Update Firmware</button>
			</div>
		</div>
		<script>
			fetch('/get-latest-version-number')
			.then(function (response) {
				return response.text();
			}).
			then(function (text) {
				document.getElementById("firmwareStatus").innerHTML = text;
				console.log(text);
			})

			document.getElementById("updateButton").addEventListener('click', function() {
				fetch('/trigger-firmware-update')
				.then(function (response) {
					return response.text();
				})
				.then(function (text) {
					document.getElementById("updateResponse").innerHTML = text;
					console.log("response:", text);
				})
				.catch(function (error) {
					console.error(error);
				});
			});
		</script>
	)---";

	wm.setCustomHeadElement(update_button_html); // Add "Update Firmware" button to WiFi portal <head>


	char existingWifiBuffer[128];
	snprintf(
		existingWifiBuffer,
		sizeof(existingWifiBuffer),
		"Existing WiFi credentials: %s\n",
		wm.getWiFiIsSaved() ? "TRUE" : "FALSE"
	);
	Serial.print(existingWifiBuffer);

	if (wm.getWiFiIsSaved() == true) {
		char ssidBuffer[256];
		snprintf(
			ssidBuffer,
			sizeof(ssidBuffer),
			"Connecting to WiFi network %s...\n",
			wm.getWiFiSSID().c_str()
		);
		Serial.print(ssidBuffer);

		bool res;
		res = wm.autoConnect(FileUtils::config.wifiNetwork.apSSID, FileUtils::config.wifiNetwork.apPass);

		if (!res) // Wifi connection failed
		{
			char failureStringBuffer[256];
			snprintf(failureStringBuffer, sizeof(failureStringBuffer), "%sFailed to connect to WiFi%s\n", DevUtils::termColor("red"), DevUtils::termColor("reset"));
			Serial.print(failureStringBuffer);
		} else // Wifi connection successful
		{
			String deviceIP = WiFi.localIP().toString();
			String deviceLocalWifi = WiFi.SSID();
			char successStringBuffer[256];
			snprintf(
				successStringBuffer,
				sizeof(successStringBuffer),
				"%sConnected to WiFi successfully%s\n%s on %s\n\n",
				DevUtils::termColor("green"),
				DevUtils::termColor("reset"),
				deviceIP != nullptr ? deviceIP : "NULL",
				deviceLocalWifi != nullptr ? deviceLocalWifi : "NULL"
			);
			Serial.print(successStringBuffer);
		}

	} else {
		Serial.print("Use the WiFi access portal for configuration\n\n");
	}
	Serial.print("WiFi Status: " + wm.getWLStatusString() + "\n\n");

	// Start the web portal anyways for settings
	Serial.print("Starting local WiFi access point for configuration...\n");

	delay(1000); // Small delay to allow WiFi to connect

	if (FileUtils::config.wifiNetwork.apSSID != nullptr && strcmp(FileUtils::config.wifiNetwork.apSSID, "") != 0) {
		if (FileUtils::config.wifiNetwork.apPass != nullptr && strcmp(FileUtils::config.wifiNetwork.apPass, "") != 0) {
			wm.startConfigPortal(FileUtils::config.wifiNetwork.apSSID, FileUtils::config.wifiNetwork.apPass);

		} else {
			wm.startConfigPortal(FileUtils::config.wifiNetwork.apSSID);
		}
	} else {
		Serial.print("No SSID set for access point\n");
		wm.startConfigPortal();
	}

	feedWatchdog();
	delay(1000); // Small delay to allow WiFi to connect

	const char* apPassword = FileUtils::config.wifiNetwork.apPass;
	DevUtils::SerialBanners::printWiFiConfigBanner(apPassword, wm); // Display Wifi config portal connection info




	http.setReuse(true);	   // Use persistent connection
	setupOtaUpdate();		   // Setup OTA update

	SpacecraftData::loadJson();		   // Load JSON data for spacecraft lookup
	// SpacecraftData::loadSpacecraftNamesProgmem(); // Load raw spacecraft names for lookup
	// SpacecraftData::loadSpacecraftPlaceholderRatesRaw(); // Load raw spacecraft placeholder rates for lookup
	// SpacecraftData::loadSpacecraftBlacklistRaw(); // Load raw spacecraft blacklist for lookup
	// SpacecraftData::loadSpacecraftBlacklist();
	delay(100);


	/* Assign config to global state variables */
	characterWidth = FileUtils::config.textTypography.characterWidth;

	setColorTheme(colorTheme); // Set color theme
	// drawBottomPixels();		   // Draw initial bottom pixels
	delay(100);				   // Small delay to allow bottom pixels to draw
	feedWatchdog();

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

	/* Initialize the data task on the second core */
	xTaskCreatePinnedToCore(
		getData,	  /* Function to implement the task */
		"getData",	  /* Name of the task */
		6144,		  /* Stack size in byts */
		NULL,		  /* Task input parameter */
		0,			  /* Priority of the task */
		&xHandleData, /* Task handle. */
		0);			  /* Core where the task should run */

	queue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create queue to pass data between tasks on separate cores

	// Check that queue was created successfully
	if (queue == NULL) {
		if (FileUtils::config.debugUtils.showSerial) {
			Serial.print(
				DevUtils::termColor("red") +
				"Error creating the queue" +
				DevUtils::termColor("reset") +
				"\n");
			delay(3000);
		}
		ESP.restart();
	}

	// updateFirmwareOta(); // Check for OTA update
	feedWatchdog();
	delay(1000);

}


/* MAIN LOOP
 * Runs repeatedly as long as board is on
 */
void loop() {
	feedWatchdog();
	uint32_t currentMillis = millis(); // Store the current time

	bool showSerial = FileUtils::config.debugUtils.showSerial;
	bool testCoresEnabled = FileUtils::config.debugUtils.testCores;
	bool diagMeasureEnabled = FileUtils::config.debugUtils.diagMeasure;

	// Check core test
	if (testCoresEnabled && currentMillis - lastTime > 4000 && currentMillis - lastTime < 4500) {
		Serial.printf("loop() running on core: %d\n", xPortGetCoreID());
	}

	doWiFiManager();

	try {
		// Check for new data
		if (dataStarted && nameScrollDone && (currentMillis - displayDurationTimer) > displayMinDuration + animationCleanupDelay && (currentMillis - craftDelayTimer) > craftDelay) {

			if (diagMeasureEnabled) {
				// Serial.printf("Queue: %u\n", uxQueueMessagesWaiting(queue));
			}

			CraftQueueItem theInfoBuffer; // Buffer to hold data from queue
			CraftQueueItem* infoBuffer = &theInfoBuffer;

			if (queue && infoBuffer && xQueueReceive(queue, &infoBuffer, 1) == pdPASS) {
				if (showSerial) {
					char retrieveQueueBuffer[256];
					snprintf(
						retrieveQueueBuffer,
						sizeof(retrieveQueueBuffer),
						"\n\n%s%s%s",
						"   ┌────────────────────────────────┐   \n",
						"───│          RETRIEVE QUEUE        │───\n",
						"   └────────────────────────────────┘   \n"
					);
					printCurrentQueue(queue);

					// printCraftInfo(0, infoBuffer->callsign, infoBuffer->name, infoBuffer->nameLength, infoBuffer->downSignal, infoBuffer->upSignal);
					// Serial.println("--------------------------------------------\n\n");
				}

				// Copy data from queue to buffer
				if (infoBuffer && infoBuffer->name && strlen(infoBuffer->name) > 0) {
					strncpy(currentCraftBuffer.callsignArray, infoBuffer->callsignArray, sizeof(currentCraftBuffer.callsignArray) - 1);
					currentCraftBuffer.callsignArray[sizeof(currentCraftBuffer.callsignArray) - 1] = '\0'; // Ensure null-termination

					strncpy(currentCraftBuffer.nameArray, infoBuffer->nameArray, sizeof(currentCraftBuffer.nameArray) - 1);
					currentCraftBuffer.nameArray[sizeof(currentCraftBuffer.nameArray) - 1] = '\0'; // Ensure null-termination

					currentCraftBuffer.nameLength = infoBuffer->nameLength;
					currentCraftBuffer.downSignal = infoBuffer->downSignal;
					currentCraftBuffer.upSignal = infoBuffer->upSignal;

					nameScrollDone = false;

					if (showSerial) {
						printCurrentCraftBuffer();
					}
				} else if (showSerial) {
					Serial.println("Craft name is empty");
					noTargetFoundCounter++;
				}

				freeSemaphoreItem(infoBuffer);

				if (!firstStartupAnimation) {
					firstStartupAnimation = false;
				} else {
					displayDurationTimer = currentMillis;
				}
			}
		}
	}
	catch (...) {
		Serial.print("Error: loop() getting new data failed\n");
		dev.handleException();
	}

	// Update All LED Animations
	try {
		updateAnimation(currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	}
	catch (...) {
		if (showSerial) {
			Serial.print("Error: updateAnimation() failed\n");
		}
		dev.handleException();
	}

	// Serial display diagnostics for plotter
	if (diagMeasureEnabled) {
		Serial.println("FreeHeap:" + String(ESP.getFreeHeap() * .001) + ",");
		// char buffer[1024];
		// snprintf(buffer, sizeof(buffer),
		// 	"Duration:%ld\nDelay:%ld\nQueue:%u\nFreeListTop:%d\n"
		// 	"stationCount:%d\ndishCount:%d\ntargetCount:%d\nsignalCount:%d\n"
		// 	"parseCounter:%d\nnoTargetFoundCounter:%d\nanimationTypeDown:%d\n"
		// 	"animationTypeUp:%d\nPerfTimer:%ld\nFree Heap:%d\n",
		// 	(currentMillis - displayDurationTimer) / 1000,
		// 	(currentMillis - craftDelayTimer) / 1000,
		// 	uxQueueMessagesWaiting(queue),
		// 	freeListTop,
		// 	stationCount,
		// 	dishCount,
		// 	targetCount,
		// 	signalCount,
		// 	parseCounter,
		// 	noTargetFoundCounter,
		// 	animationTypeDown,
		// 	animationTypeUp,
		// 	currentMillis - perfTimer,
		// 	ESP.getFreeHeap(),
		// 	"\n"
		// );
		// Serial.print(buffer);
		// perfTimer = currentMillis;
	}
}
