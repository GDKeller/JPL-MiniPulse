/* LIBRARIES */
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <tinyxml2.h>
#include <iostream>
#include <cstring>
#include <TextCharacters.h>


/* NAMESPACES */
using namespace tinyxml2;
using namespace std;



/* CONFIG */
const char* ssid = "Verizon_LT6SKN";
const char* password = "smile-grey9-hie";
#define AP_SSID  "esp32-v6"

#define TEST_CORES 0
#define SHOW_SERIAL 0

#define OUTER_PIN 17
#define MIDDLE_PIN 18
#define INNER_PIN 19
#define BOTTOM_PIN 16

#define WIFI_RST 21
#define OUTPUT_ENABLE 22
#define BRIGHTNESS 127        // Global brightness value. 8bit, 0-255

String serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml"; // URL to fetch

// Time is measured in milliseconds and will become a bigger number
// than can be stored in an int, so long is used
unsigned long lastTime = 0;       // Init reference variable for timer
unsigned long wordLastTime = 0;
unsigned long timerDelay = 5000;  // Set timer to 5 seconds (5000)

// how often each pattern updates
unsigned long pattern1Interval  = 500;
unsigned long pattern2Interval  = 500;
unsigned long pattern3Interval  = 500;
unsigned long pattern4Interval  = 500;


/* TASKS */
TaskHandle_t  HandleData;     // Task for fetching and parsing data
QueueHandle_t queue;          // Queue to pass data between tasks



/* CLASSES */
Adafruit_NeoPixel neopixel;
TextCharacter textCharacter;




/* NETWORKING */
static volatile bool wifi_connected = false;
const char* rssiToString(uint rssi) {
  if (rssi > -31) return "Amazing";
  if (rssi > -68) return "Very Good";
  if (rssi > -71) return "Okay";
  if (rssi > -81) return "Not Good";
  if (rssi > -91) return "Unusable";
  return "Unknown";
}

void wifiOnConnect(){
    Serial.println("STA Connected");
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

void wifiOnDisconnect(){
    Serial.println("STA Disconnected");
    delay(1000);
    WiFi.begin(ssid, password);
}

void WiFiEvent(WiFiEvent_t event){
    switch(event) {

        case ARDUINO_EVENT_WIFI_AP_START:
            //can set ap hostname here
            WiFi.softAPsetHostname(AP_SSID);
            //enable ap ipv6 here
            WiFi.softAPenableIpV6();
            break;

        case ARDUINO_EVENT_WIFI_STA_START:
            //set sta hostname here
            WiFi.setHostname(AP_SSID);
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            //enable sta ipv6 here
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

void scanWifiNetworks() {
  Serial.println("Scanning available WiFi networks...");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete");
  if (n == 0) {
      Serial.println("No networks found");
  } else {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          Serial.print(WiFi.encryptionType(i));
          Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
          delay(10);
      }
  }
  Serial.println("");
  delay(500);
}

void printWifiMode() {
    Serial.print("WiFi Mode: "); Serial.println(WiFi.getMode()); Serial.println();
    delay(100);
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "WiFi status code unknown";
  }
}

void printWifiStatus() {
  Serial.print("WiFi Status: "); Serial.println(wl_status_to_string(WiFi.status()));
}

void wifiSetup() {  
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(AP_SSID);
}



/* HARDWARDE */
// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
Adafruit_NeoPixel
  outer_pixels(10, OUTER_PIN, NEO_GRB + NEO_KHZ800),
  middle_pixels(10, MIDDLE_PIN, NEO_GRB + NEO_KHZ800),
  inner_pixels(10, INNER_PIN, NEO_GRB + NEO_KHZ800),
  bottom_pixels(10, BOTTOM_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel * allStrips[4] = {
  &bottom_pixels,
  &outer_pixels,
  &middle_pixels,
  &inner_pixels,
};




// const uint32_t bgrRed = Adafruit_NeoPixel::Color((uint8_t)0,  (uint8_t)0,  (uint8_t)255);
const uint32_t bgrBlue = Adafruit_NeoPixel::Color((uint8_t)255,  (uint8_t)0,  (uint8_t)0);
const uint32_t* pBgrBlue = &bgrBlue;
const uint32_t bgrGreen = Adafruit_NeoPixel::Color((uint8_t)25,  (uint8_t)25,  (uint8_t)25);
const uint32_t* pBgrGreen = &bgrGreen;
const uint32_t bgrOff = Adafruit_NeoPixel::Color((uint8_t)0,  (uint8_t)0,  (uint8_t)0);
const uint32_t* pBgrOff = &bgrOff;
uint32_t bgrPurple = Adafruit_NeoPixel::Color((uint8_t)64,  (uint8_t)0,  (uint8_t)64);
uint32_t off = Adafruit_NeoPixel::Color((uint8_t)0,  (uint8_t)0,  (uint8_t)0);  // Variable for LED off state
uint32_t bgrWhite = Adafruit_NeoPixel::Color((uint8_t)255,  (uint8_t)255,  (uint8_t)255); // Full white color
uint32_t* pBgrWhite = &bgrWhite;






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



// Adafruit_NeoPixel ledsOuter = outer_pixels;
// Adafruit_NeoPixel * ledsMiddle = &middle_pixels;
// Adafruit_NeoPixel * ledsInner = &inner_pixels;
// Adafruit_NeoPixel * ledsBottom = &bottom_pixels;

// Adafruit_NeoPixel stripsArray[] = {
//   outer_pixels,
//   middle_pixels,
//   inner_pixels,
//   bottom_pixels 
// };








/* ANIMATION FUNCTIONS */

// Custom function to calculate color relative to global BRIGHTNESS variable 
void setPixelColor( Adafruit_NeoPixel &strip, uint16_t n, uint32_t color ) {
  uint8_t rgb[4];             // Create array that will hold color channel values
  *(uint32_t*)&rgb = color;   // Assigns color value to the color channel array
  uint8_t channelR = rgb[0];         // blue color channel value
  uint8_t channelG = rgb[1];         // Green color channel value
  uint8_t channelB = rgb[2];         // Blue color channel value
  // Serial.println(r);
  // Serial.println(g);
  // Serial.println(b);
  // Serial.println();
	strip.setPixelColor(n, ((BRIGHTNESS*channelR)/255) , ((BRIGHTNESS*channelG)/255), ((BRIGHTNESS*channelB)/255));
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(Adafruit_NeoPixel &strip, uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) {  // For each pixel in strip...
    setPixelColor(strip, i, color);         //  Set pixel's color (in RAM)
    strip.show();                           //  Update strip to match
    delay(wait);                            //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(Adafruit_NeoPixel &strip, uint32_t color, int wait) {
  for(int a=0; a<10; a++) {                 // Repeat 10 times...
    for(int b=0; b<3; b++) {                //  'b' counts from 0 to 2...
      strip.clear();                        //   Set all pixels in RAM to 0 (off)
      
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        setPixelColor(strip, c, color);     // Set pixel 'c' to value 'color'
      }
      strip.show();                         // Update strip with new contents
      delay(wait);                          // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(Adafruit_NeoPixel &strip, int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue, 1, 255, BRIGHTNESS, true);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(Adafruit_NeoPixel &strip, int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        setPixelColor(strip, c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}





// Display letter from array
void doLetter(char theLetter, int startingPixel) {
  // Serial.println(startingPixel);

  const uint32_t nope = 0;
  const uint32_t* pNope = &nope;

  int * ledCharacter = textCharacter.getCharacter(theLetter);


  const uint32_t* character_array[20] = {};

  for (int i = 0; i < 20; i++) {
    // Serial.println(ledCharacter[i]);
    switch (ledCharacter[i]) {
      case 0:
        character_array[i] = pNope;
        break;
      case 1:
        character_array[i] = pBgrBlue;
        break;
      default:
        character_array[i] = pNope;
    }

    const uint32_t dim = Adafruit_NeoPixel::Color(20, 20, 20);
    const uint32_t* pDim = &dim;
    if (*character_array[i] > *pBgrWhite) character_array[i] = pDim;
  }



  Serial.print("Blue: "); Serial.print(*pBgrBlue); Serial.print(", ");
  Serial.print("Off: "); Serial.print(*pBgrOff); Serial.print(", ");
  Serial.print("Green: "); Serial.print(*pBgrGreen); Serial.println();
  
  int pixel = 0 + startingPixel;
  int previousPixel = startingPixel - 1;


  for (int i = 0; i < 20; i++) {

    int j = i + 1;
    int stripInt = j % 4;
    if (stripInt == 0) stripInt = 4;
    --stripInt;
    
    

    // Adafruit_NeoPixel*& target = allStrips[stripInt];

    // if (stripInt == 0) {
    //   // Serial.print(*character_array[i]); Serial.print("/"); Serial.print(pixel); Serial.print(", ");
    //   if (-1 < previousPixel < 10) setPixelColor(*target, previousPixel, nope);
    //   if (-1 < pixel < 10) setPixelColor(*target, pixel, *character_array[i]);
    // }
    // if (stripInt == 1) {
    //   Serial.print(*character_array[i]); Serial.print("/"); Serial.print(pixel); Serial.print(", ");
    //   if (-1 < previousPixel < 10) setPixelColor(*target, previousPixel, nope);
    //   if (-1 < pixel < 10) setPixelColor(*target, pixel, *character_array[i]);
    // }
    // if (stripInt == 2) {
    //   // Serial.print(*character_array[i]); Serial.print("/"); Serial.print(pixel); Serial.print(", ");
    //   if (-1 < previousPixel < 10) setPixelColor(*target, previousPixel, nope);
    //   if (-1 < pixel < 10) setPixelColor(*target, pixel, *character_array[i]);
    // }
    // if (stripInt == 3) {
    //   // Serial.print(*character_array[i]); Serial.print("/"); Serial.println(pixel); Serial.println();
    //   if (-1 < previousPixel < 10) setPixelColor(*target, previousPixel, nope);
    //   if (-1 < pixel < 10) setPixelColor(*target, pixel, *character_array[i]);
      
    //   pixel++;  // Move to next pixel
    // }
  }

  Serial.println();
  
  // for (int p = 0; p < 4; p++) {
  //   allStrips[p]->show();
  // }
}






bool initStartingPixels = true;
void scrollLetters(string spacecraftName) {
  
  unsigned int wordSize = spacecraftName.size();
  unsigned int lastArrayIndex = wordSize - 1;

    int initLetterStartingPixels[] = {};
    int letterStartingPixels[] = {};


  // TODO: Refactor this so that starting pixels are calculated as an offset of previous letter, instead of multiplied
  for (int i = 0; i < wordSize; i++) {
     

    int refLetterPixel;
    int previousArrayIndex = i - 1;
    if (previousArrayIndex < 0) previousArrayIndex = 0;

    if (initStartingPixels == true) {
      unsigned int letterSpacing = 6 * i;
      initLetterStartingPixels[i] = -5 - letterSpacing;
      letterStartingPixels[i] = initLetterStartingPixels[i];
    }


    Serial.print(letterStartingPixels[i]); Serial.print(", ");
    Serial.println();
  
    int startingPixel = letterStartingPixels[i];
    char theLetter = spacecraftName[i];

    // Serial.print(startingPixel); Serial.print(", ");
    // Serial.println();
    doLetter(theLetter, startingPixel);

    startingPixel = startingPixel + 1;

    if (startingPixel > 10) {      
      if (i == 0) {
        refLetterPixel = letterStartingPixels[lastArrayIndex];
        startingPixel = refLetterPixel - 12;
      } else {
        
        refLetterPixel = letterStartingPixels[previousArrayIndex];
        startingPixel = refLetterPixel - 6; 
      }      
    }

    letterStartingPixels[i] = startingPixel;
  
  }
  Serial.println();

  initStartingPixels = false;
  lastUpdateP1 = millis();
}


// void updatePattern1() { // rain
//   uint32_t colorArray [] = {
//     outer_pixels.Color(0, 0, 255),
//     outer_pixels.Color(0, 2, 24),
//     outer_pixels.Color(0, 2, 8),
//     outer_pixels.Color(0, 1, 2)
//   };

//   int colorArrayLength = sizeof(colorArray) / sizeof(int);
//   int numPixels = outer_pixels.numPixels() + 8;
//   int numPixelsExtend = numPixels + colorArrayLength;  
  
//   setPixelColor(outer_pixels, p1State, colorArray[0]); // turn on next led in pattern
  
//   if ( 0 < p1StateExtend && p1StateExtend <= numPixels ) {
//     // second
//     int secondLedExtend = p1StateExtend - 1;
//     if (secondLedExtend < 0) {               // wrap round count
//       secondLedExtend = numPixelsExtend - 1;
//     }
//     setPixelColor(outer_pixels, secondLedExtend, colorArray[1]);     // turn off last LED we set

//     // third
//     int thirdLedExtend = p1StateExtend - 2;
//     if (thirdLedExtend < 0) {               // wrap round count
//       thirdLedExtend = numPixelsExtend - 2;
//     }
//     setPixelColor(outer_pixels, thirdLedExtend, colorArray[2]);     // turn off last LED we set

//     // fourth
//     int fourthLedExtend = p1StateExtend - 3;
//     if (fourthLedExtend < 0) {               // wrap round count
//       fourthLedExtend = numPixelsExtend - 3;
//     }
//     setPixelColor(outer_pixels, fourthLedExtend, colorArray[3]);     // turn off last LED we set

//     // last
//     int lastLedExtend = p1StateExtend - colorArrayLength;        // find LED to turn off
//     if (lastLedExtend < 0) {               // wrap round count
//       lastLedExtend = numPixelsExtend - colorArrayLength;
//     }
//     setPixelColor(outer_pixels, lastLedExtend, off);     // turn off last LED we set


//   } else {
//     // second
//     int secondLed = p1State - 1;
//     if (secondLed < 0) {               // wrap round count
//       secondLed = numPixels - 1;
//     }
//     setPixelColor(outer_pixels, secondLed, colorArray[1]);     // turn off last LED we set

//     // third
//     int thirdLed = p1State - 2;
//     if (thirdLed < 0) {               // wrap round count
//       thirdLed = numPixels - 2;
//     }
//     setPixelColor(outer_pixels, thirdLed, colorArray[2]);     // turn off last LED we set

//     // fourth
//     int fourthLed = p1State - 3;
//     if (fourthLed < 0) {               // wrap round count
//       fourthLed = numPixels - 3;
//     }
//     setPixelColor(outer_pixels, fourthLed, colorArray[3]);     // turn off last LED we set

//     // last
//     int lastLed = p1State - colorArrayLength;        // find LED to turn off
//     if (lastLed < 0) {               // wrap round count
//       lastLed = numPixels - colorArrayLength;
//     }
//     setPixelColor(outer_pixels, lastLed, off);     // turn off last LED we set
//   }

  

//   if ( p1State == numPixels - 1 ) {
//     p1StateExtend = p1State;
//   }


//   p1State ++;                 // move on state variable for the next time we enter this
//   if (p1State >= numPixels){   // wrap round the state
//   p1State = 0;
//   }

//   if ( p1StateExtend != 0 ) {
//     p1StateExtend++;
//   }  
//   if ( p1StateExtend >= numPixelsExtend ) {
//     p1StateExtend = 0;
//   }

//   outer_pixels.show(); // update display
//   lastUpdateP1 = millis();               // to calculate next update
// }

// void updatePattern2() { // pattern 1 a walking green led
//   uint32_t color2 = inner_pixels.Color(0, 64, 0);
//   uint32_t offColor = inner_pixels.Color(0, 0, 0);
//   setPixelColor(inner_pixels, p2State, color2); // turn on next led in pattern
//   int lastLed2 = p2State - 1;        // find LED to turn off
//   if (lastLed2 < 0) {               // wrap round count
//     lastLed2 = inner_pixels.numPixels() - 1;
//   }
//   setPixelColor(inner_pixels, lastLed2, off);     // turn off last LED we set
//   p2State ++;                 // move on state variable for the next time we enter this
//   if(p2State >= inner_pixels.numPixels()){   // wrap round the state
//   p2State = 0;
//   }
//   inner_pixels.show(); // update display
//   lastUpdateP2 = millis();               // to calculate next update
// }

// void updatePattern3() { // pattern 1 a walking blue led
//   uint32_t color2 = middle_pixels.Color(64, 0, 0);
//   uint32_t offColor = middle_pixels.Color(0, 0, 0);
//   setPixelColor(middle_pixels, p3State, color2); // turn on next led in pattern
//   int lastLed2 = p3State - 1;        // find LED to turn off
//   if (lastLed2 < 0) {               // wrap round count
//     lastLed2 = middle_pixels.numPixels() - 1;
//   }
//   setPixelColor(middle_pixels, lastLed2, off);     // turn off last LED we set
//   p3State ++;                 // move on state variable for the next time we enter this
//   if(p3State >= middle_pixels.numPixels()){   // wrap round the state
//   p3State = 0;
//   }
//   middle_pixels.show(); // update display
//   lastUpdateP3 = millis();               // to calculate next update
// }

// void updatePattern4() { // pattern 1 a walking purple led
//   uint32_t color2 = bottom_pixels.Color(32, 0, 32);
//   uint32_t offColor = bottom_pixels.Color(0, 0, 0);
//   setPixelColor(bottom_pixels, p4State, color2); // turn on next led in pattern
//   int lastLed2 = p4State - 1;        // find LED to turn off
//   if (lastLed2 < 0) {               // wrap round count
//     lastLed2 = bottom_pixels.numPixels() - 1;
//   }
//   setPixelColor(bottom_pixels, lastLed2, off);     // turn off last LED we set
//   p4State ++;                 // move on state variable for the next time we enter this
//   if(p4State >= bottom_pixels.numPixels()){   // wrap round the state
//   p4State = 0;
//   }
//   bottom_pixels.show(); // update display
//   lastUpdateP4 = millis();               // to calculate next update
// }




// Create data structure objects

// Target struct
struct DSN_Target {
  const char * name;
};
struct DSN_Target blankTarget;      // Create a blank placeholder struct

// Signal Struct
struct DSN_Signal {
  const char * direction;
  const char * type;
  const char * rate;
  const char * frequency;
  const char * spacecraft;
  const char * spacecraftId;
};
struct DSN_Signal blankSignal;      // Create a blank placeholder struct

// Dish struct
struct DSN_Dish {
  const char * name;
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
struct DSN_Dish blankDish;          // Create a blank placeholder struct

// Station struct
struct DSN_Station {
  uint64_t fetchTimestamp;
  const char * callsign;
  const char * name;
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
void getData( void * parameter) {

  for(;;) {

    // Send an HTTP POST request every 5 seconds
    if ((millis() - lastTime) > timerDelay) {

      if ( TEST_CORES == 1 ) {
        Serial.print("getData() running on core ");
        Serial.println(xPortGetCoreID());
      }

      // Reset station data sets
      struct DSN_Station blankStation;    // Create a blank placeholder struct
      

      for (int s = 0; s < 3; s++) {
        // stations[s] = blankStation;       // Reset all station array elements with blank structs

        for (int d = 0; d < 10; d++) {
          stations[s].dishes[d] = blankDish;  // Reset all dish array elements with blank structs

          for (int sig = 0; sig < 10; sig++) {
            stations[s].dishes[d].signals[sig] = blankSignal; // Reset all signal array elements with blank structs
          }

          for (int t = 0; t < 10; t++) {
            stations[s].dishes[d].targets[t] = blankTarget; // Reset all target array elements with blank structs
          }
        }
      }



      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){

        HTTPClient http;

        String serverPath = serverName + "?r=" + String(random(1410065407));
        
        if ( SHOW_SERIAL == 1 ) {
          Serial.println(serverPath);
        }

        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) {
          if ( SHOW_SERIAL == 1 ) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
          }

          String payload = http.getString();
          // Serial.println(payload);

          // XML Parsing
          XMLDocument xmlDocument;  // Create variable for XML document

          if(xmlDocument.Parse(payload.c_str())!= XML_SUCCESS){   // Handle XML parsing error
            Serial.println("Error parsing");  
            return;
          };

          // Find XML elements          
          XMLNode * root = xmlDocument.FirstChild();      // Find document root node
          XMLElement * timestamp = root->FirstChildElement("timestamp");  // Find XML timestamp element

          uint64_t timestampInt;      
          timestamp->QueryUnsigned64Text(&timestampInt);  // Convert timestamp to int

         
          DSN_Station newStation;
          int i = 0;    // Create station elements counter
          for (XMLElement * xmlStation = root->FirstChildElement("station"); xmlStation != NULL; xmlStation = xmlStation->NextSiblingElement("station")) {
            newStation.fetchTimestamp = timestampInt;
            newStation.callsign = xmlStation->Attribute("name");
            newStation.name = xmlStation->Attribute("friendlyName");

            int d2;   // Create dish array struct counter
            for (int d2 = 0; d2 < 10; d2++) {
              newStation.dishes[d2] = blankDish;  // Reset this array struct to blank
            }


            // Find dish elements that are associated with this station
            string dish_string("dish");   // Convet from string to char, as XMLElement->Value returns char
            const char * compare = &dish_string[0];    // Assign string-to-char for comparison
            int n = 0;    // Create dish elements counter
            for (XMLElement * xmlDish = xmlStation->NextSiblingElement(); xmlDish != NULL; xmlDish = xmlDish->NextSiblingElement()) {
              const char * elementValue = xmlDish->Value(); // Get element value (tag type)

              if (elementValue != dish_string) {    // If the element isn't a dish, exit the loop
                break;
              }

              newStation.dishes[n].name = xmlDish->Attribute("name");   // Add dish name to data struct array


              // Find all signal elements within this dish element
              int sig2 = 0;   // Create target elements counter
              for (XMLElement * xmlSignal = xmlDish->FirstChildElement(); xmlSignal != NULL; xmlSignal = xmlSignal->NextSiblingElement()) {
                string upSignal_string("upSignal");
                string downSignal_string("downSignal");
                const char * upSignal_char = &upSignal_string[0];
                const char * downSignal_char = &downSignal_string[0];
                const char * elementValue = xmlSignal->Value();

                if (elementValue != upSignal_string and elementValue != downSignal_string) {
                  continue;
                }

                newStation.dishes[n].signals[sig2].direction = elementValue;
                newStation.dishes[n].signals[sig2].type = xmlSignal->Attribute("signalType");
                newStation.dishes[n].signals[sig2].frequency = xmlSignal->Attribute("frequency");
                newStation.dishes[n].signals[sig2].rate = xmlSignal->Attribute("dataRate");
                newStation.dishes[n].signals[sig2].spacecraft = xmlSignal->Attribute("spacecraft");
                newStation.dishes[n].signals[sig2].spacecraftId = xmlSignal->Attribute("spacecraftID");
                
                sig2 ++;
              }



              // Find all target elements within this dish element
              int t2 = 0;   // Create target elements counter
              for (XMLElement * xmlTarget = xmlDish->FirstChildElement("target"); xmlTarget != NULL; xmlTarget = xmlTarget->NextSiblingElement("target")) {
                newStation.dishes[n].targets[t2].name = xmlTarget->Attribute("name");
                t2 ++;
              }

              n++;    // Iterate dish element counter
            }

            stations[i] = newStation; // Add data to top-level station struct array
            i++;    // Iterate station element counter
          }






          // Add data to queue, to be passed to another task
          if (xQueueSend( queue, stations, portMAX_DELAY )) {
            // success
          } else {
            if ( SHOW_SERIAL == 1 ) {
              Serial.println("Error adding to queue");
            }
          }
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }

        http.end(); // Free up resources
      }
      else {
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

      lastTime = millis();  // Sync reference variable for timer
    }
  }
}








// setup() function -- runs once at startup --------------------------------
void setup() {
  Serial.begin(115200);     // Begin serial communications, ESP32 uses 115200 rate

  if ( TEST_CORES == 1 ) {
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
  digitalWrite(OUTPUT_ENABLE, HIGH);

  // Initialize NeoPixel objects
  outer_pixels.begin();
  inner_pixels.begin();
  middle_pixels.begin();
  bottom_pixels.begin();

  // Turn off all NeoPixels
  outer_pixels.show();
  inner_pixels.show();
  middle_pixels.show();
  bottom_pixels.show();

  // Set brightness
  outer_pixels.setBrightness(BRIGHTNESS);
  inner_pixels.setBrightness(BRIGHTNESS);
  middle_pixels.setBrightness(BRIGHTNESS);
  bottom_pixels.setBrightness(BRIGHTNESS);

  // Connect to WiFi
  wifiSetup();
  printWifiMode();
  scanWifiNetworks();                     // Scan for available WiFi networks
  delay(1000);
  printWifiStatus();                      // Print WiFi status
  delay(1000);
 
 
  WiFi.begin(ssid, password);             // Attempt to connect to WiFi
  Serial.println("Connecting...");
  while(WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  delay(1000);
  Serial.println();
  printWifiStatus();
  Serial.print("Connected to WiFi network with IP Address: "); Serial.println(WiFi.localIP());
  delay(1000);



  // Initialize task for core 1
  xTaskCreatePinnedToCore(
    getData, /* Function to implement the task */
    "getData", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    0,  /* Priority of the task */
    &HandleData,  /* Task handle. */
    0); /* Core where the task should run */


  // Create queue to pass data between tasks on separate cores
  queue = xQueueCreate( 1, sizeof( uint64_t ) );    // Create queue

  if (queue == NULL) {                              // Check that queue was created successfully
    Serial.println("Error creating the queue");
  }

}


// string spacecraftName = "abcdefghijklmnopqrstuvwxyz";
string spacecraftName = "grant";


// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
    
  if ( TEST_CORES == 1 ) {    
    if ( millis() - lastTime > 4000 && millis() - lastTime < 4500 ) {
      Serial.print("loop() running on core ");
      Serial.println(xPortGetCoreID());
    }
  }

  // Receive from queue (data task on core 1)
  // struct DSN_Stations stations[3];
  if (xQueueReceive(queue, &stations, 1) == pdPASS) {
    if ( SHOW_SERIAL == 1 ) {
      Serial.println();
      Serial.println("-------");
      Serial.print("timestamp: ");
      Serial.println(stations[0].fetchTimestamp);
      Serial.println();

      int i;
      for (i = 0; i < 3; i++) {
        DSN_Station station = stations[i];
        Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        Serial.print("Station: ");
        Serial.print(station.name);
        Serial.print(" (");
        Serial.print(station.callsign);
        Serial.println(") ");
        Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

        for (int d = 0; d < 10; d++) {
          const char * dishName = station.dishes[d].name;          
          
          if (dishName != NULL) {

            Serial.print("> Dish: ");
            Serial.println(dishName);


            for (int t = 0; t < 10; t++) {
              const char * targetName = station.dishes[d].targets[t].name;

              if (targetName != NULL) {
                Serial.print("  Target: ");
                Serial.println(targetName);
              }
            }

            for (int sig = 0; sig < 10; sig++) {
              if (not station.dishes[d].signals[sig].direction) {
                break;
              }
              const char * direction = station.dishes[d].signals[sig].direction;
              const char * type = station.dishes[d].signals[sig].type;
              const char * rate = station.dishes[d].signals[sig].rate;
              const char * frequency = station.dishes[d].signals[sig].frequency;
              const char * spacecraft = station.dishes[d].signals[sig].spacecraft;
              const char * spacecraftId = station.dishes[d].signals[sig].spacecraftId;
              
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

  

  if ( (millis() - wordLastTime) > 500) {
    scrollLetters(spacecraftName);

    Serial.println();    
    wordLastTime = millis();    // Set word timer to current millis()
  }


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
