/* LIBRARIES */
#include <Arduino.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <tinyxml2.h>
#include <iostream>
#include <cstring>
#include <ArduinoJson.h>
#include <algorithm>
#include <MathHelpers.h>	// Custom math helpers lib
#include <TextCharacters.h> // Custom LED text lib
#include <AnimationUtils.h> // Custom animation utilities lib
#include <Animate.h>		// Custom animate lib
#include <SpacecraftData.h>
#include <WiFiManager.h>

/* NAMESPACES */
using namespace tinyxml2;
using namespace std;

/* CONFIG */
#define AP_SSID "MiniPulse"

#define OUTER_PIN 17
#define MIDDLE_PIN 18
#define INNER_PIN 19
#define BOTTOM_PIN 16
#define WIFI_RST 21
#define OUTPUT_ENABLE 22
#define BRIGHTNESS 16 // Global brightness value. 8bit, 0-255
#define POTENTIOMETER 32

// Diagnostic utilities, all 0 is normal operation
#define TEST_CORES 0
#define SHOW_SERIAL 0
#define ID_LEDS 0
#define DISABLE_WIFI 0

AnimationUtils au(POTENTIOMETER);
AnimationUtils::Colors mpColors;
Animate animate;
SpacecraftData data;

const char* serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml?r="; // URL to fetch
char fetchUrl[45];
const char* dummyXmlData = PROGMEM R"==--==(<dsn><station friendlyName="Goldstone" name="gdscc" timeUTC="1663914800000" timeZoneOffset="-25200000" /><dish azimuthAngle="187.0" elevationAngle="60.00" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="0.0000"><upSignal dataRate="16000" frequency="2090" power="4.955" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="40000" frequency="2270000000" power="-120.2351" signalType="data" spacecraft="JWST" spacecraftID="-170" /><downSignal dataRate="28000000" frequency="25900000000" power="-91.1759" signalType="data" spacecraft="JWST" spacecraftID="-170" /><target downlegRange="1.278e+06" id="170" name="JWST" rtlt="8.527" uplegRange="1.278e+06" /></dish><dish azimuthAngle="182.5" elevationAngle="39.07" isArray="false" isDDOR="false" isMSPA="false" name="DSS25" windSpeed="0.0000"><upSignal dataRate="2000" frequency="7160" power="0.0000" signalType="none" spacecraft="DSSR" spacecraftID="-116" /><downSignal dataRate="0.0000" frequency="7160000000" power="-150.0076" signalType="carrier" spacecraft="DSSR" spacecraftID="-116" /><target downlegRange="-1.000e+00" id="116" name="DSSR" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><dish azimuthAngle="303.4" elevationAngle="12.20" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="0.0000"><downSignal dataRate="0.0000" frequency="8421000000" power="-177.8969" signalType="none" spacecraft="TEST" spacecraftID="-99" /><target downlegRange="-1.000e+00" id="99" name="TEST" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><dish azimuthAngle="277.2" elevationAngle="11.18" isArray="false" isDDOR="false" isMSPA="false" name="DSS14" windSpeed="2.469"><downSignal dataRate="160.0" frequency="8419000000" power="-155.4095" signalType="data" spacecraft="VGR1" spacecraftID="-31" /><target downlegRange="2.360e+10" id="31" name="VGR1" rtlt="157500" uplegRange="2.361e+10" /></dish><station friendlyName="Madrid" name="mdscc" timeUTC="1663914800000" timeZoneOffset="7200000" /><dish azimuthAngle="99.03" elevationAngle="34.73" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="4.321"><downSignal dataRate="146400" frequency="2271000000" power="-101.8831" signalType="data" spacecraft="LRO" spacecraftID="-85" /><target downlegRange="3.928e+05" id="85" name="LRO" rtlt="2.621" uplegRange="3.929e+05" /></dish><dish azimuthAngle="96.27" elevationAngle="24.56" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="4.321"><upSignal dataRate="1000" frequency="2081" power="0.2041" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="0.0000" frequency="8475000000" power="-119.3601" signalType="none" spacecraft="KPLO" spacecraftID="-155" /><downSignal dataRate="8192" frequency="2261000000" power="-113.5036" signalType="data" spacecraft="KPLO" spacecraftID="-155" /><target downlegRange="-1.000e+00" id="155" name="KPLO" rtlt="-1.0000" uplegRange="-1.000e+00" /></dish><dish azimuthAngle="228.6" elevationAngle="65.13" isArray="false" isDDOR="false" isMSPA="true" name="DSS53" windSpeed="4.321"><downSignal dataRate="0.0000" frequency="8411000000" power="-153.9174" signalType="none" spacecraft="TGO" spacecraftID="-143" /><downSignal dataRate="0.0000" frequency="8440000000" power="-481.3964" signalType="none" spacecraft="MRO" spacecraftID="-74" /><target downlegRange="1.240e+08" id="74" name="MRO" rtlt="827.5" uplegRange="1.240e+08" /><target downlegRange="1.240e+08" id="143" name="TGO" rtlt="827.5" uplegRange="1.240e+08" /></dish><dish azimuthAngle="228.6" elevationAngle="64.82" isArray="false" isDDOR="false" isMSPA="false" name="DSS55" windSpeed="4.321"><downSignal dataRate="11.63" frequency="8446000000" power="-155.2768" signalType="data" spacecraft="MVN" spacecraftID="-202" /><upSignal dataRate="7.813" frequency="7188" power="7.354" signalType="data" spacecraft="MVN" spacecraftID="-202" /><target downlegRange="1.240e+08" id="202" name="MVN" rtlt="827.5" uplegRange="1.240e+08" /></dish><dish azimuthAngle="228.5" elevationAngle="64.93" isArray="false" isDDOR="false" isMSPA="true" name="DSS63" windSpeed="4.321"><upSignal dataRate="2000" frequency="7162" power="0.0000" signalType="none" spacecraft="M20" spacecraftID="-168" /><downSignal dataRate="6000000" frequency="8440000000" power="-473.8288" signalType="none" spacecraft="MRO" spacecraftID="-74" /><downSignal dataRate="0.0000" frequency="8415000000" power="-473.8288" signalType="none" spacecraft="M20" spacecraftID="-168" /><target downlegRange="1.240e+08" id="168" name="M20" rtlt="827.5" uplegRange="1.240e+08" /><target downlegRange="1.240e+08" id="74" name="MRO" rtlt="827.5" uplegRange="1.240e+08" /></dish><station friendlyName="Canberra" name="cdscc" timeUTC="1663914800000" timeZoneOffset="36000000" /><dish azimuthAngle="69.28" elevationAngle="36.89" isArray="false" isDDOR="false" isMSPA="false" name="DSS34" windSpeed="10.49"><upSignal dataRate="2000" frequency="2101" power="0.2471" signalType="data" spacecraft="MMS1" spacecraftID="-108" /><downSignal dataRate="2500000" frequency="2282000000" power="-101.9699" signalType="data" spacecraft="MMS1" spacecraftID="-108" /><target downlegRange="5.863e+04" id="108" name="MMS1" rtlt="0.3911" uplegRange="5.862e+04" /></dish><dish azimuthAngle="290.8" elevationAngle="47.88" isArray="false" isDDOR="false" isMSPA="false" name="DSS35" windSpeed="10.49"><downSignal dataRate="0.0000" frequency="32040000000" power="-149.7441" signalType="none" spacecraft="SPP" spacecraftID="-96" /><upSignal dataRate="125.0" frequency="7176" power="0.0000" signalType="none" spacecraft="SPP" spacecraftID="-96" /><downSignal dataRate="0.0000" frequency="32040000000" power="-151.3900" signalType="none" spacecraft="SPP" spacecraftID="-96" /><target downlegRange="1.261e+08" id="96" name="SPP" rtlt="841.1" uplegRange="1.261e+08" /></dish><dish azimuthAngle="279.5" elevationAngle="23.46" isArray="false" isDDOR="false" isMSPA="false" name="DSS43" windSpeed="10.49"><downSignal dataRate="245800" frequency="2245000000" power="-123.2127" signalType="data" spacecraft="SOHO" spacecraftID="-21" /><target downlegRange="1.679e+06" id="21" name="SOHO" rtlt="11.20" uplegRange="1.679e+06" /></dish><timestamp>1663914800000</timestamp></dsn>)==--==";
bool usingDummyData = false;
int noTargetFoundCounter = 0;
int noTargetLimit = 3;				// After target is not found this many times, switch to dummy XML data
int retryDataFetchCounter = 0;
int retryDataFetchLimit = 10;		// After dummy data is used this many times, try to get actual data again
const char* upSignal_string ="upSignal";
const char* downSignal_string = "downSignal";
const char* datasignal_string = "data";

// Time is measured in milliseconds and will become a bigger number
// than can be stored in an int, so long is used
unsigned long lastTime = 0; // Init reference variable for timer
unsigned long wordLastTime = 0;
unsigned long timerDelay = 5000; // Set timer to 5 seconds (5000)
unsigned long animationTimer = 0;
unsigned long targetChangeTimer = 0;
unsigned long meteorsTimer = 0;
unsigned long tick = 0;
unsigned long tickAfter = 0;

// how often each pattern updates
unsigned long wordScrollInterval = 10;
unsigned long pattern1Interval = 500;
unsigned long pattern2Interval = 500;
unsigned long pattern3Interval = 500;
unsigned long pattern4Interval = 500;
unsigned long targetChangeInterval = 5000;


/* TASKS */
TaskHandle_t HandleData; // Task for fetching and parsing data
QueueHandle_t queue;	 // Queue to pass data between tasks

/* NETWORKING */
WiFiManager wm;
HTTPClient http;

/* HARDWARDE */
Adafruit_NeoPixel neopixel;

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
void scrollLetters(const char * spacecraftName, int wordArraySize, bool nameChanged)
{
	int regionStart = 4;

	// Serial.println(spacecraftName);
	static int startPixel = 0;
	int wordSize = 0;

	int letterPixel = startPixel;

	for (int i = 0; i < wordArraySize; i++)
	{
		int previousArrayIndex = i - 1;
		if (previousArrayIndex < 0)
			previousArrayIndex = 0;

		
		char theLetter = spacecraftName[i];
		
		if (theLetter == 0) {
			wordSize = i;
			break;
		}

		doLetterRegions(theLetter, 0, letterPixel);
		doLetterRegions(theLetter, 7, letterPixel);
		// doLetterRegions(theLetter, 8, letterPixel);

		letterPixel = letterPixel - letterSpacing - characterKerning;
	}


	int wrapPixel = innerPixelsChunkLength + (wordSize * (characterHeight + characterKerning));
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
		// if (i < 4) continue;
		animationMeteorPulseRegion(strip, i, directionDown, 0, pulseCount, offset, randomizeOffset);
	}
}



/**
 * Main Animation update function
 * 
 * Gets called every loop();
 * 
*/
void updateAnimation(const char* spacecraftName, int spacecraftNameSize, bool nameChanged, bool hasDownSignal, char * downSignalRate, bool hasUpSignal, char * upSignalRate)
{
	// Update brightness from potentiometer
	au.updateBrightness();

	/* Update Scrolling letters animation */
	if ((millis() - wordLastTime) > wordScrollInterval)
	{
		// string wordLength = (string) spacecraftName;
		// int wordSize = wordLength.length();

		if (nameScrollDone == false) scrollLetters(spacecraftName, spacecraftNameSize, nameChanged);
		wordLastTime = millis(); // Set word timer to current millis()
	}


	if ((millis() - animationTimer) > 3000)
	{
		// printMeteorArray();
		Serial.println("-------- fire meteors ------------");
		
		if (hasUpSignal == true) {
			int upSignalRateInt = strtol(upSignalRate, nullptr, 10);
			int pulseCountUp = 1;

			
			if (upSignalRateInt > (1024 * 1024)) {
				pulseCountUp = 3;
				Serial.print("[Upsignal Gb]");
				animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
			} else if (upSignalRateInt > 1024) {
				 pulseCountUp = 2;
				 Serial.print("[Upsignal kb]");
				 animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
			} else {
				Serial.print("[Upsignal slow]");
				animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
				animationMeteorPulseRegion(1, random(10), false, 0, pulseCountUp, 12, true);
			}
			// Serial.print("upSignalRateInt: "); Serial.println(upSignalRateInt);
			// Serial.println(1024 * 1024);
			// Serial.println(upSignalRateInt > (1024 * 1024));


			// animationMeteorPulseRing(1, false, pulseCountUp, 16, true);
			// animationMeteorPulseRegion(1, 4, false, 0, pulseCountUp, 12, true);
			// animationMeteorPulseRegion(1, 5, false, 0, pulseCountUp, 12, true);
			// animationMeteorPulseRegion(1, 6, false, 0, pulseCountUp, 12, true);
			// animationMeteorPulseRegion(1, 7, false, 0, pulseCountUp, 12, true);
		} else {
			Serial.print("[Upsignal -- ]");
		}
		if (hasDownSignal == true) {
			int downSignalRateInt = strtol(downSignalRate, nullptr, 10);
			int pulseCountDown = 1;

			
			if (downSignalRateInt > (1024 * 1024)) {
				pulseCountDown = 3;
				Serial.print("[Downsignal Gb]");
				animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
			} else if (downSignalRateInt > 1024) {
				pulseCountDown = 2;
				Serial.print("[Downsignal kb]");
				animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
			} else {
				Serial.print("[Downsignal slow]");
				animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
				animationMeteorPulseRegion(2, random(10), true, 0, pulseCountDown, 12, true);
			}

			// animationMeteorPulseRing(2, true, pulseCountDown, 16, true);
			// animationMeteorPulseRegion(2, 8, true, 0, pulseCountDown, 12, true);
			// animationMeteorPulseRegion(2, 9, true, 0, pulseCountDown, 12, true);
			// animationMeteorPulseRegion(2, 10, true, 0, pulseCountDown, 12, true);
			// animationMeteorPulseRegion(2, 11, true, 0, pulseCountDown, 12, true);
		} else {
			Serial.print("[Downsignal -- ]");
		}

		Serial.println("");
		animationTimer = millis(); // Set animation timer to current millis()
	}


	// Update meteor animations
	for (int i = 0; i < 100; i++) {	
		if (animate.ActiveMeteors[i] != nullptr) animate.animateMeteor(animate.ActiveMeteors[i]);
	}



	// Illuminate LEDs
	// allStripsShow();
	allStripsShow();
	// allStrips[0]->show();
	// allStrips[1]->show();
	// allStrips[2]->show();
	// allStrips[3]->show();


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

// Create data structure objects

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

// struct currentSpacecraft {
// 	const char* callsign;
// 	const char* name;
// 	bool hasDownSignal;
// 	int downSignalRate;
// 	bool hasUpSignal;
// 	int upSignalRate;
// };



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
	} catch (...) {
		Serial.print(termColor("red"));
		Serial.print("Problem parsing payload:");
		Serial.println(termColor("reset"));
		// Serial.println(charPayload);
		
		handleException();
		return;
	}

	memset(stations, 0, sizeof(DSN_Station) * 3);


	// Find XML elements
	XMLNode *root = xmlDocument.RootElement();					  // Find document root node
	XMLElement *timestamp = root->FirstChildElement("timestamp"); // Find XML timestamp element

	uint64_t timestampInt;
	timestamp->QueryUnsigned64Text(&timestampInt); // Convert timestamp to int

	DSN_Station newStation;
	
	int i = 0; // Create station elements counter
	for (XMLElement *xmlStation = root->FirstChildElement("station"); xmlStation != NULL; xmlStation = xmlStation->NextSiblingElement("station"))
	{		
		newStation.fetchTimestamp = timestampInt;
		newStation.callsign = xmlStation->Attribute("name");
		newStation.name = xmlStation->Attribute("friendlyName");


		for (int d2 = 0; d2 < 10; d2++)
		{
			newStation.dishes[d2] = {0}; // Reset this array struct to blank
		}

		// Find dish elements that are associated with this station
		const char* dish_string = "dish";			   // Convert from string to char, as XMLElement->Value returns char
		int n = 0;							   // Create dish elements counter
		for (XMLElement *xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement())
		{
			const char *elementValue = xmlDish->Value(); // Get element value (tag type)

			if ( strcmp(elementValue, dish_string) != 0)
			{ // If the element isn't a dish, exit the loop
				break;
			}

			newStation.dishes[n].name = xmlDish->Attribute("name"); // Add dish name to data struct array

			// Find all signal elements within this dish element
			int sig2 = 0; // Create target elements counter
			for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement())
			{
				const char * spacecraft = xmlSignal->Attribute("spacecraft");
				
				// Serial.println(data.checkBlacklist(spacecraft));
				if (data.checkBlacklist(spacecraft) == true || spacecraft == nullptr) continue; // Skip this spacecraft
				Serial.print("fetch spacecraft: "); Serial.println(spacecraft);
				
				const char *elementValue = xmlSignal->Value();

				// If element is not an up/down signal, skip it
				if (strcmp(elementValue, upSignal_string) != 0 && strcmp(elementValue, downSignal_string) != 0) continue;

				newStation.dishes[n].signals[sig2] = {
					elementValue,							// direction
					xmlSignal->Attribute("signalType"),		// type
					xmlSignal->Attribute("dataRate"),		// rate
					xmlSignal->Attribute("frequency"),		// frequency
					spacecraft,		// spacecraft
					xmlSignal->Attribute("spacecraftID"),	// spacecraftID
				};

				sig2++;
			}

			// Find all target elements within this dish element
			int t2 = 0; // Create target elements counter
			for (XMLElement *xmlTarget = xmlDish->FirstChildElement("target"); xmlTarget != NULL; xmlTarget = xmlTarget->NextSiblingElement("target"))
			{
				// Serial.print("Found target #");
				// Serial.println(t2);
				const char* target = xmlTarget->Attribute("name");
				// Serial.print("Target name: "); Serial.println(target);

				// bool hasSignal = false;
				// for (int sig3 = 0; sig3 < 10; sig3++) {
				// 	Serial.println(newStation.dishes[n].signals[sig3].spacecraft);
				// 	Serial.println(target);
				// 	Serial.println("------");
				// 	if (newStation.dishes[n].signals[sig3].spacecraft == target) {
				// 		hasSignal = true;
				// 		break;
				// 	}
				// }

				// if (hasSignal == true) {
				if (target == nullptr) {
					Serial.println("Skipping target bc I think it's empty?");
					continue;
				}
					const char* fullName = data.callsignToName(target);
					fullName == nullptr
					? (
						Serial.print("?--> Unknown callsign, skipping "),
						Serial.println(target)
					) : (
						newStation.dishes[n].targets[t2].name = target,
						t2++
					);
				// } else {
				// 	Serial.print("No signal found for ");
				// 	Serial.println(target);
				// }
			}

			n++; // Iterate dish element counter
		}

		stations[i] = newStation; // Add data to top-level station struct array
		i++;					  // Iterate station element counter
	}

	return;
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

		if (usingDummyData == true) parseData(dummyXmlData);

		if (usingDummyData == false && noTargetFoundCounter > noTargetLimit) {
			Serial.println("Target not found limit reach - using dummy xml data");
			usingDummyData = true;
			Serial.println(usingDummyData);
			noTargetFoundCounter = 0;
			parseData(dummyXmlData);
		}

		if (usingDummyData = false) {
			if (httpResponseCode != 200) {
				Serial.print("Error code: ");
				Serial.println(httpResponseCode);

				usingDummyData = true;
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

					// Serial.println("===========!!!!!!!! PAYLOAD STRING !!!!!!!!!!!==============");
					// Serial.println(charRes);
					// pPayload = pRes;
					usingDummyData = false;
					parseData(charRes);
				} catch (...) {
					Serial.println("dunno");
				}
			}

			http.end(); // Free up resources
		}
		
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

			// Add data to queue, to be passed to another task
			if (xQueueSend(queue, stations, portMAX_DELAY))
			{
				// success
			} else {

			}
	
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
	queue = xQueueCreate(1, sizeof(uint64_t)); // Create queue

	if (queue == NULL)
	{ // Check that queue was created successfully
		Serial.println("Error creating the queue");
	}

	http.setReuse(true);
	data.loadJson();
}


char spacecraftCallsign[16] = {0};
char displaySpacecraftName[100] = {0};
int displaySpacecraftNameSize = 100; // This must match the array size of displaySpacecraftName
bool nameChanged = true;

bool hasDownSignal = false;
char downSignalRate[16] = {};
bool hasUpSignal = false;
char upSignalRate[16] = {};

int stationCount = 0;
int dishCount = 0;
int targetCount = 0;
int signalCount = 0;


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
		if (xQueueReceive(queue, &stations, 1) == pdPASS)
		{
			printFreeHeap();

		
			memset(spacecraftCallsign, 0, 16);
			memset(displaySpacecraftName, 0, 100);
			nameScrollDone = false;
			hasDownSignal = false;
			memset(downSignalRate, 0, 16);
			hasUpSignal = false;
			memset(upSignalRate, 0, 16);
			
			Serial.println("-----------------------------");

			// Copy callsign value into char[]
			try {
				const char* dataTargetCallsign = stations[stationCount].dishes[dishCount].targets[targetCount].name;
				Serial.print("Loop target name/callsign: ");
				if (dataTargetCallsign != 0) {
					Serial.println(dataTargetCallsign);
					strcpy(spacecraftCallsign, dataTargetCallsign);
				} else {
					Serial.println("NULL !");
				}
			} catch (...) {
				Serial.println("ERROR: Could not copy callsign string from data queue");
				handleException();
			}

			for (int i = 0; i < 10; i++) {
				Serial.println("------- SIGNAL CHECK ----------");
				const char* signalDirection = stations[stationCount].dishes[dishCount].signals[i].direction;
				const char* signalType = stations[stationCount].dishes[dishCount].signals[i].type;
				const char* signalTarget = stations[stationCount].dishes[dishCount].signals[i].spacecraft;
				
				if (hasDownSignal == true && hasUpSignal == true) break; // Both signals have values, don't look for more

				if (signalDirection == NULL) {	// Once we hit a non-existent array item, we can assume there aren't any more existing items so we end the loop
					Serial.println("signalDirection is null");
					noTargetFoundCounter++;
					break;
				}

				if (signalTarget == nullptr) continue;
				Serial.print("signalTarget: "); Serial.println(signalTarget);
				Serial.print("spacecraftCallsign: ");Serial.println(spacecraftCallsign);
				if (strcmp(signalTarget, &spacecraftCallsign[0]) != 0) continue; // Check that this signal iteraion has the spaceraft callsign
				
				Serial.print("signalDirection: "); Serial.println(signalDirection);
				Serial.print("downSignal_string: "); Serial.println(downSignal_string);
				if (hasDownSignal == false && strcmp(signalDirection, downSignal_string) == 0 && strcmp(signalTarget, &spacecraftCallsign[0]) == 0 && strcmp(signalType, datasignal_string) == 0 ) {
					hasDownSignal = true;
					strcpy(downSignalRate, stations[stationCount].dishes[dishCount].signals[i].rate);
					Serial.print("downSignalRate: "); Serial.println(downSignalRate);
				}

				if (hasUpSignal == false && strcmp(signalDirection, upSignal_string) == 0 && strcmp(signalTarget, spacecraftCallsign) == 0 && strcmp(signalType, datasignal_string) == 0) {
					hasUpSignal = true;
					strcpy(upSignalRate, stations[stationCount].dishes[dishCount].signals[i].rate);
					Serial.print("upSignalRate: "); Serial.println(upSignalRate);
				}
			
				Serial.print("\n---------------------------------\n");
			}
			
			Serial.print("Name: "); Serial.println(spacecraftCallsign);
			const char* fullName = data.callsignToName(spacecraftCallsign);
			Serial.print("Full name: "); Serial.println(fullName);
			if (fullName != "") strcpy(displaySpacecraftName, fullName);

			nextDataTarget();

			Serial.println("-----------------------------");
			targetChangeTimer = millis();
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
	

	if (spacecraftCallsign == NULL) memset(spacecraftCallsign, 0, 16);
	if (displaySpacecraftName == NULL) memset(displaySpacecraftName, 0, 100);
	updateAnimation(displaySpacecraftName, displaySpacecraftNameSize, nameChanged, hasDownSignal, downSignalRate, hasUpSignal, upSignalRate);
	nameChanged = false;
}
