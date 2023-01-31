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
char fetchUrl[45];	// DSN XML fetch URL

// Placeholder for XML data
const char* dummyXmlData = PROGMEM R"==--==(<?xml version='1.0' encoding='utf-8'?><dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" /><dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556"><downSignal dataRate="28000000" frequency="25900000000" power="-91.3965" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="JWST" spacecraftID="-170" /><upSignal dataRate="16000" frequency="2090" power="4.804" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.653e+06" id="170" name="JWST" rtlt="11.03" uplegRange="1.653e+06" /></dish><dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556"><downSignal dataRate="4000000" frequency="8439000000" power="-138.1801" signalType="none" spacecraft="MRO" spacecraftID="-74" /><upSignal dataRate="2000" frequency="7183" power="0.0000" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="8.207e+07" id="74" name="MRO" rtlt="547.5" uplegRange="8.207e+07" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" /><dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="2250000000" power="-478.1842" signalType="none" spacecraft="CHDR" spacecraftID="-151" /><target downlegRange="1.417e+05" id="151" name="CHDR" rtlt="0.9455" uplegRange="1.417e+05" /></dish><dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556"><downSignal dataRate="87650" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="ACE" spacecraftID="-92" /><upSignal dataRate="1000" frequency="2098" power="0.2630" signalType="data" spacecraft="ACE" spacecraftID="-92" /><target downlegRange="1.389e+06" id="92" name="ACE" rtlt="9.266" uplegRange="1.389e+06" /></dish><dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8436000000" power="-170.1741" signalType="none" spacecraft="LICI" spacecraftID="-210" /><target downlegRange="4.099e+06" id="210" name="LICI" rtlt="27.34" uplegRange="4.099e+06" /></dish><dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556"><upSignal dataRate="2000" frequency="2066" power="1.758" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><downSignal dataRate="245800" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.331e+06" id="21" name="SOHO" rtlt="8.882" uplegRange="1.331e+06" /></dish><dish azimuthAngle="120.0" elevationAngle="46.53" isArray="false" isDDOR="false" isMSPA="false" name="DSS63" windSpeed="5.556"><downSignal dataRate="0.0000" frequency="8415000000" power="-478.2658" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" /><dish azimuthAngle="330.6" elevationAngle="37.39" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="3.087"><upSignal dataRate="250000" frequency="2041" power="0.2421" signalType="data" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="974200" frequency="2217000000" power="-116.4022" signalType="none" spacecraft="EM1" spacecraftID="-23" /><downSignal dataRate="2000000" frequency="2216000000" power="-107.4503" signalType="carrier" spacecraft="EM1" spacecraftID="-23" /><target downlegRange="3.870e+05" id="23" name="EM1" rtlt="2.581" uplegRange="3.869e+05" /></dish><dish azimuthAngle="10.27" elevationAngle="28.97" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="3.087"><downSignal dataRate="11.63" frequency="8446000000" power="-141.8096" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7189" power="8.303" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="8.207e+07" id="202" name="MVN" rtlt="547.5" uplegRange="8.207e+07" /></dish><dish azimuthAngle="207.0" elevationAngle="15.51" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="3.087"><upSignal dataRate="16.00" frequency="2114" power="20.20" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><downSignal dataRate="160.0" frequency="8420000000" power="-156.2618" signalType="data" spacecraft="VGR2" spacecraftID="-32" /><target downlegRange="1.984e+10" id="32" name="VGR2" rtlt="132300" uplegRange="1.984e+10" /></dish><dish azimuthAngle="7.205" elevationAngle="26.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS36" windSpeed="3.087"><downSignal dataRate="8500000" frequency="8475000000" power="-120.3643" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-104.9668" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="4.405e+05" id="155" name="KPLO" rtlt="2.939" uplegRange="4.405e+05" /></dish><timestamp>1670419133000</timestamp></dsn>)==--==";
bool usingDummyData = false;					// If true, use dummy data instead of actual data
int noTargetFoundCounter = 0;					// Keeps track of how many times target is not found
int noTargetLimit = 3;							// After target is not found this many times, switch to dummy XML data
int retryDataFetchCounter = 0;					// Keeps track of how many times data fetch failed
int retryDataFetchLimit = 10;					// After dummy data is used this many times, try to get actual data again
const char* upSignal_string ="upSignal";		// String to compare data against
const char* downSignal_string = "downSignal";	// String to compare data against
const char* datasignal_string = "data";			// String to compare data against

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
uint stationCount = 0;
uint dishCount = 0;
uint targetCount = 0;
uint signalCount = 0;


struct craftDisplay {
	const char* callsign;
	const char* name;
	uint nameLength;
	uint upSignal;
	uint downSignal;
};
// Create a buffer to hold the data
craftDisplay craftDisplayBuffer;
craftDisplay* pCraftDisplayBuffer = &craftDisplayBuffer;




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
;
	
	for (int i = 0; i < 500; i++) {
		// String iPad;
		// printf("%03d", i, iPad);

		if (animate.ActiveMeteors[i] == nullptr){
			// Serial.print("["); Serial.print(termColor("red")); Serial.print(i); Serial.print(" = null"); Serial.print("]");
			printString += "[";
			printString += termColor("red");
			printString += i;
			printString += " = nul";
			printString += "]";
		}
		else {
			// Serial.print("["); Serial.print(termColor("green")); Serial.print(i); Serial.print(" = "); Serial.print(animate.ActiveMeteors[i]->firstPixel); Serial.print("]");
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



static bool nameScrollDone = true;

// Display letter from array
void doLetter(char theLetter, int startingPixel)
{
	int *ledCharacter = textCharacter.getCharacter(theLetter);
	const uint32_t *character_array[20] = {};

	// Map character array to LED colors
	for (int i = 0; i < 20; i++)
	{
		switch (ledCharacter[i])
		{
		case 0:
			character_array[i] = mpColors.off.pointer;
			break;
		case 1:
			character_array[i] = mpColors.blue.pointer;
			break;
		default:
			character_array[i] = mpColors.off.pointer;
		}
	}

	int pixel = 0 + startingPixel;
	int previousPixel = startingPixel - letterSpacing + 1;

	for (int i = 0; i < 20; i++)
	{
		int j = i + 1;
		int stripInt = j % 4;
		if (stripInt == 0)
			stripInt = 4;
		--stripInt;

		Adafruit_NeoPixel *&target = allStrips[stripInt];

		// Serial.print(*character_array[i]); Serial.print("/"); Serial.println(pixel); Serial.println();
		if (-1 < previousPixel < innerPixelsChunkLength)
			au.setPixelColor(*target, previousPixel, mpColors.off.pointer);
		if (-1 < pixel < innerPixelsChunkLength)
			au.setPixelColor(*target, pixel, character_array[i]);
		if (stripInt == allStripsLength - 1)
			pixel--; // Move to next pixel
	}

	for (int p = 0; p < 4; p++)
	{
		allStrips[p]->show();
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
		if (strip == 1 && directionDown == false && i == 1) continue;
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
	if ((millis() - wordLastTime) > wordScrollInterval)
	{
		// string wordLength = (string) spacecraftName;
		// int wordSize = wordLength.length();

		// print all parameters
		// Serial.println("-------- updateAnimation ------------");
		// Serial.print("nameScrollDone: "); Serial.println(nameScrollDone);
		// Serial.print("spacecraftName: "); Serial.println(spacecraftName);
		// Serial.print("spacecraftNameSize: "); Serial.println(spacecraftNameSize);
		// Serial.print("downSignalRate: "); Serial.println(downSignalRate);
		// Serial.print("upSignalRate: "); Serial.println(upSignalRate);


		if (nameScrollDone == false) scrollLetters(spacecraftName, spacecraftNameSize);
		wordLastTime = millis(); // Set word timer to current millis()
	}


	if ((millis() - animationTimer) > 3000)
	{
		// printMeteorArray();
		Serial.println("-------- fire meteors ------------");

		if (upSignalRate > 0) {
			int pulseCountUp = 1;

			switch(upSignalRate) {
				case 6:	// 1gbps
					pulseCountUp = 5;
					Serial.print("[Upsignal Gb]");
					animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
					break;
				case 5:	// 1mbps
					pulseCountUp = 4;
					Serial.print("[Upsignal kb]");
					animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
					break;
				case 4: // 10lbps
					pulseCountUp = 3;
					Serial.print("[Upsignal kb]");
					animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
					break;
				case 3:
					pulseCountUp = 2;
					Serial.print("[Upsignal kb]");
					animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
					break;
				case 2:
					pulseCountUp = 1;
					Serial.print("[Upsignal kb]");
					animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
					break;
				case 1:
					Serial.print("[Upsignal slow]");
					animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
					animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
					break;
				case 0:
					allStrips[1]->clear();
					Serial.print("[Upsignal -- ]");
					break;
				default:
					allStrips[1]->clear();
					Serial.print("[Upsignal n/a ]");			
			}
		} else {
			allStrips[1]->clear();
			Serial.print("[Upsignal -- ]");
		}

		if (downSignalRate > 0) {
			int pulseCountDown = 1;

			switch(downSignalRate) {
				case 6:	// 1gbps
					pulseCountDown = 5;
					Serial.print("[Downsignal Gb]");
					animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
					break;
				case 5:	// 1mbps
					pulseCountDown = 4;
					Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
					break;
				case 4: // 10lbps
					pulseCountDown = 3;
					Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
					break;
				case 3:
					pulseCountDown = 2;
					Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
					break;
				case 2:
					pulseCountDown = 1;
					Serial.print("[Downsignal kb]");
					animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
					break;
				case 1:
					Serial.print("[Downsignal slow]");
					animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
					animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
					break;
				case 0:
					allStrips[2]->clear();
					Serial.print("[Downsignal -- ]");
					break;
				default:
					allStrips[2]->clear();
					Serial.print("[Downsignal n/a ]");			
			}
		} else {
			allStrips[2]->clear();
			Serial.print("[Downsignal -- ]");
		}


		Serial.println();
		animationTimer = millis(); // Set animation timer to current millis()
	}


	// Update meteor animations
	for (int i = 0; i < 100; i++) {	
		if (animate.ActiveMeteors[i] != nullptr) animate.animateMeteor(animate.ActiveMeteors[i]);
	}



	// Illuminate LEDs
	// allStripsShow();
	allStripsShow();


	/* After Showing LEDs */

	/* Update first pixel location for all active Meteors in array */
	for (int i = 0; i < 500; i++) {
		if (animate.ActiveMeteors[i] != nullptr){
			animate.ActiveMeteors[i]->firstPixel = animate.ActiveMeteors[i]->firstPixel + 2;

			// If meteor is beyond the display region, unallocate memory and remove array item
			if (animate.ActiveMeteors[i]->firstPixel > animate.ActiveMeteors[i]->regionLength * 2) {
				delete animate.ActiveMeteors[i];
				// Serial.print("Deleted meteor #"); Serial.println(i);
				animate.ActiveMeteors[i] = nullptr;
			}
		}
	}

	// delay(1000);
}

/* Create data structure objects */
// Target struct
struct DSN_Target
{
	const char *name;
};

// Signal Struct
struct DSN_Signal
{
	const char *direction;
	const char *type;
	const char *rate;
	const char *frequency;
	const char *spacecraft;
	const char *spacecraftId;
};

// Dish struct
struct DSN_Dish
{
	const char *name;
	struct DSN_Signal signals[10];
	struct DSN_Target targets[10];
};
 
// Station struct
struct DSN_Station
{
	uint64_t fetchTimestamp;
	const char *callsign;
	const char *name;
	struct DSN_Dish dishes[10];
};

struct DSN_Station stations[3];


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
	// XMLDocument* pXmlDocument = &xmlDocument;

	const char * charPayload = payload;
	// Serial.println(charPayload);

	// Serial.println("Payload:");
	// Serial.println(charPayload);
	// Handle XML parsing error
	
	try {
		// Serial.println("XML Parse Status:");
		if (xmlDocument.Parse(charPayload) == XML_SUCCESS) {
			Serial.print(termColor("green"));
			Serial.print("XML Parsed Succcessfully");
			Serial.println(termColor("reset"));
		} else {
			Serial.println("Unable to parse XML");
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
		// Serial.println(charPayload);
		
		handleException();
		return;
	}

	// memset(stations, 0, sizeof(stations));


	// print stationcount, dishcount, targetcount
	Serial.print("stationCount: "); Serial.println(stationCount);
	Serial.print("dishCount: "); Serial.println(dishCount);
	Serial.print("targetCount: "); Serial.println(targetCount);


	// Find XML elements
	XMLNode *root = xmlDocument.RootElement();					  // Find document root node
	XMLElement *timestamp = root->FirstChildElement("timestamp"); // Find XML timestamp element

	craftDisplay newCraft {
		"",
		"",
		0,
		0,
		0
	};
	
	for (int i = 0; i < 20; i++) {

		int s = 0; // Create station elements counter
		for (XMLElement *xmlStation = root->FirstChildElement("station"); xmlStation != NULL; xmlStation = xmlStation->NextSiblingElement("station"))
		{	
			if (s > 2) s = 0;
			if (s != stationCount) {
				s++;				
				continue;
			}

			int d = 0; // Create dish elements counter
			for (XMLElement *xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement()) {
				if (d > 9) {
					stationCount++;
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
					// Serial.print("NEW FETCH target: ");
					// Serial.println(target);
					newCraft.callsign = target;
					const char* name = data.callsignToName(target);
					newCraft.name = name;
					newCraft.nameLength = strlen(name);
					break;
				}

				// Loop through XML signal elements and find the one that matches the target
				for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("downSignal"))
				{
					if (strcmp(xmlSignal->Attribute("signalType"), "data") != 0) continue;
					if (strcmp(xmlSignal->Attribute("spacecraft"), newCraft.callsign) != 0) continue;
					const char* rate = xmlSignal->Attribute("dataRate");
					// Serial.print("NEW FETCH down rate: ");
					// Serial.println(rate);
					unsigned long rateLong = stol(rate, nullptr, 10);
					// Serial.print("NEW FETCH down rateLong: ");
					// Serial.println(rateLong);
					unsigned int rateClass = rateLongToRateClass(rateLong);
					// Serial.print("NEW FETCH down rateClass: ");
					// Serial.println(rateClass);
					newCraft.downSignal = rateClass;
					break;
				}

				for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement("upSignal"))
				{
					if (strcmp(xmlSignal->Attribute("signalType"), "data") != 0) continue;
					if (strcmp(xmlSignal->Attribute("spacecraft"), newCraft.callsign) != 0) continue;
					const char* rate = xmlSignal->Attribute("dataRate");
					// Serial.print("NEW FETCH up rate: ");
					// Serial.println(rate);
					unsigned long rateLong = stol(rate, nullptr, 10);
					unsigned int rateClass = rateLongToRateClass(rateLong);
					newCraft.upSignal = rateClass;
					break;
				}


				break;
			}
			break;
		}

		if (strcmp(newCraft.name, "") != 0) {
			break;
		} else {
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

	Serial.println("---------- sending to queue ----------");
	Serial.print("NEW FETCH callsign: ");
	Serial.println(newCraft.callsign);
	Serial.print("NEW FETCH name: ");
	Serial.println(newCraft.name);
	Serial.print("NEW FETCH nameLength: ");
	Serial.println(newCraft.nameLength);
	Serial.print("NEW FETCH downSignal: ");
	Serial.println(newCraft.downSignal);
	Serial.print("NEW FETCH upSignal: ");
	Serial.println(newCraft.upSignal);


	// Add data to queue, to be passed to another task
	if (xQueueSend(queue, &newCraft, 1))
	{
		Serial.println("Added to queue");
	} else {
		Serial.println("Failed to add to queue");
	}

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



	// ---------------------------------

	// DSN_Station newStation;

	// uint64_t timestampInt;
	// timestamp->QueryUnsigned64Text(&timestampInt); // Convert timestamp to int


	// int i = 0; // Create station elements counter
	// for (XMLElement *xmlStation = root->FirstChildElement("station"); xmlStation != NULL; xmlStation = xmlStation->NextSiblingElement("station"))
	// {		
	// 	newStation.fetchTimestamp = timestampInt;
	// 	newStation.callsign = xmlStation->Attribute("name");
	// 	newStation.name = xmlStation->Attribute("friendlyName");

	// 	// print callsign and name
	// 	Serial.print(">>>> FETCH Station: ");
	// 	Serial.print(newStation.callsign);
	// 	Serial.print(" - ");
	// 	Serial.println(newStation.name);


	// 	for (int d2 = 0; d2 < 10; d2++)
	// 	{
	// 		newStation.dishes[d2] = {0}; // Reset this array struct to blank
	// 	}

	// 	// Find dish elements that are associated with this station
	// 	const char* dish_string = "dish";			   // Convert from string to char, as XMLElement->Value returns char
	// 	int n = 0;							   // Create dish elements counter
	// 	for (XMLElement *xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement())
	// 	{
	// 		const char *elementValue = xmlDish->Value(); // Get element value (tag type)

	// 		if ( strcmp(elementValue, dish_string) != 0)
	// 		{ // If the element isn't a dish, exit the loop
	// 			break;
	// 		}

	// 		newStation.dishes[n].name = xmlDish->Attribute("name"); // Add dish name to data struct array

	// 		// Find all signal elements within this dish element
	// 		int sig2 = 0; // Create target elements counter
	// 		for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement())
	// 		{
	// 			const char * spacecraft = xmlSignal->Attribute("spacecraft");
				
	// 			// Serial.println(data.checkBlacklist(spacecraft));
	// 			if (data.checkBlacklist(spacecraft) == true || spacecraft == nullptr) continue; // Skip this spacecraft
	// 			Serial.print("fetch spacecraft: "); Serial.println(spacecraft);
				
	// 			const char *elementValue = xmlSignal->Value();

	// 			// If element is not an up/down signal, skip it
	// 			if (strcmp(elementValue, upSignal_string) != 0 && strcmp(elementValue, downSignal_string) != 0) continue;

	// 			newStation.dishes[n].signals[sig2] = {
	// 				elementValue,							// direction
	// 				xmlSignal->Attribute("signalType"),		// type
	// 				xmlSignal->Attribute("dataRate"),		// rate
	// 				xmlSignal->Attribute("frequency"),		// frequency
	// 				spacecraft,		// spacecraft
	// 				xmlSignal->Attribute("spacecraftID"),	// spacecraftID
	// 			};

	// 			sig2++;
	// 		}

	// 		// Find all target elements within this dish element
	// 		int t2 = 0; // Create target elements counter
	// 		for (XMLElement *xmlTarget = xmlDish->FirstChildElement("target"); xmlTarget != NULL; xmlTarget = xmlTarget->NextSiblingElement("target"))
	// 		{
	// 			// Serial.print("Found target #");
	// 			// Serial.println(t2);
	// 			const char* target = xmlTarget->Attribute("name");
	// 			// Serial.print("Target name: "); Serial.println(target);

	// 			// bool hasSignal = false;
	// 			// for (int sig3 = 0; sig3 < 10; sig3++) {
	// 			// 	Serial.println(newStation.dishes[n].signals[sig3].spacecraft);
	// 			// 	Serial.println(target);
	// 			// 	Serial.println("------");
	// 			// 	if (newStation.dishes[n].signals[sig3].spacecraft == target) {
	// 			// 		hasSignal = true;
	// 			// 		break;
	// 			// 	}
	// 			// }

	// 			// if (hasSignal == true) {
	// 			if (target == nullptr) {
	// 				Serial.println("Skipping target bc I think it's empty?");
	// 				continue;
	// 			}
	// 				const char* fullName = data.callsignToName(target);
	// 				fullName == nullptr
	// 				? (
	// 					Serial.print("?--> Unknown callsign, skipping "),
	// 					Serial.println(target)
	// 				) : (
	// 					newStation.dishes[n].targets[t2].name = target,
	// 					t2++
	// 				);
	// 			// } else {
	// 			// 	Serial.print("No signal found for ");
	// 			// 	Serial.println(target);
	// 			// }
	// 		}

	// 		n++; // Iterate dish element counter
	// 	}

	// 	stations[i] = newStation; // Add data to top-level station struct array
	// 	i++;					  // Iterate station element counter
	// }
}


void fetchData() {
	if (TEST_CORES == 1)
		{
			Serial.print("getData() running on core ");
			Serial.println(xPortGetCoreID());
		}


		for (int s = 0; s < 3; s++)
		{
			// stations[s] = blankStation;       // Reset all station array elements with blank structs

			for (int d = 0; d < 10; d++)
			{
				stations[s].dishes[d] = {0}; // Reset all dish array elements with blank structs

				for (int sig = 0; sig < 10; sig++)
				{
					stations[s].dishes[d].signals[sig] = {0}; // Reset all signal array elements with blank structs
				}

				for (int t = 0; t < 10; t++)
				{
					stations[s].dishes[d].targets[t] = {0}; // Reset all target array elements with blank structs
				}
			}
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

		// Your Domain name with URL path or IP address with path
		http.begin(fetchUrl);

		// Send HTTP GET request
		int httpResponseCode = http.GET();

		if (SHOW_SERIAL == 1)
		{
			Serial.print("HTTP Response code: ");
			Serial.println(httpResponseCode);
		}

		Serial.print("Using dummy data? "); Serial.println(usingDummyData);

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

				// usingDummyData = true;
				Serial.println("----------->>> Dummy XML <<<-------------");
				try {
					// Serial.println(dummyXmlData);
					// pPayload = pDummyXmlData;
					parseData(dummyXmlData);
					
				} catch(...) {
					Serial.println("nope!");
				}
			} else {
				try {
					// Serial.println("Get HTTP String"); 
					String res = http.getString();
					const char* charRes = res.c_str();

					Serial.println("===========!!!!!!!! PAYLOAD STRING !!!!!!!!!!!==============");
					// Serial.println(charRes);
					// pPayload = pRes;
					usingDummyData = false;
					parseData(charRes);
				} catch (...) {
					Serial.println("dunno");
				}
			}
		}

		Serial.print(termColor("yellow"));
		Serial.print("---->>> Freeing resources <<<----");
		Serial.println(termColor("reset"));
		http.end(); // Free up resources
		
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
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(WIFI_RST) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for production
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
        Serial.println("connected...yeey :)");
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
	res = wm.autoConnect(AP_SSID); // non-password protected ap

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

	// reverseStripsArray();

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

	// Turn off all NeoPixels
	allStripsOff();
	

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

	Serial.print("Size of craft queue: ");
	Serial.println(sizeof(craftDisplay));

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
	queue = xQueueCreate(1, sizeof(craftDisplay)); // Create queue

	if (queue == NULL)
	{ // Check that queue was created successfully
		Serial.println("Error creating the queue");
	}

	http.setReuse(true);
	data.loadJson();
}


// char spacecraftCallsign[16] = {0};
// char displaySpacecraftName[100] = {0};
// int displaySpacecraftNameSize = 100; // This must match the array size of displaySpacecraftName
// bool nameChanged = true;

// bool hasDownSignal = false;
// char downSignalRate[16] = {};
// bool hasUpSignal = false;
// char upSignalRate[16] = {};




craftDisplay dataBuffer;

void nextDataTarget() {
	targetCount++;
	const char * nextTargetName = stations[stationCount].dishes[dishCount].targets[targetCount].name;
	if (nextTargetName == NULL) {
		dishCount++;
		targetCount = 0;
	}
	const char * nextDishName = stations[stationCount].dishes[dishCount].name;
	// Serial.print("Next dish int: "); Serial.println(dishCount);
	// Serial.print("Next Dish name: "); Serial.println(nextDishName);

	if (nextDishName == NULL) {
		stationCount ++;
		dishCount = 0;
	}			
	if (stationCount > 2) stationCount = 0;
	// Serial.print("Station int: "); Serial.println(stationCount);
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

	if (nameScrollDone == true) {
		// Receive from queue (data task on core 1)

		craftDisplay infoBuffer;
		
		if (xQueueReceive(queue, &infoBuffer, 1) == pdPASS)
		{
			// print all values of dataBuffer
			Serial.println("------- DATA BUFFER ----------");
			Serial.print("callsign: "); Serial.println(dataBuffer.callsign);
			Serial.print("name: "); Serial.println(dataBuffer.name);
			Serial.print("nameLength: "); Serial.println(dataBuffer.nameLength);
			Serial.print("downSignal: "); Serial.println(dataBuffer.downSignal);
			Serial.print("upSignal: "); Serial.println(dataBuffer.upSignal);
			printFreeHeap();

			nameScrollDone = false;
			// print everything in queueBuffer
			Serial.println("------- DATA RECEIVED ----------");
			Serial.print("callsign: "); Serial.println(infoBuffer.callsign);
			Serial.print("name: "); Serial.println(infoBuffer.name);
			Serial.print("nameLength: "); Serial.println(infoBuffer.nameLength);
			Serial.print("downSignal: "); Serial.println(infoBuffer.downSignal);
			Serial.print("upSignal: "); Serial.println(infoBuffer.upSignal);

			if (strcmp(infoBuffer.name, "") != 0) {
				Serial.println("OGOGOGOGO");
				dataBuffer = infoBuffer;
			}

			



			// Reset storage variables
			// memset(spacecraftCallsign, 0, sizeof(spacecraftCallsign));
			// memset(displaySpacecraftName, 0, sizeof(displaySpacecraftName));
			// memset(downSignalRate, 0, sizeof(downSignalRate));
			// memset(upSignalRate, 0, sizeof(upSignalRate));

			
		// 	hasDownSignal = false;
		// 	hasUpSignal = false;


		// 	Serial.println("------- DATA RECEIVED ----------");
		// 	Serial.print("Station: "); Serial.println(stations[stationCount].name);
		// 	Serial.print("Dish: "); Serial.println(stations[stationCount].dishes[dishCount].name);
		// 	Serial.print("Target: "); Serial.println(stations[stationCount].dishes[dishCount].targets[targetCount].name);


			
		// 	Serial.println("-----------------------------");

		// 	// Copy callsign value into char[]
		// 	try {
		// 		const char* dataTargetCallsign = stations[stationCount].dishes[dishCount].targets[targetCount].name;
		// 		Serial.print("Loop target name/callsign: ");
		// 		if (dataTargetCallsign != 0) {
		// 			Serial.println(dataTargetCallsign);
		// 			strcpy(spacecraftCallsign, dataTargetCallsign);
		// 		} else {
		// 			Serial.println("NULL !");
		// 			noTargetFoundCounter++;
		// 		}
		// 	} catch (...) {
		// 		Serial.println("ERROR: Could not copy callsign string from data queue");
		// 		handleException();
		// 	}

		// 	for (int i = 0; i < 10; i++) {
		// 		Serial.println("------- SIGNAL CHECK ----------");
		// 		const char* signalDirection = stations[stationCount].dishes[dishCount].signals[i].direction;
		// 		const char* signalType = stations[stationCount].dishes[dishCount].signals[i].type;
		// 		const char* signalTarget = stations[stationCount].dishes[dishCount].signals[i].spacecraft;
				
		// 		if (hasDownSignal == true && hasUpSignal == true) break; // Both signals have values, don't look for more

		// 		if (signalDirection == nullptr) {	// Once we hit a non-existent array item, we can assume there aren't any more existing items so we end the loop
		// 			Serial.println("signalDirection is null");
		// 			break;
		// 		}

		// 		if (signalTarget == nullptr) continue;
		// 		Serial.print("signalTarget: "); Serial.println(signalTarget);
		// 		Serial.print("spacecraftCallsign: ");Serial.println(spacecraftCallsign);
		// 		if (strcmp(signalTarget, &spacecraftCallsign[0]) != 0) continue; // Check that this signal iteraion has the spaceraft callsign
				
		// 		Serial.print("signalDirection: "); Serial.println(signalDirection);
		// 		Serial.print("downSignal_string: "); Serial.println(downSignal_string);
		// 		if (hasDownSignal == false && strcmp(signalDirection, downSignal_string) == 0 && strcmp(signalTarget, &spacecraftCallsign[0]) == 0 && strcmp(signalType, datasignal_string) == 0 ) {
		// 			hasDownSignal = true;
		// 			strcpy(downSignalRate, stations[stationCount].dishes[dishCount].signals[i].rate);
		// 			Serial.print("downSignalRate: "); Serial.println(downSignalRate);
		// 		}

		// 		if (hasUpSignal == false && strcmp(signalDirection, upSignal_string) == 0 && strcmp(signalTarget, &spacecraftCallsign[0]) == 0 && strcmp(signalType, datasignal_string) == 0) {
		// 			hasUpSignal = true;
		// 			strcpy(upSignalRate, stations[stationCount].dishes[dishCount].signals[i].rate);
		// 			Serial.print("upSignalRate: "); Serial.println(upSignalRate);
		// 		}
			
		// 		Serial.print("\n---------------------------------\n");
		// 	}
			
		// 	Serial.print("Name: "); Serial.println(spacecraftCallsign);
		// 	const char* fullName = data.callsignToName(spacecraftCallsign);
		// 	Serial.print("Full name: "); Serial.println(fullName);
		// 	if (fullName != "") strcpy(displaySpacecraftName, fullName);

		// 	nextDataTarget();

		// 	Serial.println("-----------------------------");
		// 	targetChangeTimer = millis();
		// }
		}

		if (SHOW_SERIAL == 1)
		{
			Serial.println();
			Serial.println("-------");
			Serial.print("timestamp: ");
			Serial.println(stations[0].fetchTimestamp);
			Serial.println();

			for (int i = 0; i < 3; i++)
			{
				DSN_Station station = stations[i];
				Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
				Serial.print("Station: ");
				Serial.print(station.name);
				Serial.print(" (");
				Serial.print(station.callsign);
				Serial.println(") ");
				Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

				for (int d = 0; d < 10; d++)
				{
					const char *dishName = station.dishes[d].name;

					if (dishName != NULL)
					{

						Serial.print("> Dish: ");
						Serial.println(dishName);

						for (int t = 0; t < 10; t++)
						{
							const char *targetName = station.dishes[d].targets[t].name;

							if (targetName != NULL)
							{
								Serial.print("  Target: ");
								Serial.println(targetName);
							}
						}

						for (int sig = 0; sig < 10; sig++)
						{
							if (not station.dishes[d].signals[sig].direction)
							{
								break;
							}
							const char *direction = station.dishes[d].signals[sig].direction;
							const char *type = station.dishes[d].signals[sig].type;
							const char *rate = station.dishes[d].signals[sig].rate;
							const char *frequency = station.dishes[d].signals[sig].frequency;
							const char *spacecraft = station.dishes[d].signals[sig].spacecraft;
							const char *spacecraftId = station.dishes[d].signals[sig].spacecraftId;

							Serial.println();
							Serial.print("- Signal: ");
							Serial.println(direction);
							Serial.print("  Type: ");
							Serial.println(type);
							Serial.print("  Rate: ");
							Serial.println(rate);
							Serial.print("  Frequency: ");
							Serial.println(frequency);
							Serial.print("  Spacecraft: ");
							Serial.println(spacecraft);
						}

						Serial.println();
						Serial.println("----------------------------");
						Serial.println();
					}
				}
				Serial.println();
			}
		}
	}
	
	updateAnimation(dataBuffer.name, dataBuffer.nameLength, dataBuffer.downSignal, dataBuffer.upSignal);

	// if (spacecraftCallsign == NULL) memset(spacecraftCallsign, 0, 16);
	// if (displaySpacecraftName == NULL) memset(displaySpacecraftName, 0, 100);
	// updateAnimation(displaySpacecraftName, displaySpacecraftNameSize, nameChanged, hasDownSignal, downSignalRate, hasUpSignal, upSignalRate);
	// nameChanged = false;
}
