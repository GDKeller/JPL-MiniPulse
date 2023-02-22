/* LIBRARIES */
#include <Arduino.h>			// Arduino core
#include <HTTPClient.h>			// HTTP client
#include <Adafruit_NeoPixel.h>	// NeoPixel lib
#include <FastLED.h>			// FastLED lib
#include <tinyxml2.h>			// XML parser
#include <iostream>				// C++ I/O
#include <cstring>				// C++ string
#include <ArduinoJson.h>		// JSON parser
#include <algorithm>			// C++ algorithms
#include <MathHelpers.h>		// Custom math helpers lib
#include <TextCharacters.h>		// Custom LED text lib
#include <AnimationUtils.h>		// Custom animation utilities lib
#include <Animate.h>			// Custom animate lib
#include <SpacecraftData.h>		// Custom spacecraft data lib
#include <WiFiManager.h>		// WiFi manager lib

/* NAMESPACES */
using namespace tinyxml2;		// XML parser
using namespace std;			// C++ I/O
 
/* HARDWARE CONFIG */
#pragma region
#define AP_SSID "MiniPulse"		// WiFi AP SSID
#define AP_PASS ""				// WiFi AP password
#define OUTER_PIN 17			// Outer ring pin
#define MIDDLE_PIN 18			// Middle ring pin
#define INNER_PIN 19			// Inner ring pin
#define BOTTOM_PIN 16			// Bottom ring pin
#define WIFI_RST 21				// WiFi reset pin
#define OUTPUT_ENABLE 22		// Output enable pin
#define BRIGHTNESS 16			// Global brightness value. 8bit, 0-255
#define POTENTIOMETER 32		// Brightness potentiometer pin
#define FPS 30					// Frames per second
#pragma endregion


/* DIAGNOSTICS */
// all 0 is normal operation
#define TEST_CORES 0			// Test cores
#define SHOW_SERIAL 0			// Show serial output
#define ID_LEDS 0				// ID LEDs
#define DISABLE_WIFI 0			// Disable WiFi
#define DIAG_MEASURE 0			// Output memory & performance info for plotter



#pragma region CLASS INSTANTIATIONS
// Animations
AnimationUtils au(POTENTIOMETER);	// Instantiate animation utils
AnimationUtils::Colors mpColors;	// Instantiate colors
Animate animate;					// Instantiate animate

// Data
SpacecraftData data;				// Instantiate spacecraft data

// Tasks
TaskHandle_t xHandleData; // Task for fetching and parsing data
TaskStatus_t xHandleDataDetails; // Task details for HandleData
QueueHandle_t queue;	 // Queue to pass data between tasks

// Networking
WiFiManager wm;			// Used for connecting to WiFi
HTTPClient http;		// Used for fetching data
#pragma endregion


/**
 * Color Theme
 * ID - Name		(Letter/Meteor/Meteor tail)
 * 0  - Snow		(White/white/blue tint)
 * 1  - Cyber		(Teal/teal/blue tint)
 * 2  - Valentine	(pink/pink/purple tint)
*/
const uint8_t colorTheme = 0;

const char* serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml?r=";	// DSN XML server
char fetchUrl[50];	// DSN XML fetch URL - random number is appended when used to prevent caching

// Placeholder for XML data
const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
bool usingDummyData = false;			// If true, use dummy data instead of actual data
uint8_t noTargetFoundCounter = 0;		// Keeps track of how many times target is not found
uint8_t noTargetLimit = 3;				// After target is not found this many times, switch to dummy XML data
uint8_t retryDataFetchCounter = 0;		// Keeps track of how many times data fetch failed
uint8_t retryDataFetchLimit = 10;		// After dummy data is used this many times, try to get actual data again
bool dataStarted = false;
WiFiManagerParameter field_color_theme; // global param ( for non blocking w params )
WiFiManagerParameter field_meteor_tail_decay; // global param ( for non blocking w params )
WiFiManagerParameter field_meteor_tail_random; // global param ( for non blocking w params )

uint16_t wmTimout = 120;
unsigned long wmStartTime = millis();
bool portalRunning = false;
bool startAP = false;			// start AP and webserver if true, else start only webserver


// Time is measured in milliseconds and will become a bigger number
// than can be stored in an int, so long is used
unsigned long fpsTimer = 0;
unsigned long perfTimer = 0;
unsigned long perfDiff = 0;
unsigned long lastTime = 0;				// Init reference variable for timer
unsigned long timerDelay = 10000;		// Timer for how often to fetch data
unsigned long animationTimer = 0;
unsigned long craftDelayTimer = 0;
unsigned long craftDelay = 3000;	// Wait this long after finishing for new craft to be dipslayed
unsigned long displayMinDuration = 20000;	// Minimum time to display a craft before switching to next craft
unsigned long displayDurationTimer = 20000;		// Timer to keep track of how long craft has been displayed, set at minimum for no startup delay
const uint8_t meteorOffset = 32;
const uint8_t offsetHalf = meteorOffset * 0.5;


/* DATA */
// Counters to track during XML parsing
unsigned int stationCount = 0;
unsigned int dishCount = 0;
unsigned int targetCount = 0;
unsigned int signalCount = 0;
int parseCounter = 0;

// Holds current craft to be animated
static CraftQueueItem currentCraftBuffer;
static bool nameScrollDone = true;

/* LED HARDWARE CONFIG */
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

/* TEXT UTILITIES */
TextCharacter textCharacter;
const int characterWidth = 4;
const int characterHeight = 7;
const int characterKerning = 3;
int letterSpacing = 7;
int letterTotalPixels = 28;

/* ANIMATION UTILITIES */
// Do not change these

uint8_t fpsInMs = 1000 / FPS;
bool meteorTailDecay = false;
bool meteorTailRandom = false;



/* HARDWARDE */
// Adafruit_NeoPixel neopixel;

// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
// Adafruit_NeoPixel
// 	outer_pixels(outerPixelsTotal, OUTER_PIN, NEO_GRB + NEO_KHZ800),
// 	middle_pixels(middlePixelsTotal, MIDDLE_PIN, NEO_GRB + NEO_KHZ800),
// 	inner_pixels(innerPixelsTotal, INNER_PIN, NEO_GRB + NEO_KHZ800),
// 	bottom_pixels(bottomPixelsTotal, BOTTOM_PIN, NEO_GRB + NEO_KHZ800);

// Adafruit_NeoPixel* allStrips[4] = {
// 	&inner_pixels,	// ID: Green
// 	&middle_pixels, // ID: Red
// 	&outer_pixels,	// ID: Blue
// 	&bottom_pixels, // ID: Purple
// };


CRGB inner_leds[innerPixelsTotal];
CRGB middle_leds[middlePixelsTotal];
CRGB outer_leds[outerPixelsTotal];
CRGB bottom_leds[bottomPixelsTotal];

CRGB* allStrips[4] = {
	inner_leds,	// ID: Green
	middle_leds, // ID: Red
	outer_leds,	// ID: Blue
	bottom_leds, // ID: Purple
};



/* GENERAL UTILITIES */
ColorTheme currentColors;

void setColorTheme(uint8_t colorTheme) {
	switch (colorTheme) {
		case 0:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 127;
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
		default:
			currentColors.letter = mpColors.white.value;
			currentColors.meteor = mpColors.white.value;
			currentColors.tailHue = 160;
			currentColors.tailSaturation = 127;
	}
}

const char* termColor(const char* color) {
	if (color == "black") return "\e[0;30m";
	if (color == "red") return "\e[0;31m";
	if (color == "green") return "\e[32m";
	if (color == "yellow") return "\e[33m";
	if (color == "blue") return "\e[34m";
	if (color == "purple") return "\e[35m";
	if (color == "cyan") return "\e[36m";
	if (color == "white") return "\e[37m";
	if (color == "reset") return "\e[0m";

	return "\e[0m";
}

void printFreeHeap() {
	String printString;

	// printString += "\n";
	// printString += termColor("blue");
	printString += "MEM_Free_Heap:";
	// printString += ESP.getFreeHeap() * 0.01; // Value being divided for visualization on plotter
	printString += ESP.getFreeHeap();	// This is the actual value
	printString += ",";
	// printString += termColor("reset");
	// printString += "\n";

	Serial.print(printString);
}

void handleException() {
	Serial.print(termColor("red"));
	Serial.println("EXCEPTION CAUGHT:");
    try {
        throw;
    } catch (const exception &e) {
        Serial.println(e.what());
    } catch (const int i) {
        Serial.println(i);
    } catch (const long l) {
        Serial.println(l);
    } catch (const char *p) {
        Serial.println(p);
    } catch (...) {
        Serial.println("Exception unknown");
    }
	Serial.println(termColor("reset"));
}

void printMeteorArray() {
		
	String printString = "\n---------------[ METEOR ARRAY ]---------------\n";
	
	for (int i = 0; i < 500; i++) {
		if (animate.ActiveMeteors[i] == nullptr){
			printString += "[";
			printString += termColor("red");
			printString += i;
			printString += " = nul";
			printString += "]";
		}
		else {
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

// Force Wifi portal when WiFi reset button is pressed
void checkWifiButton(){
  // check for button press
  if ( digitalRead(WIFI_RST) == LOW ) {
    // poor mans debounce/press-hold to prevent accidental trigger
    delay(50);
    if( digitalRead(WIFI_RST) == LOW ){
      Serial.println("WiFi Reset button pressed");
      // still holding button for 3000 ms, reset settings
      delay(3000); // reset delay hold
      if( digitalRead(WIFI_RST) == LOW ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
      
      // start portal w delay
      Serial.println("Starting config portal");
    //   wm.setConfigPortalTimeout(120);
	  try {
		if (!wm.startConfigPortal(AP_SSID, AP_PASS)) {
			Serial.println("Failed to connect or hit timeout");
			delay(3000);
			// ESP.restart();
		} else {
			//if you get here you have connected to the WiFi
			Serial.println("Connected to WiFi)");
		}
	  } catch (...) {
		  handleException();
	  }
    }
  }
}
void doWiFiManager(){
  // is auto timeout portal running
  if(portalRunning){
    wm.process(); // do processing

    // check for timeout
    if((millis() - wmStartTime) > (wmTimout * 1000)){
      Serial.println("portaltimeout");
      portalRunning = false;
      if(startAP){
        wm.stopConfigPortal();
      }
      else{
        wm.stopWebPortal();
      } 
   }
  }

  // is configuration portal requested?
  if(digitalRead(WIFI_RST) == LOW && (!portalRunning)) {
    if(startAP){
      Serial.println("Button Pressed, Starting Config Portal");
      wm.setConfigPortalBlocking(false);
      wm.startConfigPortal(AP_SSID, AP_PASS);
    }  
    else{
      Serial.println("Button Pressed, Starting Web Portal");
      wm.startWebPortal();
    }  
    portalRunning = true;
    wmStartTime = millis();
  }
}



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



/* ANIMATION FUNCTIONS */

// Utility function to reverse elements of an array
void reverseStripsArray(void)
{
	reverse(allStrips, allStrips + 4);
}

uint32_t brightnessAdjust(uint32_t color)
{
	au.updateBrightness();
	uint8_t rgb[4];			   // Create array that will hold color channel values
	*(uint32_t *)&rgb = color; // Assigns color value to the color channel array
	uint8_t channelR = rgb[0]; // blue color channel value
	uint8_t channelG = rgb[1]; // Green color channel value
	uint8_t channelB = rgb[2]; // Blue color channel value
	// Serial.println(r);
	// Serial.println(g);
	// Serial.println(b);
	// Serial.println();
	return Adafruit_NeoPixel::Color(((AnimationUtils::brightness * channelR) / 255), ((AnimationUtils::brightness * channelG) / 255), ((AnimationUtils::brightness * channelB) / 255));
}





// Display letter from array
void doLetterRegions(char theLetter, int regionStart, int startingPixel)
{
	const int *ledCharacter = textCharacter.getCharacter(theLetter);
	const uint16_t regionOffset = innerPixelsChunkLength * regionStart;

	int16_t pixel = 0 + startingPixel + regionOffset;

	const int16_t previousPixel = startingPixel + regionOffset - letterSpacing;

	for (int i = 0; i < letterTotalPixels; i++)
	{
		int j = i + 1;						// Add 1 to avoid modulus on 0
		int regionInt = j % characterWidth; // Modulus gives an int for the region to draw to
		if (regionInt == 0)
			regionInt = characterWidth; // Modulus of same int gives zero, so assign to last region int
		--regionInt;					// Decrement to un-offset from original offset for modulus calc

		int drawPixel = pixel + (innerPixelsChunkLength * (regionInt));					// Calculate pixel to draw
		int drawPreviousPixel = previousPixel + (innerPixelsChunkLength * (regionInt)); // Calculate trailing pixel after letter to "draw" off value

		int regionStart = (innerPixelsChunkLength * (regionInt + 1)) - innerPixelsChunkLength + regionOffset; // Calculate the pixel before the region start
		int regionEnd = innerPixelsChunkLength * (regionInt + 1) + regionOffset;								   // Calculate the pixel after the region end

		if (regionStart <= drawPreviousPixel && drawPreviousPixel < regionEnd) {
			// au.setPixelColor(*target, drawPreviousPixel, mpColors.off.pointer);
			inner_leds[drawPreviousPixel] = mpColors.off.value;
		}
		if (regionStart <= drawPixel && drawPixel < regionEnd) {
			// au.setPixelColor(*target, drawPixel, character_array[i]);
			if (ledCharacter[i] == 1)
				inner_leds[drawPixel] = currentColors.letter;
			else
				inner_leds[drawPixel] = mpColors.off.value;

		}

		if (regionInt == characterWidth - 1)
			pixel--; // Move to next pixel
	}
}

// Updates scrolling letters on inner strips
void scrollLetters(const char * spacecraftName, int wordArraySize)
{
	int regionStart = 4;
	static int startPixel = 0;
	int letterPixel = startPixel;

	for (int i = 0; i < wordArraySize; i++)
	{		
		char theLetter = spacecraftName[i];

		doLetterRegions(theLetter, 0, letterPixel);
		doLetterRegions(theLetter, 7, letterPixel);
		// doLetterRegions(theLetter, 8, letterPixel);

		letterPixel = letterPixel - letterSpacing - characterKerning;
	}


	int wrapPixel = innerPixelsChunkLength + (wordArraySize * (characterHeight + characterKerning));
	startPixel++;

	if (startPixel > wrapPixel) {
		startPixel = 0;
		if (millis() - displayDurationTimer > displayMinDuration) {
			nameScrollDone = true;
			craftDelayTimer = millis();
		}
	}
}



// Create Meteor object
void createMeteor(int strip, int region, bool directionDown = true,  int startPixel = 0) {
	CHSV meteorColor = currentColors.meteor;

	for (int i = 0; i < 500; i++) {
		if (animate.ActiveMeteors[i] != nullptr) {
			// Serial.print("Could not create meteor #"); Serial.print(i); Serial.print(" startPixel: "); Serial.println(animate.ActiveMeteors[i]->firstPixel);
			continue;
		}

		animate.ActiveMeteors[i] = new Meteor {
			directionDown,					// directionDown
			startPixel,						// firstPixel
			region,							// region
			(int) outerPixelsChunkLength,	// regionLength
			meteorColor,					// pColor
			1,								// meteorSize
			meteorTailDecay,				// meteorTrailDecay
			meteorTailRandom,				// meteorRandomDecay
			currentColors.tailHue,			// tailHueStart
			true,							// tailHueAdd
			0.75,							// tailHueExponent
			currentColors.tailSaturation,	// tailHueSaturation
			allStrips[strip]				// rStrip
		};

		break;
		
	}
	
	if (animate.ActiveMeteorsSize > 499) animate.ActiveMeteorsSize = 0;
}

void animationMeteorPulseRegion(
	uint8_t strip,
	uint8_t region,
	bool directionDown = true,
	int16_t startPixel = 0,
	uint8_t pulseCount = 2,
	int8_t offset = 10,
	bool randomizeOffset = false)
{

	// Stagger the starting pixel
	if (randomizeOffset == true) startPixel = startPixel - ((rand() % offsetHalf + 1) * 2);

	for (int i = 0; i < pulseCount; i++) {
		int16_t pixel = i + startPixel + (i * offset * -1);
		if (randomizeOffset == true) pixel = pixel - (random(0, 6) * 2);
		createMeteor(strip, region, directionDown, pixel);
	}
}


void animationMeteorPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t pulseCount = 2,
	int8_t offset = 32,
	bool randomizeOffset = false)
{
	for (int i = 0; i < outerChunks; i++) {
		animationMeteorPulseRegion(strip, i, directionDown, 0, pulseCount, offset, randomizeOffset);
	}
}

void doRateBasedAnimation(bool isDown, uint8_t rateClass, uint8_t offset) {
	int stripId = isDown == true ? 2 : 1;

	if (rateClass == 0) {
		// allStrips[stripId]->clear();
		FastLED.clear(allStrips[stripId]);
		return;
	}

	int pulseCount = 1;

	switch(rateClass) {
		case 6:	// 1gbps
			pulseCount = 5;
			// Serial.print("[Downsignal Gb]");
			animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
			break;
		case 5:	// 1mbps
			pulseCount = 4;
			// Serial.print("[Downsignal kb]");
			animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
			break;
		case 4: // 10lbps
			pulseCount = 3;
			// Serial.print("[Downsignal kb]");
			animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
			break;
		case 3:
			pulseCount = 2;
			// Serial.print("[Downsignal kb]");
			animationMeteorPulseRing(stripId, isDown, pulseCount, offset, true);
			break;
		case 2:
			pulseCount = 1;
			// Serial.print("[Downsignal kb]");
			animationMeteorPulseRing(stripId, isDown, pulseCount, 16, true);
			break;
		case 1:
			// Serial.print("[Downsignal slow]");
			animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, true);
			animationMeteorPulseRegion(stripId, random(10), isDown, 0, pulseCount, 12, true);
			break;
		case 0:
			// allStrips[stripId]->clear();
			FastLED.clear(allStrips[stripId]);
			// Serial.print("[Downsignal -- ]");
			break;
		default:
			// allStrips[stripId]->clear();
			FastLED.clear(allStrips[stripId]);
			// Serial.print("[Downsignal n/a ]");			 
	}
}

void drawMeteors() {
	// Update meteor animations
	for (int i = 0; i < 100; i++) {	
		if (animate.ActiveMeteors[i] != nullptr) animate.animateMeteor(animate.ActiveMeteors[i]);
	}
}

void updateMeteors() {
	// Update first pixel location for all active Meteors in array
	for (int i = 0; i < 500; i++) {
		if (animate.ActiveMeteors[i] != nullptr){
			animate.ActiveMeteors[i]->firstPixel = animate.ActiveMeteors[i]->firstPixel + 2;

			// If meteor is beyond the display region, unallocate memory and remove array item
			if (animate.ActiveMeteors[i]->firstPixel > animate.ActiveMeteors[i]->regionLength * 2) {
				delete animate.ActiveMeteors[i];
				animate.ActiveMeteors[i] = nullptr;
			}
		}
	}
}

/** Main Animation update function
 * Gets called every loop();
 */
void updateAnimation(const char* spacecraftName, int spacecraftNameSize, int downSignalRate, int upSignalRate)
{
	// Serial.print("FPS: "); Serial.println(FastLED.getFPS());


	// Update brightness from potentiometer
	au.updateBrightness();

	/* Update Scrolling letters animation */
	if (nameScrollDone == false) {
		try {
			scrollLetters(spacecraftName, spacecraftNameSize);
		} catch (...) {
			Serial.println("Error in scrollLetters()");
		}
	}

	// Fire meteors
	if (displayDurationTimer > 6000 && (millis() - animationTimer) > 3000) {
		// printMeteorArray();

		if (nameScrollDone == false) {
			try {				
				doRateBasedAnimation(true, downSignalRate, meteorOffset);
				doRateBasedAnimation(false, upSignalRate, meteorOffset);				
			} catch (...) {
				Serial.println("Error in signal animation");
			}
		}

		animationTimer = millis(); // Reset meteor animation timer
	}

	drawMeteors(); // Assign new pixels for meteors
	FastLED.delay(1000/FPS);
	updateMeteors(); // Update first pixel location for all active Meteors in array

	FastLED.countFPS();
}

unsigned int rateLongToRateClass(unsigned long rate) {
	if (rate > (1024 * 1000000)) {
		return 6;						// > 1gbps
	} else if (rate > (1024 * 1024)) {
		return 5;						// > 1mbps
	} else if (rate > (1024 * 100)) {
		return 4;						// > 100kbps
	} else if (rate > (1024 * 5)) {
		return 3;						// > 5kbps
	} else if (rate > 1024) {
		return 2;						// > 1kbps
	} else if (rate > 0) {
		return 1;						// > < 1kbps
	} else {
		return 0;						// 0
	}
}

void parseData(const char* payload) {
	if (DIAG_MEASURE == 1) {
		// Serial.print("parseData() top of func: ");
		// printFreeHeap();
	}

	// XML Parsing
	XMLDocument xmlDocument; // Create variable for XML document

	const char * charPayload = payload;

	// Handle XML parsing error	
	try {
		if (xmlDocument.Parse(charPayload) == XML_SUCCESS) {
			if (SHOW_SERIAL == 1) {
				Serial.print(termColor("green"));
				Serial.print("XML Parsed Succcessfully");
				Serial.println(termColor("reset"));
			}
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


	// Find XML elements
	XMLNode *root = xmlDocument.RootElement();					  // Find document root node
	XMLElement *timestamp = root->FirstChildElement("timestamp"); // Find XML timestamp element

	CraftQueueItem* newCraft = new CraftQueueItem;
	


	for (int i = 0; i < 100; i++) {		// Arbitray loop number to prevent infinite loop, but ensure we try to parse all elements
		if (SHOW_SERIAL == 1) {
			Serial.print("->>> Parse loop: ");
			Serial.println(i);
			// print stationcount, dishcount, targetcount
			Serial.println("");
			Serial.print("stationCount: "); Serial.println(stationCount);
			Serial.print("dishCount: "); Serial.println(dishCount);
			Serial.print("targetCount: "); Serial.println(targetCount);
			Serial.println("");
		}

		int s = 0; // Create station elements counter
		for (XMLElement *xmlStation = root->FirstChildElement("station"); xmlStation != NULL; xmlStation = xmlStation->NextSiblingElement("station")) {	
			if (s > 2) s = 0;
			if (s != stationCount) {
				s++;				
				continue;
			}

			int d = 0; // Create dish elements counter
			for (XMLElement *xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement()) {
				if (d > 9) {
					stationCount++;
					if (stationCount > 2) {
						stationCount = 0;
					}
					d = 0;
				}
				if (d != dishCount) {
					d++;
					continue;
				}

				int t = 0; // Create target elements counter
				for (XMLElement *xmlTarget = xmlDish->FirstChildElement("target"); xmlTarget != NULL; xmlTarget = xmlTarget->NextSiblingElement("target")) {
					if (t > 9) {
						t = 0;
					}
					if (t != targetCount) {
						
						t++;
						continue;
					}
					const char* target = xmlTarget->Attribute("name");
					if (data.checkBlacklist(target) == true) {
						t++;
						continue;
					}

					try {
						strlcpy(newCraft->callsignArray, target, 10);
					} catch (...) {
						if (SHOW_SERIAL == 1) {
							Serial.print(termColor("red"));
							Serial.println("Problem copying callsign to newCraft->callsignArray");
							Serial.println(termColor("reset"));
						}
						handleException();
						return;
					}

					const char* name = data.callsignToName(target);

					if (SHOW_SERIAL == 1) {
						// print callsign
						Serial.print(termColor("yellow"));
						Serial.print("PARSED callsign: "); Serial.println(newCraft->callsign);
						Serial.println(termColor("reset"));
						
						// print name	
						Serial.print(termColor("yellow"));
						Serial.print("PARSED name: "); Serial.println(name);
						Serial.println(termColor("reset"));
					}

					try {
						strlcpy(newCraft->nameArray, name, 100);
					} catch (...) {
						if (SHOW_SERIAL == 1) {
							Serial.print(termColor("red"));
							Serial.println("Problem copying name to newCraft->nameArray");
							Serial.println(termColor("reset"));
						}
						handleException();
						return;
					}
					newCraft->nameLength = strlen(name);
					break;
				}

				if (newCraft->nameLength < 1) break;	// Bail if we didn't find a target

				// Loop through XML signal elements and find the one that matches the target
				for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("downSignal")) {
					const char* spacecraft = xmlSignal->Attribute("spacecraft");
					const char* signalType = xmlSignal->Attribute("signalType");
					
					if (SHOW_SERIAL == 1) {
						// Serial.print("signalType: "); Serial.println(signalType);
						Serial.print("looking for "); Serial.println(newCraft->callsign);
					}

					try {
						if (strcmp(signalType, "data") != 0) {
							// Serial.println("Not a data signal, skipping...");
							continue;
						}
						if (strcmp(spacecraft, newCraft->callsign) != 0) {
							// Serial.print(spacecraft); Serial.print(" != "); Serial.println(newCraft->callsign);
							// Serial.println("Not the right spacecraft, skipping...");
							continue;
						}
					}
					catch (...) {
						if (SHOW_SERIAL == 1) {
							Serial.print(termColor("red"));
							Serial.println("Problem parsing payload:");
							Serial.println(termColor("reset"));
							handleException();
						}
						return;
					}

					if (SHOW_SERIAL == 1) Serial.println("---");

					const char* downRate = xmlSignal->Attribute("dataRate");
					
					if (SHOW_SERIAL == 1) {
						// print rate
						Serial.print(termColor("yellow"));
						Serial.print("PARSED down rate: "); Serial.println(downRate);
						Serial.println(termColor("reset"));
					}

					unsigned long downRateLong = stol(downRate, nullptr, 10);
					if (downRateLong == 0) continue;
					unsigned int downRateClass = rateLongToRateClass(downRateLong);
					if (downRateClass == 0) continue;
					newCraft->downSignal = downRateClass;
					break;
				}

				for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("upSignal"))
				{
					const char* spacecraft = xmlSignal->Attribute("spacecraft");
					const char* signalType = xmlSignal->Attribute("signalType");
					// Serial.print("signalType: "); Serial.println(signalType);


					try {
						if (strcmp(xmlSignal->Attribute("signalType"), "data") != 0) {
							// Serial.println("Not a data signal, skipping...");
							continue;
						}
						if (strcmp(spacecraft, newCraft->callsign) != 0) {
							// Serial.print(spacecraft); Serial.print(" != "); Serial.println(newCraft->callsign);
							// Serial.println("Not the right spacecraft, skipping...");
							continue;
						}
					}
					catch (...) {
						if (SHOW_SERIAL == 1) {
							Serial.print(termColor("red"));
							Serial.println("Problem parsing payload:");
							Serial.println(termColor("reset"));
						}
						handleException();
						return;
					}

					if (SHOW_SERIAL == 1) Serial.println("---");
					
					const char* upRate = xmlSignal->Attribute("dataRate");

					if (SHOW_SERIAL == 1) {
						// print rate
						Serial.print(termColor("yellow"));
						Serial.print("PARSED up rate: "); Serial.println(upRate);
						Serial.println(termColor("reset"));
					}

					unsigned long upRateLong = stol(upRate, nullptr, 10);
					if (upRateLong == 0) continue;
					unsigned int upRateClass = rateLongToRateClass(upRateLong);
					if (upRateClass == 0) continue;
					newCraft->upSignal = upRateClass;
					break;
				}


				break;
			}
			break;
		}

		if (newCraft->name != 0 && (newCraft->downSignal != 0 || newCraft->upSignal != 0)) {
			if (SHOW_SERIAL == 1) {
				Serial.println("---------- found craft ----------");
				Serial.println(newCraft->name);
				Serial.println(newCraft->callsign);
				Serial.println(newCraft->downSignal);
				Serial.println(newCraft->upSignal);
			}
			break;
		} else {
			// Serial.println("---------- no craft found ----------");
			targetCount++;
			if (targetCount > 9) {
				targetCount = 0;
				dishCount++;
			}
			if (dishCount > 9) {
				dishCount = 0;
				stationCount++;
			}
			if (stationCount > 2) {
				stationCount = 0;
			}
			parseCounter++;
		}
	}

	if (newCraft->name != 0 && (newCraft->downSignal != 0 || newCraft->upSignal != 0)) {

		if (SHOW_SERIAL == 1) {
			Serial.println();
			Serial.println("---------- sending to queue ----------");
			Serial.print("NEW FETCH callsign: ");
			Serial.println(newCraft->callsign);
			Serial.print("NEW FETCH name: ");
			Serial.println(newCraft->name);
			Serial.print("NEW FETCH nameLength: ");
			Serial.println(newCraft->nameLength);
			Serial.print("NEW FETCH downSignal: ");
			Serial.println(newCraft->downSignal);
			Serial.print("NEW FETCH upSignal: ");
			Serial.println(newCraft->upSignal);
			Serial.println();
		}


		// Add data to queue, to be passed to another task
		if (xQueueSend(queue, newCraft, portMAX_DELAY)) {
			if (SHOW_SERIAL == 1) {
				Serial.print(termColor("green"));
				Serial.print("Added to queue");
				Serial.println(termColor("reset"));
			}
		} else {
			delete newCraft;

			if (SHOW_SERIAL == 1) {
				Serial.println(termColor("red"));
				Serial.print("Failed to add to queue");
				Serial.println(termColor("reset"));
			}
		}

		// Increment target counters
		targetCount++;
		if (targetCount > 9) {
			targetCount = 0;
			dishCount++;
		}
		if (dishCount > 9) {
			dishCount = 0;
			stationCount++;
		}
		if (stationCount > 2) {
			stationCount = 0;
		}
	}

	return;
}


void fetchData() {

	if (TEST_CORES == 1)
		{
		Serial.print("getData() running on core ");
		Serial.println(xPortGetCoreID());
	}

	// Serial.print("WiFi Status: ");
	// Serial.println(WiFi.status());

	uint16_t httpResponseCode;

	// Check WiFi connection status
	if (WiFi.status() != WL_CONNECTED) {
		if (SHOW_SERIAL == 1) Serial.println("WiFi Disconnected");

		// WiFi.begin(ssid, password);
		// Serial.println("Connecting");
		// // while(WiFi.status() != WL_CONNECTED) {
		// //   Serial.print(".");
		// // }
		// Serial.println("");
		// Serial.print("Connected to WiFi network with IP Address: ");
		// Serial.println(WiFi.localIP());

		usingDummyData = true;
	}

	if (usingDummyData == false) {
	
		try {
			memset(fetchUrl, 0, sizeof(fetchUrl));		// set fetchUrl to empty
			strcpy(fetchUrl, serverName);				// copy serverName to fetchUrl
			char randBuffer[9];							// buffer for random number cache buster
			ltoa(random(999999999), randBuffer, 10);	// convert random number to string
			strcat(fetchUrl, randBuffer);				// append random number to fetchUrl
		} catch (...) {
			handleException();
		}

		if (SHOW_SERIAL == 1) {
			// print fetchUrl
			Serial.print(termColor("purple"));
			Serial.print("fetchUrl: ");
			Serial.println(fetchUrl);


			Serial.println(termColor("reset"));
		}

		// Use WiFiClient class to create TCP connections
		if (!http.begin(fetchUrl)) {
			if (SHOW_SERIAL == 1) {
				Serial.println("Failed to connect to server");
			}
			return;
		}

		// Send HTTP GET request
		http.setTimeout(10000);
		http.addHeader("Content-Type", "text/xml");
		httpResponseCode = http.GET();
	}

	if (SHOW_SERIAL == 1) {
		Serial.print("Using dummy data? ");

		if (usingDummyData == true) {
			Serial.println("TRUE");
		} else {
			Serial.println("FALSE");
		}
	}

	if (usingDummyData == true) {
		if (SHOW_SERIAL == 1) {
			Serial.print(termColor("purple"));
			Serial.print("Using dummy xml data!!");
			Serial.println(termColor("reset"));
		}

		parseData(dummyXmlData);
	}

	if (usingDummyData == false && noTargetFoundCounter > noTargetLimit) {
		Serial.print(termColor("purple"));
		Serial.print("Target not found limit reach - using dummy xml data");
		Serial.println(termColor("reset"));
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

				if (SHOW_SERIAL == 1) Serial.println("HTTP response received");
				usingDummyData = false;
				parseData(charRes);
			} catch (...) {
				handleException();
			}
		}
	}

	http.end(); // Free up resources
	if (dataStarted == false) dataStarted = true;

	return;
}

// Fetch XML data from HTTP & parse for use in animations
void getData(void *parameter) 
{
	// UBaseType_t uxHighWaterMark;
	// uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	// Serial.print("high_water_mark:"); Serial.print(uxHighWaterMark); Serial.print(",");

	for (;;)
	{
		// Send an HTTP POST request every 5 seconds
		if ((millis() - lastTime) > timerDelay) {
			if (uxQueueSpacesAvailable( queue ) > 0) {
				fetchData();
				lastTime = millis(); // Sync reference variable for timer
			}
		}
	}
}




String getParam(String name){
	//read parameter from server, for customhmtl input
	if (wm.server->hasArg(name) == 0) {
		return "0";
	}

	String value = wm.server->arg(name);
	return value;
}

void saveColorThemeCallback() {
	try {
		String colorTheme = getParam("customfieldid");
		const int colorThemeId = atoi(colorTheme.c_str());

		Serial.println("[CALLBACK] saveColorThemeCallback fired");
		Serial.print("PARAM customfieldid = "); Serial.println(colorTheme);

		setColorTheme(colorThemeId);
	} catch (...) {
		handleException();
	}	
}


// setup() function -- runs once at startup --------------------------------
void setup()
{
	Serial.begin(115200); // Begin serial communications, ESP32 uses 115200 rate

	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
	Serial.println("Starting...");

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	//wm.resetSettings();

	#if SHOW_SERIAL == 0
		Serial.setDebugOutput(false);
		wm.setDebugOutput(false);
	#endif


	// Set up pins
	pinMode(OUTER_PIN, OUTPUT);
	pinMode(INNER_PIN, OUTPUT);
	pinMode(MIDDLE_PIN, OUTPUT);
	pinMode(BOTTOM_PIN, OUTPUT);
	pinMode(OUTPUT_ENABLE, OUTPUT);
	pinMode(WIFI_RST, INPUT);
	pinMode(POTENTIOMETER, INPUT);
	digitalWrite(OUTPUT_ENABLE, HIGH);

	// Initialize NeoPixel objects
	// for (int i = 0; i < allStripsLength; i++)
	// {
	// 	allStrips[i]->begin();
	// }

	// // Set brightness
	// for (int i = 0; i < allStripsLength; i++)
	// {
	// 	allStrips[i]->setBrightness(BRIGHTNESS);
	// }
	FastLED.setBrightness(BRIGHTNESS);

	// allStripsOff();	// Turn off all NeoPixels
	
	// // Identify Neopixel strips by filling with unique colors
	// if (ID_LEDS == 1)
	// {
	// 	#if SHOW_SERIAL == 1
	// 		Serial.println("ID LED strips");
	// 	#endif

	// 	au.updateBrightness(); // Update brightness from potentiometer

	// 	// const uint32_t *colors[] = {mpColors.red.pointer, mpColors.green.pointer, mpColors.blue.pointer, mpColors.purple.pointer, mpColors.white.pointer};

	// 	// for (int c = 0; c < sizeof(colors) / sizeof(colors[0]); c++)
	// 	// {
	// 	// 	for (int i = 0; i < allStripsLength; i++)
	// 	// 	{
	// 	// 		allStrips[i]->fill(*colors[c]);
	// 	// 	}
	// 	// 	allStripsShow();
	// 	// 	delay(5000);
	// 	// }

	// 	// outer_pixels.fill(*mpColors.blue.pointer);
	// 	// // bottom_pixels.fill(*mpColors.purple.pointer);
	// 	// outer_pixels.show();
	// 	// bottom_pixels.show();
		
	// 	allStrips[0]->fill(*mpColors.red.pointer);
	// 	allStrips[1]->fill(*mpColors.red.pointer);
	// 	allStrips[2]->fill(*mpColors.red.pointer);
	// 	allStripsShow();
	// 	delay(5000);

	// 	allStrips[0]->fill(*mpColors.green.pointer);
	// 	allStrips[1]->fill(*mpColors.green.pointer);
	// 	allStrips[2]->fill(*mpColors.green.pointer);
	// 	allStripsShow();
	// 	delay(5000);

	// 	allStrips[0]->fill(*mpColors.blue.pointer);
	// 	allStrips[1]->fill(*mpColors.blue.pointer);
	// 	allStrips[2]->fill(*mpColors.blue.pointer);
	// 	allStripsShow();
	// 	delay(5000);


		
	// 	// allStrips[0]->fill(*mpColors.red.pointer);
	// 	// allStrips[1]->fill(*mpColors.green.pointer);
	// 	// allStrips[2]->fill(*mpColors.blue.pointer);
	// 	// allStrips[3]->fill(*mpColors.purple.pointer);
	// 	// allStripsShow();
	// 	// delay(10000);
	// 	// allStripsOff();
	// 	// delay(3000);

	// 	// hueCycle(*allStrips[0], 10);
	// 	// allStripsOff();
	// 	// hueCycle(*allStrips[1], 10);
	// 	// allStripsOff();
	// 	// hueCycle(*allStrips[2], 10);
	// 	// allStripsOff();
	// 	// hueCycle(*allStrips[3], 10);
	// 	// allStripsOff();
	// 	// delay(3000);

	// 	// colorWipe(*allStrips[0], mpColors.red.pointer, 10);
	// 	// colorWipe(*allStrips[1], mpColors.green.pointer, 10);
	// 	// colorWipe(*allStrips[2], mpColors.blue.pointer, 10);
	// 	// colorWipe(*allStrips[3], mpColors.yellow.pointer, 10);

	// 	// allStripsShow();
	// 	// delay(10000);
	// 	// rainbow(*allStrips[0], 10);             // Flowing rainbow cycle along the whole strip
	// 	// rainbow(*allStrips[1], 10);             // Flowing rainbow cycle along the whole strip
	// 	// rainbow(*allStrips[2], 10);             // Flowing rainbow cycle along the whole strip
	// 	// rainbow(*allStrips[3], 10);             // Flowing rainbow cycle along the whole strip

	// 	allStripsOff();
	// }


	/* Fast LED */
	FastLED.addLeds<NEOPIXEL, OUTER_PIN>(outer_leds, outerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, MIDDLE_PIN>(middle_leds, middlePixelsTotal);
	FastLED.addLeds<NEOPIXEL, INNER_PIN>(inner_leds, innerPixelsTotal);
	FastLED.addLeds<NEOPIXEL, BOTTOM_PIN>(bottom_leds, bottomPixelsTotal);

	allStripsOff();

	

	// CHSV* stuff = allStrips[2];

	// for(int dot = 0; dot < outerPixelsTotal; dot++) { 
	// 	stuff[dot] = CHSV::Purple;
	// 	FastLED.show();
	// 	// clear this led for the next time around the loop
	// 	if (dot > 0) stuff[dot - 1] = CHSV::Black;
	// 	// delay(30);
	// }



	wm.setConfigPortalBlocking(false);
	// test custom html(radio)
	const char* custom_radio_str = "<br/><label for='customfieldid'>Color Theme</label><br/><input type='radio' name='customfieldid' value='0' checked> Snow<br><input type='radio' name='customfieldid' value='1'> Cyber<br><input type='radio' name='customfieldid' value='2'> Valentine";
	const char* meteor_decay_checkbox_str = "<br/><label for='meteorDecay'>Meteor Decay</label><br/><input type='checkbox' name='meteorDecay' value='0'>";
	const char* meteor_random_checkbox_str = "<br/><label for='meteorRandom'>Meteor Tail</label><br/><input type='checkbox' name='meteorRandom' value='0'>";
	new (&field_color_theme) WiFiManagerParameter(custom_radio_str); // custom html input
	new (&field_meteor_tail_decay) WiFiManagerParameter(meteor_decay_checkbox_str); // custom html input
	new (&field_meteor_tail_random) WiFiManagerParameter(meteor_random_checkbox_str); // custom html input

	wm.addParameter(&field_color_theme);
	wm.addParameter(&field_meteor_tail_decay);
	wm.addParameter(&field_meteor_tail_random);
	wm.setSaveParamsCallback(saveColorThemeCallback);
	 std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
	wm.setMenu(menu);

	// set dark theme
	wm.setClass("invert");


	wm.setCleanConnect(true);
	wm.setConnectRetries(5);
	wm.setConnectTimeout(10); // connect attempt fails after n seconds
	
	// wm.setSaveConnectTimeout(5);
	wm.setConfigPortalTimeout(120);
	
	// wm.startConfigPortal(AP_SSID, AP_PASS);

	// Automatically connect using saved credentials,
	// if connection fails, it starts an access point with the specified name ( AP_SSID ),
	// if AP_PASSWORD is empty it will be a non-protected AP
	// then goes into a blocking loop awaiting configuration and will return success result

	Serial.println("Saved WiFi credentials? "); Serial.println(wm.getWiFiIsSaved());
	if (wm.getWiFiIsSaved() == true) {
		bool res;
		res = wm.autoConnect(AP_SSID, AP_PASS); // non-password protected ap

		if(!res) {
			Serial.println("Failed to connect");
			// ESP.restart();
			// wm.startConfigPortal(AP_SSID, AP_PASS);
		} 
		else {
			//if you get here you have connected to the WiFi    
			Serial.println("Connected to WiFi");
		}
	} else {
		wm.startConfigPortal(AP_SSID, AP_PASS);
	    wm.startWebPortal();
		portalRunning = true;
		wmStartTime = millis();
	}


	if (TEST_CORES == 1)
	{
		Serial.print("setup() running on core ");
		Serial.println(xPortGetCoreID());
	}







	// Initialize task for core 1
	xTaskCreatePinnedToCore(
		getData,	 /* Function to implement the task */
		"getData",	 /* Name of the task */
		4096,		 /* Stack size in words */
		NULL,		 /* Task input parameter */
		0,			 /* Priority of the task */
		&xHandleData, /* Task handle. */
		0);			 /* Core where the task should run */

	// Create queue to pass data between tasks on separate cores
	queue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create queue

	if (queue == NULL)
	{ // Check that queue was created successfully
		#if SHOW_SERIAL == 1
			Serial.println("Error creating the queue");
			delay(3000);
		#endif
		ESP.restart();
	}

	http.setReuse(true);

	data.loadJson();



	setColorTheme(colorTheme);
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop()
{
	if (TEST_CORES == 1)
	{
		if (millis() - lastTime > 4000 && millis() - lastTime < 4500)
		{
			Serial.print("loop() running on core ");
			Serial.println(xPortGetCoreID());
		}
	}

	try {
		doWiFiManager();
		
	} catch (...) {
		handleException();
	}
	checkWifiButton();

	if ((dataStarted == true && nameScrollDone == true && millis() - displayDurationTimer > displayMinDuration && millis() - craftDelayTimer > craftDelay)) {
		CraftQueueItem infoBuffer;	// Create buffer to hold data from queue
		CraftQueueItem* pInfoBuffer;
		
		// print all items in queueu without removing them
		// Serial.print(termColor("purple"));
		// Serial.print("------- QUEUE ----------");
		// Serial.println(termColor("reset"));
		// for (int i = 0; i < uxQueueMessagesWaiting(queue); i++){
		// 	xQueuePeek(queue, &infoBuffer, i);
		// 	Serial.print("Queue item "); Serial.print(i); Serial.println("");
		// 	Serial.print("callsign: "); Serial.println(infoBuffer.callsign);
		// 	Serial.print("name: "); Serial.println(infoBuffer.name);
		// 	Serial.print("nameLength: "); Serial.println(infoBuffer.nameLength);
		// 	Serial.print("downSignal: "); Serial.println(infoBuffer.downSignal);
		// 	Serial.print("upSignal: "); Serial.println(infoBuffer.upSignal);
		// 	printFreeHeap();
		// }

		// Receive from queue (data task on core 1)
		if (xQueueReceive(queue, &infoBuffer, 1) == pdPASS)
		{
			try {
				if (SHOW_SERIAL == 1) {
					// print all values of currentCraftBuffer
					Serial.println("------- DATA BUFFER ----------");
					Serial.print("callsign: "); Serial.println(currentCraftBuffer.callsign);
					Serial.print("name: "); Serial.println(currentCraftBuffer.name);
					Serial.print("nameLength: "); Serial.println(currentCraftBuffer.nameLength);
					Serial.print("downSignal: "); Serial.println(currentCraftBuffer.downSignal);
					Serial.print("upSignal: "); Serial.println(currentCraftBuffer.upSignal); 
					printFreeHeap();
				}
			} catch (...) {
				if (SHOW_SERIAL == 1) Serial.println("Error printing currentCraftBuffer");
				handleException();
			}

			try {
				if (SHOW_SERIAL == 1) {
					// print received queue item
					Serial.println("------- DATA RECEIVED ----------");
					Serial.print("callsign: "); Serial.println(infoBuffer.callsign);
					Serial.print("name: "); Serial.println(infoBuffer.name);
					Serial.print("nameLength: "); Serial.println(infoBuffer.nameLength);
					Serial.print("downSignal: "); Serial.println(infoBuffer.downSignal);
					Serial.print("upSignal: "); Serial.println(infoBuffer.upSignal);
				}
			} catch (...) {
				if (SHOW_SERIAL == 1) Serial.println("Error printing infoBuffer");
				handleException();
			}

			try {
				if (infoBuffer.name != 0 && strlen(infoBuffer.name) > 0 ) {
					try {
						// memset(&currentCraftBuffer, 0, sizeof(struct CraftQueueItem));

						memcpy(&currentCraftBuffer, &infoBuffer, sizeof(struct CraftQueueItem));

						nameScrollDone = false;

						try {
							if (SHOW_SERIAL == 1) {
								// print all values of currentCraftBuffer
								Serial.println("------- DATA BUFFER 2 ----------");
								Serial.print("callsign: "); Serial.println(currentCraftBuffer.callsign);
								Serial.print("name: "); Serial.println(currentCraftBuffer.name);
								Serial.print("nameLength: "); Serial.println(currentCraftBuffer.nameLength);
								Serial.print("downSignal: "); Serial.println(currentCraftBuffer.downSignal);
								Serial.print("upSignal: "); Serial.println(currentCraftBuffer.upSignal);
								printFreeHeap();
							}
						} catch (...) {
							if (SHOW_SERIAL == 1) Serial.println("Error printing currentCraftBuffer");
							handleException();
						}

					} catch(...) {
						if (SHOW_SERIAL == 1) Serial.println("Error copying data from queue to buffer");
						handleException();
					}
				} else {
					if (SHOW_SERIAL == 1) Serial.println("Callsign is empty");
					noTargetFoundCounter++;
				}
			} catch(...) {
				if (SHOW_SERIAL == 1) Serial.println("Error copying data from queue to buffer");
				handleException();
			}

			delete pInfoBuffer;

			displayDurationTimer = millis();

		} else {
			if (SHOW_SERIAL == 1) {
				Serial.print(termColor("red"));
				Serial.println("No data received");
				Serial.print(termColor("reset"));
			}
		}
	}

	try {
		updateAnimation(currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	} catch(...) {
	if (SHOW_SERIAL == 1) {
		Serial.println("Error updating animation");
	}
	handleException();
}

	
	#if DIAG_MEASURE == 1
		// Serial.print("Duration:"); Serial.print(millis() - displayDurationTimer); Serial.print(",");
		// Serial.print("Delay:"); Serial.print(millis() - craftDelayTimer); Serial.print(",");
		// perfDiff = (millis() - perfTimer) * 10;	// Multiplied by 10 for ease of visualization on plotter
		perfDiff = (millis() - perfTimer);	// This is the actual value
		UBaseType_t uxHighWaterMark;
		uxHighWaterMark = uxTaskGetStackHighWaterMark( xHandleData );
		printFreeHeap();	// Value might be being multiplied in printFreeHeap() function for ease of visualization on plotter
		Serial.print("high_water_mark:"); Serial.print(uxHighWaterMark); Serial.print(",");
		Serial.print("PerfTimer:"); Serial.print(perfDiff); Serial.print(",");
		// Serial.print("QueueSize:"); Serial.print(uxQueueMessagesWaiting(queue) * 1000); Serial.print(",");
		// Serial.print("ParseCounter:"); Serial.print(parseCounter * 1000); Serial.print(",");	// Multiplied by 10 for ease of visualization on plotter
		// Serial.print("ParseCounter:"); Serial.print(parseCounter); Serial.print(",");	// This is the actual value
		Serial.println();
		parseCounter = 0;
		perfTimer = millis();
	#endif
}
