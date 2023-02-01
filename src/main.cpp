/* LIBRARIES */
#include <Arduino.h>			// Arduino core
#include <HTTPClient.h>			// HTTP client
#include <Adafruit_NeoPixel.h>	// NeoPixel lib
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

/* CONFIG */
#define AP_SSID "MiniPulse"		// WiFi AP SSID
#define AP_PASS ""		// WiFi AP password
#define OUTER_PIN 17			// Outer ring pin
#define MIDDLE_PIN 18			// Middle ring pin
#define INNER_PIN 19			// Inner ring pin
#define BOTTOM_PIN 16			// Bottom ring pin
#define WIFI_RST 21				// WiFi reset pin
#define OUTPUT_ENABLE 22		// Output enable pin
#define BRIGHTNESS 16			// Global brightness value. 8bit, 0-255
#define POTENTIOMETER 32		// Brightness potentiometer pin

// Diagnostic utilities, all 0 is normal operation
#define TEST_CORES 0			// Test cores
#define SHOW_SERIAL 0			// Show serial output
#define ID_LEDS 0				// ID LEDs
#define DISABLE_WIFI 0			// Disable WiFi

AnimationUtils au(POTENTIOMETER);	// Instantiate animation utils
AnimationUtils::Colors mpColors;	// Instantiate colors
Animate animate;					// Instantiate animate
SpacecraftData data;				// Instantiate spacecraft data

const char* serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml?r=";	// DSN XML server
char fetchUrl[45];	// DSN XML fetch URL - random number is appended when used to prevent caching

// Placeholder for XML data
const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
bool usingDummyData = false;					// If true, use dummy data instead of actual data
uint8_t noTargetFoundCounter = 0;					// Keeps track of how many times target is not found
uint8_t noTargetLimit = 3;							// After target is not found this many times, switch to dummy XML data
uint8_t retryDataFetchCounter = 0;					// Keeps track of how many times data fetch failed
uint8_t retryDataFetchLimit = 10;					// After dummy data is used this many times, try to get actual data again
bool dataStarted = false;

// Time is measured in milliseconds and will become a bigger number
// than can be stored in an int, so long is used
unsigned long lastTime = 0;				// Init reference variable for timer
unsigned long wordLastTime = 0;
unsigned long timerDelay = 5000;		// Set timer to 5 seconds (5000)
unsigned long animationTimer = 0;
unsigned long targetChangeTimer = 0;
unsigned long meteorsTimer = 0;
unsigned long tick = 0;
unsigned long tickAfter = 0;

// how often each pattern updates
unsigned long wordScrollInterval = 0;
unsigned long pattern1Interval = 500;
unsigned long pattern2Interval = 500;
unsigned long pattern3Interval = 500;
unsigned long pattern4Interval = 500;
unsigned long targetChangeInterval = 5000;


/* TASKS */
TaskHandle_t HandleData; // Task for fetching and parsing data
QueueHandle_t queue;	 // Queue to pass data between tasks

/* NETWORKING */
WiFiManager wm;			// Used for connecting to WiFi
HTTPClient http;		// Used for fetching data


/* Data */
unsigned int stationCount = 0;
unsigned int dishCount = 0;
unsigned int targetCount = 0;
unsigned int signalCount = 0;


// Struct to pass data via queue
// struct craftDisplay {
// 	const char* callsign;
// 	const char* name;
// 	uint nameLength;
// 	uint upSignal;
// 	uint downSignal;
// };

// Holds current craft to be animated
static CraftQueueItem currentCraftBuffer ;
static bool nameScrollDone = true;



/* LEDs Config */
const int outerPixelsTotal = 800;
const int middlePixelsTotal = 800;
const int innerPixelsTotal = 960;
const int bottomPixelsTotal = 5;

const int outerChunks = 10;
const int middleChunks = 10;
const int innerChunks = 12;
const int bottomChunks = 1;

const int innerPixelsChunkLength = innerPixelsTotal / innerChunks;
const int outerPixelsChunkLength = outerPixelsTotal / outerChunks;

/* Text Utilities */
TextCharacter textCharacter;
const int characterWidth = 4;
const int characterHeight = 7;
const int characterKerning = 3;
int letterSpacing = 7;
int letterTotalPixels = 28;

/* ANIMATION UTILITIES */
// Do not change these

// Init reference variables for when last update occurred
unsigned long lastUpdateP1 = 0;
unsigned long lastUpdateP2 = 0;
unsigned long lastUpdateP3 = 0;
unsigned long lastUpdateP4 = 0;

// Init state variables for patterns, tracks which pixel to animate
int p1State = 0;
int p1StateExtend = 0;
int p2State = 0;
int p3State = 0;
int p4State = 0;



/* HARDWARDE */
Adafruit_NeoPixel neopixel;

// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
Adafruit_NeoPixel
	outer_pixels(outerPixelsTotal, OUTER_PIN, NEO_GRB + NEO_KHZ800),
	middle_pixels(middlePixelsTotal, MIDDLE_PIN, NEO_GRB + NEO_KHZ800),
	inner_pixels(innerPixelsTotal, INNER_PIN, NEO_GRB + NEO_KHZ800),
	bottom_pixels(bottomPixelsTotal, BOTTOM_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel* allStrips[4] = {
	&inner_pixels,	// ID: Green
	&middle_pixels, // ID: Red
	&outer_pixels,	// ID: Blue
	&bottom_pixels, // ID: Purple
};



/* GENERAL UTILITIES */
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

	printString += "\n";
	printString += termColor("blue");
	printString += "MEM Free Heap: ";
	printString += ESP.getFreeHeap();
	printString += ",";
	printString += termColor("reset");
	printString += "\n";

	Serial.println(printString);
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
        Serial.println("nope, sorry, I really have no clue what that is");
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



int allStripsLength = sizeof(allStrips) / sizeof(allStrips[0]);
void allStripsShow(void)
{
	for (int i = 0; i < 4; i++)
	{
		allStrips[i]->show();
	}
}

void allStripsOff(void)
{
	for (int i = 0; i < allStripsLength; i++)
	{
		allStrips[i]->clear();
		allStripsShow();
	}
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

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(Adafruit_NeoPixel &strip, uint32_t *color, int wait)
{
	for (int i = 0; i < strip.numPixels(); i++)
	{									   // For each pixel in strip...
		au.setPixelColor(strip, i, color); //  Set pixel's color (in RAM)
		strip.show();					   //  Update strip to match
		delay(wait);					   //  Pause for a moment
	}
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(Adafruit_NeoPixel &strip, uint32_t *color, int wait)
{
	for (int a = 0; a < 10; a++)
	{ // Repeat 10 times...
		for (int b = 0; b < 3; b++)
		{				   //  'b' counts from 0 to 2...
			strip.clear(); //   Set all pixels in RAM to 0 (off)

			// 'c' counts up from 'b' to end of strip in steps of 3...
			for (int c = b; c < strip.numPixels(); c += 3)
			{
				au.setPixelColor(strip, c, color); // Set pixel 'c' to value 'color'
			}
			strip.show(); // Update strip with new contents
			delay(wait);  // Pause for a moment
		}
	}
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(Adafruit_NeoPixel &strip, int wait)
{
	// Hue of first pixel runs 5 complete loops through the color wheel.
	// Color wheel has a range of 65536 but it's OK if we roll over, so
	// just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
	// means we'll make 5*65536/256 = 1280 passes through this loop:
	for (long firstPixelHue = 0; firstPixelHue < 1 * 65536; firstPixelHue += 256)
	{
		// strip.rainbow() can take a single argument (first pixel hue) or
		// optionally a few extras: number of rainbow repetitions (default 1),
		// saturation and value (brightness) (both 0-255, similar to the
		// ColorHSV() function, default 255), and a true/false flag for whether
		// to apply gamma correction to provide 'truer' colors (default true).
		au.updateBrightness();
		strip.rainbow(firstPixelHue, 1, 255, AnimationUtils::brightness, true);
		// Above line is equivalent to:
		// strip.rainbow(firstPixelHue, 1, 255, 255, true);
		strip.show(); // Update strip with new contents
		delay(wait);  // Pause for a moment
	}
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void hueCycle(Adafruit_NeoPixel &strip, int wait)
{
	// Hue of first pixel runs 5 complete loops through the color wheel.
	// Color wheel has a range of 65536 but it's OK if we roll over, so
	// just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
	// means we'll make 5*65536/256 = 1280 passes through this loop:
	for (long hue = 0; hue < 1 * 65536; hue += 256)
	{
		au.updateBrightness();
		strip.fill(Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(hue, 255, AnimationUtils::brightness)));
		strip.show(); // Update strip with new contents
		delay(wait);  // Pause for a moment
	}
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(Adafruit_NeoPixel &strip, int wait)
{
	int firstPixelHue = 0; // First pixel starts at red (hue 0)
	for (int a = 0; a < 30; a++)
	{ // Repeat 30 times...
		for (int b = 0; b < 3; b++)
		{				   //  'b' counts from 0 to 2...
			strip.clear(); //   Set all pixels in RAM to 0 (off)
			// 'c' counts up from 'b' to end of strip in increments of 3...
			for (int c = b; c < strip.numPixels(); c += 3)
			{
				// hue of pixel 'c' is offset by an amount to make one full
				// revolution of the color wheel (range 65536) along the length
				// of the strip (strip.numPixels() steps):
				int hue = firstPixelHue + c * 65536L / strip.numPixels();
				uint32_t color = strip.ColorHSV(hue); // hue -> RGB
				const uint32_t *pColor = &color;
				au.setPixelColor(strip, c, pColor); // Set pixel 'c' to value 'color'
			}
			strip.show();				 // Update strip with new contents
			delay(wait);				 // Pause for a moment
			firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
		}
	}
}



// Display letter from array
void doLetterRegions(char theLetter, int regionStart, int startingPixel)
{
	uint32_t* letterColor = usingDummyData == false ? mpColors.teal.pointer : mpColors.red.pointer;
	int *ledCharacter = textCharacter.getCharacter(theLetter);
	const uint32_t *character_array[letterTotalPixels] = {};

	// Map character array to LED colors
	for (int i = 0; i < letterTotalPixels; i++)
	{
		switch (ledCharacter[i])
		{
		case 0:
			character_array[i] = mpColors.off.pointer;
			break;
		case 1:
			character_array[i] = letterColor;
			break;
		default:
			character_array[i] = mpColors.off.pointer;
		}
	}

	Adafruit_NeoPixel *&target = allStrips[0];

	int regionOffset = innerPixelsChunkLength * regionStart;

	int pixel = 0 + startingPixel + regionOffset;

	int previousPixel = startingPixel + regionOffset - letterSpacing;

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
			au.setPixelColor(*target, drawPreviousPixel, mpColors.off.pointer);
		}
		if (regionStart <= drawPixel && drawPixel < regionEnd) {
			au.setPixelColor(*target, drawPixel, character_array[i]);
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
		nameScrollDone = true;
	}
}



// Create Meteor object
void createMeteor(int strip, int region, bool directionDown = true,  int startPixel = 0) {
	
	for (int i = 0; i < 500; i++) {
		if (animate.ActiveMeteors[i] != nullptr) {
			// Serial.print("Could not create meteor #"); Serial.print(i); Serial.print(" startPixel: "); Serial.println(animate.ActiveMeteors[i]->firstPixel);
			continue;
		}
		
		// Serial.print(">>>> ADDING METEOR #"); Serial.println(i);
		animate.ActiveMeteors[i] = new Meteor {
			directionDown,					// directionDown
			startPixel,						// firstPixel
			region,							// region
			(int) outerPixelsChunkLength,	// regionLength
			mpColors.purple.pointer,		// pColor
			1,								// meteorSize
			false,							// meteorTrailDecay
			false,							// meteorRandomDecay
			240,							// tailHueStart
			true,							// tailHueAdd
			0.75,							// tailHueExponent
			255,							// tailHueSaturation
			allStrips[strip]					// rStrip
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
	int16_t offset = 10,
	bool randomizeOffset = false)
{

	// Stagger the starting pixel
	if (randomizeOffset == true) startPixel = startPixel - (random(0, 4) * 2);

	for (int i = 0; i < pulseCount; i++) {
		int16_t pixel = i + startPixel + (i * offset * -1);
		if (randomizeOffset == true) pixel = pixel - (random(0, 3) * 2);
		createMeteor(strip, region, directionDown, pixel);
	}
}


void animationMeteorPulseRing(
	uint8_t strip,
	bool directionDown = true,
	uint8_t pulseCount = 2,
	int16_t offset = 10,
	bool randomizeOffset = false)
{
	for (int i = 0; i < outerChunks; i++) {
		// if (strip == 1 && directionDown == false && i == 1) continue;
		animationMeteorPulseRegion(strip, i, directionDown, 0, pulseCount, offset, randomizeOffset);
	}
}


/**
 * Main Animation update function
 * 
 * Gets called every loop();
 * 
*/
void updateAnimation(const char* spacecraftName, int spacecraftNameSize, int downSignalRate, int upSignalRate)
{
	// Update brightness from potentiometer
	au.updateBrightness();

	/* Update Scrolling letters animation */
	if ((millis() - wordLastTime) > wordScrollInterval) {
		try {
			if (nameScrollDone == false) scrollLetters(spacecraftName, spacecraftNameSize);
		} catch (...) {
			Serial.println("Error in scrollLetters()");
		}
		wordLastTime = millis(); // Set word timer to current millis()
	}


	if ((millis() - animationTimer) > 3000) {
		// printMeteorArray();
		if (nameScrollDone == false) {
			try {
				// Serial.println("-------- fire meteors ------------");
				// Serial.println(spacecraftName);
				if (upSignalRate > 0) {
					int pulseCountUp = 1;

					switch(upSignalRate) {
						case 6:	// 1gbps
							pulseCountUp = 5;
							// Serial.print("[Upsignal Gb]");
							animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
							break;
						case 5:	// 1mbps
							pulseCountUp = 4;
							// Serial.print("[Upsignal kb]");
							animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
							break;
						case 4: // 10lbps
							pulseCountUp = 3;
							// Serial.print("[Upsignal kb]");
							animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
							break;
						case 3:
							pulseCountUp = 2;
							// Serial.print("[Upsignal kb]");
							animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
							break;
						case 2:
							pulseCountUp = 1;
							// Serial.print("[Upsignal kb]");
							animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
							break;
						case 1:
							// Serial.print("[Upsignal slow]");
							animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
							animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
							break;
						case 0:
							allStrips[1]->clear();
							// Serial.print("[Upsignal -- ]");
							break;
						default:
							allStrips[1]->clear();
							// Serial.print("[Upsignal n/a ]");			
					}
				} else {
					allStrips[1]->clear();
					// Serial.print("[Upsignal -- ]");
				}

				if (downSignalRate > 0) {
					int pulseCountDown = 1;

					switch(downSignalRate) {
						case 6:	// 1gbps
							pulseCountDown = 5;
							// Serial.print("[Downsignal Gb]");
							animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
							break;
						case 5:	// 1mbps
							pulseCountDown = 4;
							// Serial.print("[Downsignal kb]");
							animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
							break;
						case 4: // 10lbps
							pulseCountDown = 3;
							// Serial.print("[Downsignal kb]");
							animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
							break;
						case 3:
							pulseCountDown = 2;
							// Serial.print("[Downsignal kb]");
							animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
							break;
						case 2:
							pulseCountDown = 1;
							// Serial.print("[Downsignal kb]");
							animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
							break;
						case 1:
							// Serial.print("[Downsignal slow]");
							animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
							animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
							break;
						case 0:
							allStrips[2]->clear();
							// Serial.print("[Downsignal -- ]");
							break;
						default:
							allStrips[2]->clear();
							// Serial.print("[Downsignal n/a ]");			 
					}
				} else {
					allStrips[2]->clear();
					// Serial.print("[Downsignal -- ]");
				}
			} catch (...) {
				Serial.println("Error in signal animation");
			}

			Serial.println();
		}
		
		
		animationTimer = millis(); // Set animation timer to current millis()
	}


	// Update meteor animations
	for (int i = 0; i < 100; i++) {	
		if (animate.ActiveMeteors[i] != nullptr) animate.animateMeteor(animate.ActiveMeteors[i]);
	}



	// Illuminate LEDs
	allStripsShow();


	/* After Showing LEDs */

	/* Update first pixel location for all active Meteors in array */
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
	// XML Parsing
	XMLDocument xmlDocument; // Create variable for XML document

	const char * charPayload = payload;

	// Handle XML parsing error	
	try {
		if (xmlDocument.Parse(charPayload) == XML_SUCCESS) {
			Serial.print(termColor("green"));
			Serial.print("XML Parsed Succcessfully");
			Serial.println(termColor("reset"));
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
	
	for (int i = 0; i < 100; i++) {
		// Serial.print("->>> Parse loop: ");
		// Serial.println(i);
		// // print stationcount, dishcount, targetcount
		// Serial.println("");
		// Serial.print("stationCount: "); Serial.println(stationCount);
		// Serial.print("dishCount: "); Serial.println(dishCount);
		// Serial.print("targetCount: "); Serial.println(targetCount);
		// Serial.println("");

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
						Serial.print(termColor("red"));
						Serial.println("Problem copying callsign to newCraft->callsignArray");
						Serial.println(termColor("reset"));
						handleException();
						return;
					}
					// print callsign
					Serial.print(termColor("yellow"));
					Serial.print("PARSED callsign: "); Serial.println(newCraft->callsign);
					Serial.println(termColor("reset"));
					const char* name = data.callsignToName(target);
					// print name	
					Serial.print(termColor("yellow"));
					Serial.print("PARSED name: "); Serial.println(name);
					Serial.println(termColor("reset"));
					try {
						strlcpy(newCraft->nameArray, name, 100);
					} catch (...) {
						Serial.print(termColor("red"));
						Serial.println("Problem copying name to newCraft->nameArray");
						Serial.println(termColor("reset"));
						handleException();
						return;
					}
					newCraft->nameLength = strlen(name);
					break;
				}

				// Loop through XML signal elements and find the one that matches the target
				for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("downSignal")) {
					const char* spacecraft = xmlSignal->Attribute("spacecraft");
					const char* signalType = xmlSignal->Attribute("signalType");
					// Serial.print("signalType: "); Serial.println(signalType);

					try {
						if (strcmp(signalType, "data") != 0) {
							// Serial.println("Not a data signal, skipping...");
							continue;
						}
						if (strcmp(spacecraft, newCraft->callsign) != 0) {
							Serial.print(spacecraft); Serial.print(" != "); Serial.println(newCraft->callsign);
							// Serial.println("Not the right spacecraft, skipping...");
							continue;
						}
					}
					catch (...) {
						Serial.print(termColor("red"));
						Serial.println("Problem parsing payload:");
						Serial.println(termColor("reset"));
						handleException();
						return;
					}
					Serial.println("---");

					const char* downRate = xmlSignal->Attribute("dataRate");
					// print rate
					Serial.print(termColor("yellow"));
					Serial.print("PARSED down rate: "); Serial.println(downRate);
					Serial.println(termColor("reset"));
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
							Serial.println("Not the right spacecraft, skipping...");
							continue;
						}
					}
					catch (...) {
						Serial.print(termColor("red"));
						Serial.println("Problem parsing payload:");
						Serial.println(termColor("reset"));
						handleException();
						return;
					}
					Serial.println("---");
					
					const char* upRate = xmlSignal->Attribute("dataRate");
					// print rate
					Serial.print(termColor("yellow"));
					Serial.print("PARSED up rate: "); Serial.println(upRate);
					Serial.println(termColor("reset"));
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
			Serial.println("---------- found craft ----------");
			Serial.println(newCraft->name);
			Serial.println(newCraft->callsign);
			Serial.println(newCraft->downSignal);
			Serial.println(newCraft->upSignal);
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
		}

	}

	if (newCraft->name != 0 && (newCraft->downSignal != 0 || newCraft->upSignal != 0)) {

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

		// Add data to queue, to be passed to another task
		if (xQueueSend(queue, newCraft, portMAX_DELAY))
		{
			Serial.print(termColor("green"));
			Serial.print("Added to queue");
			Serial.println(termColor("reset"));
		} else {
			delete newCraft;
			Serial.println(termColor("red"));
			Serial.print("Failed to add to queue");
			Serial.println(termColor("reset"));
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
}


void fetchData() {
	if (TEST_CORES == 1)
		{
			Serial.print("getData() running on core ");
			Serial.println(xPortGetCoreID());
		}

		// Serial.print("WiFi Status: ");
		// Serial.println(WiFi.status());

		// Check WiFi connection status
		if (WiFi.status() != WL_CONNECTED) {
			Serial.println("WiFi Disconnected");

			// WiFi.begin(ssid, password);
			// Serial.println("Connecting");
			// // while(WiFi.status() != WL_CONNECTED) {
			// //   Serial.print(".");
			// // }
			// Serial.println("");
			// Serial.print("Connected to WiFi network with IP Address: ");
			// Serial.println(WiFi.localIP());

			return;
		}

		// set fetchUrl to empty
		memset(fetchUrl, 0, sizeof(fetchUrl));
		strcpy(fetchUrl, serverName);
		
		try {
			char randBuffer[3];
			ltoa(random(300), randBuffer, 10);
			strcat(fetchUrl, randBuffer);
		} catch (...) {
			handleException();
		}

		if (SHOW_SERIAL == 1)
		{
			Serial.println(fetchUrl);
		}

		// Use WiFiClient class to create TCP connections
		http.begin(fetchUrl);

		// Send HTTP GET request
		http.addHeader("Content-Type", "text/xml");
		int httpResponseCode = http.GET();


		Serial.print("Using dummy data? ");
		if (usingDummyData == true) {
			Serial.println("TRUE");
		} else {
			Serial.println("FALSE");
		}

		if (usingDummyData == true) {
			Serial.print(termColor("purple"));
			Serial.print("Using dummy xml data!!");
			Serial.println(termColor("reset"));
			parseData(dummyXmlData);
		}

		if (usingDummyData == false && noTargetFoundCounter > noTargetLimit) {
			Serial.print(termColor("purple"));
			Serial.print("Target not found limit reach - using dummy xml data");
			Serial.println(termColor("reset"));
			usingDummyData = true;
			Serial.println(usingDummyData);
			noTargetFoundCounter = 0;
			parseData(dummyXmlData);
		}

		if (usingDummyData == false && noTargetFoundCounter < noTargetLimit + 1) {
			if (httpResponseCode != 200) {
				Serial.print("HTTP Response: ");
				Serial.println(httpResponseCode);

				usingDummyData = true;
				Serial.println("----------->>> Dummy XML <<<-------------");
				try {
					parseData(dummyXmlData);
					
				} catch(...) {
					Serial.println("nope!");
				}
			} else {
				try {
					String res = http.getString();
					const char* charRes = res.c_str();

					Serial.println("===========!!!!!!!! PAYLOAD STRING !!!!!!!!!!!==============");
					// Serial.println(charRes);
					// pPayload = pRes;
					usingDummyData = false;
					parseData(charRes);
				} catch (...) {
					handleException();
				}
			}
		}

		// Serial.print(termColor("yellow"));
		// Serial.print("---->>> Freeing resources <<<----");
		// Serial.println(termColor("reset"));
		http.end(); // Free up resources
		if (dataStarted == false) dataStarted = true;

		return;
}

// Fetch XML data from HTTP & parse for use in animations
void getData(void *parameter) 
{
	for (;;)
	{

		// Send an HTTP POST request every 5 seconds
		if ((millis() - lastTime) > timerDelay) {
			fetchData();
			lastTime = millis(); // Sync reference variable for timer
		}
	}
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
      wm.setConfigPortalTimeout(120);
      
      if (!wm.startConfigPortal(AP_SSID)) {
        Serial.println("failed to connect or hit timeout");
        delay(3000);
        // ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial.println("Connected to WiFi)");
      }
    }
  }
}

// setup() function -- runs once at startup --------------------------------
void setup()
{
	Serial.begin(115200); // Begin serial communications, ESP32 uses 115200 rate

	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	//wm.resetSettings();

	// Serial.setDebugOutput(true);
	// wm.setDebugOutput(true);
	wm.setCleanConnect(true);
	wm.setConnectRetries(5);
	wm.setConnectTimeout(30); // connect attempt fails after n seconds
	// wm.setSaveConnectTimeout(5);
	wm.setConfigPortalTimeout(120);

	// Automatically connect using saved credentials,
	// if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
	// if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
	// then goes into a blocking loop awaiting configuration and will return success result

	bool res;
	res = wm.autoConnect(AP_SSID, AP_PASS); // non-password protected ap

	if(!res) {
		Serial.println("Failed to connect");
		ESP.restart();
	} 
	else {
		//if you get here you have connected to the WiFi    
		Serial.println("Connected to WiFi");
	}


	if (TEST_CORES == 1)
	{
		Serial.print("setup() running on core ");
		Serial.println(xPortGetCoreID());
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

	Serial.print("allStripsLength: ");
	Serial.println(allStripsLength);
	// Initialize NeoPixel objects
	for (int i = 0; i < allStripsLength; i++)
	{
		allStrips[i]->begin();
	}

	// Set brightness
	for (int i = 0; i < allStripsLength; i++)
	{
		allStrips[i]->setBrightness(BRIGHTNESS);
	}

	allStripsOff();	// Turn off all NeoPixels
	
	// Identify Neopixel strips by filling with unique colors
	if (ID_LEDS == 1)
	{
		Serial.println("ID LED strips");
		const uint32_t *colors[] = {mpColors.red.pointer, mpColors.green.pointer, mpColors.blue.pointer, mpColors.purple.pointer, mpColors.white.pointer};

		au.updateBrightness(); // Update brightness from potentiometer

		// for (int c = 0; c < sizeof(colors) / sizeof(colors[0]); c++)
		// {
		// 	for (int i = 0; i < allStripsLength; i++)
		// 	{
		// 		allStrips[i]->fill(*colors[c]);
		// 	}
		// 	allStripsShow();
		// 	delay(5000);
		// }

		// outer_pixels.fill(*mpColors.blue.pointer);
		// // bottom_pixels.fill(*mpColors.purple.pointer);
		// outer_pixels.show();
		// bottom_pixels.show();
		
		allStrips[0]->fill(*mpColors.red.pointer);
		allStrips[1]->fill(*mpColors.red.pointer);
		allStrips[2]->fill(*mpColors.red.pointer);
		allStripsShow();
		delay(5000);

		allStrips[0]->fill(*mpColors.green.pointer);
		allStrips[1]->fill(*mpColors.green.pointer);
		allStrips[2]->fill(*mpColors.green.pointer);
		allStripsShow();
		delay(5000);

		allStrips[0]->fill(*mpColors.blue.pointer);
		allStrips[1]->fill(*mpColors.blue.pointer);
		allStrips[2]->fill(*mpColors.blue.pointer);
		allStripsShow();
		delay(5000);


		
		// allStrips[0]->fill(*mpColors.red.pointer);
		// allStrips[1]->fill(*mpColors.green.pointer);
		// allStrips[2]->fill(*mpColors.blue.pointer);
		// allStrips[3]->fill(*mpColors.purple.pointer);
		// allStripsShow();
		// delay(10000);
		// allStripsOff();
		// delay(3000);

		// hueCycle(*allStrips[0], 10);
		// allStripsOff();
		// hueCycle(*allStrips[1], 10);
		// allStripsOff();
		// hueCycle(*allStrips[2], 10);
		// allStripsOff();
		// hueCycle(*allStrips[3], 10);
		// allStripsOff();
		// delay(3000);

		// colorWipe(*allStrips[0], mpColors.red.pointer, 10);
		// colorWipe(*allStrips[1], mpColors.green.pointer, 10);
		// colorWipe(*allStrips[2], mpColors.blue.pointer, 10);
		// colorWipe(*allStrips[3], mpColors.yellow.pointer, 10);

		// allStripsShow();
		// delay(10000);
		// rainbow(*allStrips[0], 10);             // Flowing rainbow cycle along the whole strip
		// rainbow(*allStrips[1], 10);             // Flowing rainbow cycle along the whole strip
		// rainbow(*allStrips[2], 10);             // Flowing rainbow cycle along the whole strip
		// rainbow(*allStrips[3], 10);             // Flowing rainbow cycle along the whole strip

		allStripsOff();
	}

	if (DISABLE_WIFI == 0)
	{
		// Connect to WiFi
		// wifiSetup();
		// delay(100);
		// printWifiMode();
		// scanWifiNetworks(); // Scan for available WiFi networks
		// delay(1000);
		// printWifiStatus(); // Print WiFi status
		// delay(1000);

		// WiFi.begin(ssid, password); // Attempt to connect to WiFi
		// Serial.println("Connecting...");
		// while (WiFi.waitForConnectResult() != WL_CONNECTED)
		// {
		// 	delay(1000);
		// 	Serial.print(".");
		// }
		// delay(1000);
		// Serial.print("hostname: ");
		// Serial.println(WiFi.getHostname());
		// Serial.println();
		// printWifiStatus();
		// Serial.print("Connected to WiFi network with IP Address: ");
		// Serial.println(WiFi.localIP());
		// delay(1000);

		// serverSetup();
		// delay(1000);

		
		
		

	}

	// Initialize task for core 1
	xTaskCreatePinnedToCore(
		getData,	 /* Function to implement the task */
		"getData",	 /* Name of the task */
		50000,		 /* Stack size in words */
		NULL,		 /* Task input parameter */
		0,			 /* Priority of the task */
		&HandleData, /* Task handle. */
		0);			 /* Core where the task should run */

	// Create queue to pass data between tasks on separate cores
	queue = xQueueCreate(5, sizeof(CraftQueueItem)); // Create queue

	if (queue == NULL)
	{ // Check that queue was created successfully
		Serial.println("Error creating the queue");
		delay(3000);
		ESP.restart();
	}

	http.setReuse(true);
	data.loadJson();
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop()
{
	checkWifiButton();

	if (TEST_CORES == 1)
	{
		if (millis() - lastTime > 4000 && millis() - lastTime < 4500)
		{
			Serial.print("loop() running on core ");
			Serial.println(xPortGetCoreID());
		}
	}

	if (dataStarted == true && nameScrollDone == true) {
		Serial.println("nameScrollDone == true");
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
			Serial.println("queue received");
			try {
				// print all values of currentCraftBuffer
				Serial.println("------- DATA BUFFER ----------");
				Serial.print("callsign: "); Serial.println(currentCraftBuffer.callsign);
				Serial.print("name: "); Serial.println(currentCraftBuffer.name);
				Serial.print("nameLength: "); Serial.println(currentCraftBuffer.nameLength);
				Serial.print("downSignal: "); Serial.println(currentCraftBuffer.downSignal);
				Serial.print("upSignal: "); Serial.println(currentCraftBuffer.upSignal); 
				printFreeHeap();
			} catch (...) {
				Serial.println("Error printing currentCraftBuffer");
				handleException();
			}

			try {
				// print received queue item
				Serial.println("------- DATA RECEIVED ----------");
				Serial.print("callsign: "); Serial.println(infoBuffer.callsign);
				Serial.print("name: "); Serial.println(infoBuffer.name);
				Serial.print("nameLength: "); Serial.println(infoBuffer.nameLength);
				Serial.print("downSignal: "); Serial.println(infoBuffer.downSignal);
				Serial.print("upSignal: "); Serial.println(infoBuffer.upSignal);
			} catch (...) {
				Serial.println("Error printing infoBuffer");
				handleException();
			}

			try {
				if (infoBuffer.name != 0 && strlen(infoBuffer.name) > 0 ) {
					try {
						Serial.println("Copying data from queue to buffer...");
						// // print size of CraftQueueItem
						// Serial.print("Size of CraftQueueItem: "); Serial.println(sizeof(CraftQueueItem));
						// //print size of infoBuffer
						// Serial.print("Size of infoBuffer: "); Serial.println(sizeof(infoBuffer));
						// // print size of CurrentCraftBuffer
						// Serial.print("Size of currentCraftBuffer: "); Serial.println(sizeof(currentCraftBuffer));

						// memset(&currentCraftBuffer, 0, sizeof(struct CraftQueueItem));

						// set currentCraftBuffer to infoBuffer
						memcpy(&currentCraftBuffer, &infoBuffer, sizeof(struct CraftQueueItem));
						// memcpy(currentCraftBuffer.callsignArray, infoBuffer.callsignArray, 10);
						// memcpy(currentCraftBuffer.nameArray, infoBuffer.name, 100);
						// currentCraftBuffer.nameLength = infoBuffer.nameLength;
						// currentCraftBuffer.downSignal = infoBuffer.downSignal;
						// currentCraftBuffer.upSignal = infoBuffer.upSignal;

						nameScrollDone = false;

						try {
							// print all values of currentCraftBuffer
							Serial.println("------- DATA BUFFER 2 ----------");
							Serial.print("callsign: "); Serial.println(currentCraftBuffer.callsign);
							Serial.print("name: "); Serial.println(currentCraftBuffer.name);
							Serial.print("nameLength: "); Serial.println(currentCraftBuffer.nameLength);
							Serial.print("downSignal: "); Serial.println(currentCraftBuffer.downSignal);
							Serial.print("upSignal: "); Serial.println(currentCraftBuffer.upSignal);
							printFreeHeap();
						} catch (...) {
							Serial.println("Error printing currentCraftBuffer");
							handleException();
						}

					} catch(...) {
						Serial.println("Error copying data from queue to buffer");
						handleException();
					}
				} else {
					Serial.println("Callsign is empty");
					noTargetFoundCounter++;
				}
			} catch(...) {
				Serial.println("Error copying data from queue to buffer");
				handleException();
			}

			delete pInfoBuffer;

		} else {
			Serial.print(termColor("red"));
			Serial.println("No data received");
			Serial.print(termColor("reset"));
		}
	}

	try {
		updateAnimation(currentCraftBuffer.name, currentCraftBuffer.nameLength, currentCraftBuffer.downSignal, currentCraftBuffer.upSignal);
	} catch(...) {
		Serial.println("Error updating animation");
		handleException();
	}
}
