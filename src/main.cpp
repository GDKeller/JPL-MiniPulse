/* LIBRARIES */
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
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

/* NAMESPACES */
using namespace tinyxml2;
using namespace std;

/* CONFIG */

const char *ssid = "Verizon_LT6SKN";
const char *password = "smile-grey9-hie";
#define AP_SSID "MiniPulse"

#define OUTER_PIN 17
#define MIDDLE_PIN 18
#define INNER_PIN 19
#define BOTTOM_PIN 16
#define WIFI_RST 21
#define OUTPUT_ENABLE 22
#define BRIGHTNESS 255 // Global brightness value. 8bit, 0-255
#define POTENTIOMETER 36

// Diagnostic utilities, all 0 is normal operation
#define TEST_CORES 0
#define SHOW_SERIAL 0
#define ID_LEDS 0
#define DISABLE_WIFI 0

AnimationUtils au(POTENTIOMETER);
AnimationUtils::Colors mpColors;
Animate animate;

String serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml"; // URL to fetch

// Time is measured in milliseconds and will become a bigger number
// than can be stored in an int, so long is used
unsigned long lastTime = 0; // Init reference variable for timer
unsigned long wordLastTime = 0;
unsigned long timerDelay = 5000; // Set timer to 5 seconds (5000)
unsigned long animationTimer = 0;

// how often each pattern updates
unsigned long wordScrollInterval = 20;
unsigned long pattern1Interval = 500;
unsigned long pattern2Interval = 500;
unsigned long pattern3Interval = 500;
unsigned long pattern4Interval = 500;

/* TASKS */
TaskHandle_t HandleData; // Task for fetching and parsing data
QueueHandle_t queue;	 // Queue to pass data between tasks

/* NETWORKING */
WebServer server(80); // Set server port
IPAddress local_IP(192, 168, 100, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress ap_ip(192, 168, 0, 1);
IPAddress ap_gateway(192, 168, 0, 1);
IPAddress ap_subnet(255, 255, 255, 0);
String hostname = "minipulse";
String header; // Variable to store the HTTP request

static char buf[2048];
const size_t CAPACITY = JSON_ARRAY_SIZE(50);
StaticJsonDocument<CAPACITY> jsonDoc;
JsonArray networksArray = jsonDoc.to<JsonArray>();

void createHtml()
{
	char html[] = PROGMEM R"=====(
	<!DOCTYPE html>
	<html lang="en">
	<head>
	<style>
	body { align-items: center; display: flex; flex-direction: column; padding: 20px; font-size: 18px; }
	form { display: flex; flex-direction: column; }
	input { margin-bottom: 2rem; padding: 1.2rem; }
	select { font-size: 2rem; margin: 0 auto; }
	</style>
	</head>
	<body>
	<h1>MiniPulse</h1>
	<form>
	)=====";

	strcpy(buf, html);

	strcat(buf, "<select name=\"networks\" id=\"networks\">");

	for (JsonVariant v : networksArray)
	{

		strcat(buf, "<option value=\"");
		const char *network = v.as<char *>();
		strcat(buf, network);
		strcat(buf, "\">");
		strcat(buf, network);
		strcat(buf, "</option>");
	}
	strcat(buf, "</select>");

	char newhtml[] = PROGMEM R"=====(
	<label for="password">WiFi Network Password:</label>
	<input type="text" id="password" name="password">
	<input type="submit" value="Apply & Restart">
	</form>
	</body>
	</html>
	)=====";

	strcat(buf, newhtml);
}

void sendWebsite()
{
	Serial.println("Handling HTTP request...");
	server.send(200, "text/html", buf);
}

void serverSetup()
{
	createHtml();
	server.on("/", sendWebsite);
	// server.on("/xml", SendXML);
	server.begin();
}

static volatile bool wifi_connected = false;

const char *rssiToString(uint rssi)
{
	if (rssi > -31)
		return "Amazing";
	if (rssi > -68)
		return "Very Good";
	if (rssi > -71)
		return "Okay";
	if (rssi > -81)
		return "Not Good";
	if (rssi > -91)
		return "Unusable";
	return "Unknown";
}

void wifiOnConnect()
{
	Serial.println("STA Connected");
	Serial.print("Network: ");
	Serial.println(WiFi.SSID());
	Serial.print("STA IPv4: ");
	Serial.println(WiFi.localIP());

	const uint rssi = WiFi.RSSI();
	Serial.print("Connection Strength: ");
	Serial.print(rssiToString(rssi));
	Serial.print(" (RSSI: ");
	Serial.print(WiFi.RSSI());
	Serial.print("dBm)");
	Serial.println();
}

void wifiOnDisconnect()
{
	Serial.println("STA Disconnected");
	delay(1000);
	WiFi.begin(ssid, password);
}

void WiFiEvent(WiFiEvent_t event)
{
	switch (event)
	{

	case ARDUINO_EVENT_WIFI_AP_START:
		// can set ap hostname here
		WiFi.softAPsetHostname(AP_SSID);
		// enable ap ipv6 here
		WiFi.softAPenableIpV6();
		Serial.print("AP IP Address: ");
		Serial.println(WiFi.softAPIP());
		delay(10000);
		break;

	case ARDUINO_EVENT_WIFI_STA_START:
		// set sta hostname here
		WiFi.setHostname(AP_SSID);
		break;
	case ARDUINO_EVENT_WIFI_STA_CONNECTED:
		// enable sta ipv6 here
		WiFi.enableIpV6();
		break;
	case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
		Serial.print("STA IPv6: ");
		Serial.println(WiFi.localIPv6());
		break;
	case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
		Serial.print("AP IPv6: ");
		Serial.println(WiFi.softAPIPv6());
		break;
	case ARDUINO_EVENT_WIFI_STA_GOT_IP:
		wifiOnConnect();
		wifi_connected = true;
		break;
	case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
		wifi_connected = false;
		wifiOnDisconnect();
		break;
	default:
		break;
	}
}

void scanWifiNetworks()
{
	Serial.println("Scanning available WiFi networks...");
	// WiFi.scanNetworks will return the number of networks found
	int n = WiFi.scanNetworks();
	Serial.println("Scan complete");
	if (n == 0)
	{
		Serial.println("No networks found");
	}
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.print(WiFi.encryptionType(i));
			Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
			networksArray.add(WiFi.SSID(i));
			delay(10);
		}
	}
	Serial.println("");
	delay(500);
	return;
}

void printWifiMode()
{
	Serial.print("WiFi Mode: ");
	Serial.println(WiFi.getMode());
	Serial.println();
	delay(100);
}

const char *wl_status_to_string(wl_status_t status)
{
	switch (status)
	{
	case WL_NO_SHIELD:
		return "WL_NO_SHIELD";
	case WL_IDLE_STATUS:
		return "WL_IDLE_STATUS";
	case WL_NO_SSID_AVAIL:
		return "WL_NO_SSID_AVAIL";
	case WL_SCAN_COMPLETED:
		return "WL_SCAN_COMPLETED";
	case WL_CONNECTED:
		return "WL_CONNECTED";
	case WL_CONNECT_FAILED:
		return "WL_CONNECT_FAILED";
	case WL_CONNECTION_LOST:
		return "WL_CONNECTION_LOST";
	case WL_DISCONNECTED:
		return "WL_DISCONNECTED";
	default:
		return "WiFi status code unknown";
	}
}

void printWifiStatus()
{
	Serial.print("WiFi Status: ");
	Serial.println(wl_status_to_string(WiFi.status()));
}

void wifiSetup()
{
	WiFi.disconnect(true);
	delay(100);
	WiFi.onEvent(WiFiEvent);
	WiFi.mode(WIFI_MODE_APSTA);
	// WiFi.config(local_IP, gateway, subnet);
	WiFi.setHostname("minipulse"); // Set custom hostname
	WiFi.softAPConfig(ap_ip, ap_gateway, ap_subnet);
	WiFi.softAP(AP_SSID);
}

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

// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
Adafruit_NeoPixel
	outer_pixels(outerPixelsTotal, OUTER_PIN, NEO_GBR + NEO_KHZ800),
	middle_pixels(middlePixelsTotal, MIDDLE_PIN, NEO_GBR + NEO_KHZ800),
	inner_pixels(innerPixelsTotal, INNER_PIN, NEO_GBR + NEO_KHZ800),
	bottom_pixels(bottomPixelsTotal, BOTTOM_PIN, NEO_GBR + NEO_KHZ800);

Adafruit_NeoPixel *allStrips[4] = {
	&inner_pixels,	// ID: Green
	&middle_pixels, // ID: Red
	&outer_pixels,	// ID: Blue
	&bottom_pixels, // ID: Purple
};

int allStripsLength = sizeof(allStrips) / sizeof(allStrips[0]);
void allStripsShow(void)
{
	for (int i = 0; i < allStripsLength; i++)
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

// Manage meteors
void fireMeteor(int meteorRegion, int startPixel)
{
	animate.meteorRainRegions(
		allStrips[0],			// Strip
		meteorRegion,			// Region of strip
		innerPixelsChunkLength, // Number of pixels of each region
		startPixel,				// Pixel to start at
		mpColors.teal.pointer,	// Color of meteor core
		1,						// Size of meteor core
		100,					// Meteor tail decay amount
		true,					// Meteor tail random decay
		240,					// Starting hue for tail
		true,					// Forward hue direction for tail color
		0.75,					// Tail hue cycle exponent
		255						// Tail hue saturation start
	);
}

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
void doLetterRegions(char theLetter, int startingPixel)
{
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
			character_array[i] = mpColors.teal.pointer;
			break;
		default:
			character_array[i] = mpColors.off.pointer;
		}
	}

	Adafruit_NeoPixel *&target = allStrips[0];

	int pixel = 0 + startingPixel;

	int previousPixel = startingPixel - letterSpacing;

	for (int i = 0; i < letterTotalPixels; i++)
	{
		int j = i + 1;						// Add 1 to avoid modulus on 0
		int regionInt = j % characterWidth; // Modulus gives an int for the region to draw to
		if (regionInt == 0)
			regionInt = characterWidth; // Modulus of same int gives zero, so assign to last region int
		--regionInt;					// Decrement to un-offset from original offset for modulus calc

		int drawPixel = pixel + (innerPixelsChunkLength * (regionInt));					// Calculate pixel to draw
		int drawPreviousPixel = previousPixel + (innerPixelsChunkLength * (regionInt)); // Calculate trailing pixel after letter to "draw" off value

		int regionStart = (innerPixelsChunkLength * (regionInt + 1)) - innerPixelsChunkLength; // Calculate the pixel before the region start
		int regionEnd = innerPixelsChunkLength * (regionInt + 1);								   // Calculate the pixel after the region end

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
void scrollLetters(string spacecraftName, bool nameChanged)
{
	static int startPixel = 0;

	unsigned int wordSize = spacecraftName.size();
	unsigned int lastArrayIndex = wordSize - 1;

	int letterPixel = startPixel;
	int wrapPixel = innerPixelsChunkLength + (wordSize * (characterHeight + characterKerning));

	for (int i = 0; i < wordSize; i++)
	{

		int previousArrayIndex = i - 1;
		if (previousArrayIndex < 0)
			previousArrayIndex = 0;

		char theLetter = spacecraftName[i];
		doLetterRegions(theLetter, letterPixel);

		letterPixel = letterPixel - letterSpacing - characterKerning;
	}

	startPixel++;

	if (startPixel > wrapPixel)
		startPixel = 0;
}

static int region1StartPixel = 0;
static int region2StartPixel = 0;
static int region3StartPixel = 0;
static int region4StartPixel = 0;
static int region5StartPixel = 0;
static int region6StartPixel = 0;
static int region7StartPixel = 0;
static int region8StartPixel = 0;
static int region9StartPixel = 0;
static int region10StartPixel = 0;
static int region11StartPixel = 0;
static int region12StartPixel = 0;

// Manage meteors
void manageMeteors()
{
	fireMeteor(0, region1StartPixel);
	fireMeteor(1, region2StartPixel);
	fireMeteor(2, region3StartPixel);
	fireMeteor(3, region4StartPixel);
	fireMeteor(4, region5StartPixel);
	fireMeteor(5, region6StartPixel);
	fireMeteor(6, region7StartPixel);
	fireMeteor(7, region8StartPixel);
	fireMeteor(8, region9StartPixel);
	fireMeteor(9, region10StartPixel);
	fireMeteor(10, region11StartPixel);
	fireMeteor(11, region12StartPixel);

	region1StartPixel++;
	region2StartPixel++;
	region3StartPixel++;
	region4StartPixel++;
	region5StartPixel++;
	region6StartPixel++;
	region7StartPixel++;
	region8StartPixel++;
	region9StartPixel++;
	region10StartPixel++;
	region11StartPixel++;
	region12StartPixel++;

	if (region1StartPixel > (innerPixelsChunkLength * 1) + (innerPixelsChunkLength / 2))
		region1StartPixel = 0;
	if (region2StartPixel > (innerPixelsChunkLength * 2) + (innerPixelsChunkLength / 2))
		region2StartPixel = 0;
	if (region3StartPixel > (innerPixelsChunkLength * 3) + (innerPixelsChunkLength / 2))
		region3StartPixel = 0;
	if (region4StartPixel > (innerPixelsChunkLength * 4) + (innerPixelsChunkLength / 2))
		region4StartPixel = 0;
	if (region5StartPixel > (innerPixelsChunkLength * 5) + (innerPixelsChunkLength / 2))
		region5StartPixel = 0;
	if (region6StartPixel > (innerPixelsChunkLength * 6) + (innerPixelsChunkLength / 2))
		region6StartPixel = 0;
	if (region7StartPixel > (innerPixelsChunkLength * 7) + (innerPixelsChunkLength / 2))
		region7StartPixel = 0;
	if (region8StartPixel > (innerPixelsChunkLength * 8) + (innerPixelsChunkLength / 2))
		region8StartPixel = 0;
	if (region9StartPixel > (innerPixelsChunkLength * 9) + (innerPixelsChunkLength / 2))
		region9StartPixel = 0;
	if (region10StartPixel > (innerPixelsChunkLength * 10) + (innerPixelsChunkLength / 2))
		region10StartPixel = 0;
	if (region11StartPixel > (innerPixelsChunkLength * 11) + (innerPixelsChunkLength / 2))
		region11StartPixel = 0;
	if (region12StartPixel > (innerPixelsChunkLength * 12) + (innerPixelsChunkLength / 2))
		region12StartPixel = 0;
}

// Handles updating all animations
void updateAnimation(string spacecraftName, bool nameChanged)
{
	// potentiometerBrightess();
	au.updateBrightness();

	int wordSize = spacecraftName.size();

	// if ((millis() - animationTimer) > 1)
	// {
	// 	//   // hueCycle(*allStrips[0], 10);
	// 	//   // rainbow(*allStrips[0], 10);

	// 	manageMeteors();
	// 	allStrips[0]->show();
	// 	animationTimer = millis(); // Set word timer to current millis()
	// }

	if ((millis() - wordLastTime) > wordScrollInterval)
	{
		scrollLetters(spacecraftName, nameChanged);
		allStrips[0]->show();
		wordLastTime = millis(); // Set word timer to current millis()
	}
}

// Create data structure objects

// Target struct
struct DSN_Target
{
	const char *name;
};
struct DSN_Target blankTarget; // Create a blank placeholder struct

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
struct DSN_Signal blankSignal; // Create a blank placeholder struct

// Dish struct
struct DSN_Dish
{
	const char *name;
	struct DSN_Signal signals[10] = {
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
		blankSignal,
	};
	struct DSN_Target targets[10] = {
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
		blankTarget,
	};
};
struct DSN_Dish blankDish; // Create a blank placeholder struct

// Station struct
struct DSN_Station
{
	uint64_t fetchTimestamp;
	const char *callsign;
	const char *name;
	struct DSN_Dish dishes[10] = {
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
		blankDish,
	};
};

struct DSN_Station stations[3];

// Fetch XML data from HTTP & parse for use in animations
void getData(void *parameter)
{

	for (;;)
	{

		// Send an HTTP POST request every 5 seconds
		if ((millis() - lastTime) > timerDelay)
		{

			if (TEST_CORES == 1)
			{
				Serial.print("getData() running on core ");
				Serial.println(xPortGetCoreID());
			}

			// Reset station data sets
			struct DSN_Station blankStation; // Create a blank placeholder struct

			for (int s = 0; s < 3; s++)
			{
				// stations[s] = blankStation;       // Reset all station array elements with blank structs

				for (int d = 0; d < 10; d++)
				{
					stations[s].dishes[d] = blankDish; // Reset all dish array elements with blank structs

					for (int sig = 0; sig < 10; sig++)
					{
						stations[s].dishes[d].signals[sig] = blankSignal; // Reset all signal array elements with blank structs
					}

					for (int t = 0; t < 10; t++)
					{
						stations[s].dishes[d].targets[t] = blankTarget; // Reset all target array elements with blank structs
					}
				}
			}

			// Check WiFi connection status
			if (WiFi.status() == WL_CONNECTED)
			{

				HTTPClient http;

				String serverPath = serverName + "?r=" + String(random(1410065407));

				if (SHOW_SERIAL == 1)
				{
					Serial.println(serverPath);
				}

				// Your Domain name with URL path or IP address with path
				http.begin(serverPath.c_str());

				// Send HTTP GET request
				int httpResponseCode = http.GET();

				if (httpResponseCode > 0)
				{
					if (SHOW_SERIAL == 1)
					{
						Serial.print("HTTP Response code: ");
						Serial.println(httpResponseCode);
					}

					String payload = http.getString();
					// Serial.println(payload);

					// XML Parsing
					XMLDocument xmlDocument; // Create variable for XML document

					if (xmlDocument.Parse(payload.c_str()) != XML_SUCCESS)
					{ // Handle XML parsing error
						Serial.println("Error parsing");
						return;
					};

					// Find XML elements
					XMLNode *root = xmlDocument.FirstChild();					  // Find document root node
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

						int d2; // Create dish array struct counter
						for (int d2 = 0; d2 < 10; d2++)
						{
							newStation.dishes[d2] = blankDish; // Reset this array struct to blank
						}

						// Find dish elements that are associated with this station
						string dish_string("dish");			   // Convet from string to char, as XMLElement->Value returns char
						const char *compare = &dish_string[0]; // Assign string-to-char for comparison
						int n = 0;							   // Create dish elements counter
						for (XMLElement *xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement())
						{
							const char *elementValue = xmlDish->Value(); // Get element value (tag type)

							if (elementValue != dish_string)
							{ // If the element isn't a dish, exit the loop
								break;
							}

							newStation.dishes[n].name = xmlDish->Attribute("name"); // Add dish name to data struct array

							// Find all signal elements within this dish element
							int sig2 = 0; // Create target elements counter
							for (XMLElement *xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement())
							{
								string upSignal_string("upSignal");
								string downSignal_string("downSignal");
								const char *upSignal_char = &upSignal_string[0];
								const char *downSignal_char = &downSignal_string[0];
								const char *elementValue = xmlSignal->Value();

								if (elementValue != upSignal_string and elementValue != downSignal_string)
								{
									continue;
								}

								newStation.dishes[n].signals[sig2].direction = elementValue;
								newStation.dishes[n].signals[sig2].type = xmlSignal->Attribute("signalType");
								newStation.dishes[n].signals[sig2].frequency = xmlSignal->Attribute("frequency");
								newStation.dishes[n].signals[sig2].rate = xmlSignal->Attribute("dataRate");
								newStation.dishes[n].signals[sig2].spacecraft = xmlSignal->Attribute("spacecraft");
								newStation.dishes[n].signals[sig2].spacecraftId = xmlSignal->Attribute("spacecraftID");

								sig2++;
							}

							// Find all target elements within this dish element
							int t2 = 0; // Create target elements counter
							for (XMLElement *xmlTarget = xmlDish->FirstChildElement("target"); xmlTarget != NULL; xmlTarget = xmlTarget->NextSiblingElement("target"))
							{
								newStation.dishes[n].targets[t2].name = xmlTarget->Attribute("name");
								t2++;
							}

							n++; // Iterate dish element counter
						}

						stations[i] = newStation; // Add data to top-level station struct array
						i++;					  // Iterate station element counter
					}

					// Add data to queue, to be passed to another task
					if (xQueueSend(queue, stations, portMAX_DELAY))
					{
						// success
					}
					else
					{
						if (SHOW_SERIAL == 1)
						{
							Serial.println("Error adding to queue");
						}
					}
				}
				else
				{
					Serial.print("Error code: ");
					Serial.println(httpResponseCode);
				}

				http.end(); // Free up resources
			}
			else
			{
				Serial.println("WiFi Disconnected");

				WiFi.begin(ssid, password);
				Serial.println("Connecting");
				// while(WiFi.status() != WL_CONNECTED) {
				//   Serial.print(".");
				// }
				Serial.println("");
				Serial.print("Connected to WiFi network with IP Address: ");
				Serial.println(WiFi.localIP());
			}

			lastTime = millis(); // Sync reference variable for timer
		}
	}
}

// setup() function -- runs once at startup --------------------------------
void setup()
{
	Serial.begin(115200); // Begin serial communications, ESP32 uses 115200 rate

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
	allStripsShow();

	// Identify Neopixel strips by filling with unique colors
	if (ID_LEDS == 1)
	{
		Serial.println("ID LED strips");
		const uint32_t *colors[] = {mpColors.red.pointer, mpColors.green.pointer, mpColors.blue.pointer, mpColors.purple.pointer, mpColors.white.pointer};

		for (int c = 0; c < sizeof(colors) / sizeof(colors[0]); c++)
		{
			for (int i = 0; i < allStripsLength; i++)
			{
				allStrips[i]->fill(*colors[c]);
			}
			allStripsShow();
			delay(1000);
		}

		outer_pixels.fill(*mpColors.blue.pointer);
		inner_pixels.fill(*mpColors.green.pointer);
		middle_pixels.fill(*mpColors.red.pointer);
		bottom_pixels.fill(*mpColors.purple.pointer);

		allStripsShow();
		delay(2000);
		allStripsOff();
	}

	if (DISABLE_WIFI == 0)
	{
		// Connect to WiFi
		wifiSetup();
		delay(100);
		printWifiMode();
		scanWifiNetworks(); // Scan for available WiFi networks
		delay(1000);
		printWifiStatus(); // Print WiFi status
		delay(1000);

		WiFi.begin(ssid, password); // Attempt to connect to WiFi
		Serial.println("Connecting...");
		while (WiFi.waitForConnectResult() != WL_CONNECTED)
		{
			delay(1000);
			Serial.print(".");
		}
		delay(1000);
		Serial.print("hostname: ");
		Serial.println(WiFi.getHostname());
		Serial.println();
		printWifiStatus();
		Serial.print("Connected to WiFi network with IP Address: ");
		Serial.println(WiFi.localIP());
		delay(1000);

		serverSetup();
		delay(1000);
	}

	// Initialize task for core 1
	xTaskCreatePinnedToCore(
		getData,	 /* Function to implement the task */
		"getData",	 /* Name of the task */
		10000,		 /* Stack size in words */
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
}

// string spacecraftName = "abcdefghijklmnopqrstuvwxyz";
string spacecraftName = "nasa voyager";

bool nameChanged = true;

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

	// Receive from queue (data task on core 1)
	if (xQueueReceive(queue, &stations, 1) == pdPASS)
	{
		if (SHOW_SERIAL == 1)
		{
			Serial.println();
			Serial.println("-------");
			Serial.print("timestamp: ");
			Serial.println(stations[0].fetchTimestamp);
			Serial.println();

			int i;
			for (i = 0; i < 3; i++)
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

								if (t == 0)
									spacecraftName = targetName;
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

	server.handleClient();

	updateAnimation(spacecraftName, nameChanged);
	nameChanged = false;

	// if ( millis() - lastUpdateP1 > pattern1Interval ) updatePattern1();
	// if( millis() - lastUpdateP2 > pattern2Interval ) updatePattern2();
	// if( millis() - lastUpdateP3 > pattern3Interval ) updatePattern3();
	// if( millis() - lastUpdateP4 > pattern4Interval ) updatePattern4();

	// Fill along the length of the strip in various colors...
	// colorWipe(outer_pixels, outer_pixels.Color(0,   0,   255), 50); // Red
	// colorWipe(inner_pixels, inner_pixels.Color(255,   0,   0), 50); // Red
	// colorWipe(middle_pixels, middle_pixels.Color(255,   0,   0), 50); // Red
	// colorWipe(bottom_pixels, bottom_pixels.Color(255,   0,   0), 50); // Red

	// colorWipe(outer_pixels, outer_pixels.Color(  0, 255,   0), 50); // Green
	// colorWipe(inner_pixels, inner_pixels.Color(  0, 255,   0), 50); // Green
	// colorWipe(middle_pixels, middle_pixels.Color(  0, 255,   0), 50); // Green
	// colorWipe(bottom_pixels, bottom_pixels.Color(  0, 255,   0), 50); // Green

	// colorWipe(outer_pixels, outer_pixels.Color(  0,   0, 255), 50); // Blue
	// colorWipe(inner_pixels, inner_pixels.Color(  0,   0, 255), 50); // Blue
	// colorWipe(middle_pixels, middle_pixels.Color(  0,   0, 255), 50); // Blue
	// colorWipe(bottom_pixels, bottom_pixels.Color(  0,   0, 255), 50); // Blue

	// Do a theater marquee effect in various colors...
	// theaterChase(outer_pixels, outer_pixels.Color(127, 127, 127), 50); // White, half brightness
	// theaterChase(inner_pixels, inner_pixels.Color(127, 127, 127), 50); // White, half brightness
	// theaterChase(middle_pixels, middle_pixels.Color(127, 127, 127), 50); // White, half brightness
	// theaterChase(bottom_pixels, bottom_pixels.Color(127, 127, 127), 50); // White, half brightness

	// theaterChase(outer_pixels, outer_pixels.Color(127,   0,   0), 50); // Red, half brightness
	// theaterChase(inner_pixels, inner_pixels.Color(127,   0,   0), 50); // Red, half brightness
	// theaterChase(middle_pixels, middle_pixels.Color(127,   0,   0), 50); // Red, half brightness
	// theaterChase(bottom_pixels, bottom_pixels.Color(127,   0,   0), 50); // Red, half brightness

	// theaterChase(outer_pixels, outer_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
	// theaterChase(inner_pixels, inner_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
	// theaterChase(middle_pixels, middle_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
	// theaterChase(bottom_pixels, bottom_pixels.Color(  0,   0, 127), 50); // Blue, half brightness

	// rainbow(outer_pixels, 10);             // Flowing rainbow cycle along the whole strip
	// rainbow(inner_pixels, 10);             // Flowing rainbow cycle along the whole strip
	// rainbow(middle_pixels, 10);             // Flowing rainbow cycle along the whole strip
	// rainbow(bottom_pixels, 10);             // Flowing rainbow cycle along the whole strip

	// theaterChaseRainbow(outer_pixels, 50); // Rainbow-enhanced theaterChase variant
	// theaterChaseRainbow(inner_pixels, 50); // Rainbow-enhanced theaterChase variant
	// theaterChaseRainbow(middle_pixels, 50); // Rainbow-enhanced theaterChase variant
	// theaterChaseRainbow(bottom_pixels, 50); // Rainbow-enhanced theaterChase variant
}
