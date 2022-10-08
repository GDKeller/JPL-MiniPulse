#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <tinyxml2.h>
#include <iostream>
#include <cstring>

using namespace tinyxml2;
using namespace std;

const char* ssid = "PlanetExpress";
const char* password = "futurama";

char * testDocument = "<root><element>7</element></root>";

#define OUTER_PIN 17
#define MIDDLE_PIN 18
#define INNER_PIN 19
#define BOTTOM_PIN 16

#define WIFI_RST 21
#define OUTPUT_ENABLE 22
#define BRIGHTNESS 255        // Global brightness value. 8bit, 0-255


TaskHandle_t  HandleData;






//Your Domain name with URL path or IP address with path
String serverName = "https://eyes.nasa.gov/dsn/data/dsn.xml";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;





// how often each pattern updates
unsigned long pattern1Interval  = 50;
unsigned long pattern2Interval  = 50;
unsigned long pattern3Interval  = 50;
unsigned long pattern4Interval  = 50;

// for millis() when last update occurred
unsigned long lastUpdateP1 = 0;
unsigned long lastUpdateP2 = 0;
unsigned long lastUpdateP3 = 0;
unsigned long lastUpdateP4 = 0;

// state variables for patterns
int p1State = 0;
int p1StateExtend = 0;
int p2State = 0;
int p3State = 0; 
int p4State = 0;




// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
Adafruit_NeoPixel
  outer_pixels(20, OUTER_PIN, NEO_GRB + NEO_KHZ800),
  inner_pixels(20, INNER_PIN, NEO_GRB + NEO_KHZ800),
  middle_pixels(20, MIDDLE_PIN, NEO_GRB + NEO_KHZ800),
  bottom_pixels(20, BOTTOM_PIN, NEO_GRB + NEO_KHZ800);

uint32_t offColor = outer_pixels.Color(0, 0, 0);



// Custom function to calculate color relative to global BRIGHTNESS variable 
void setPixelColor( Adafruit_NeoPixel &strip, uint16_t n, uint32_t &color ) {
  uint8_t rgb[4];             // Create array that will hold color channel values
  *(uint32_t*)&rgb = color;   // Assigns color value to the color channel array
  uint8_t r = rgb[0];         // Red color channel value
  uint8_t g = rgb[1];         // Green color channel value
  uint8_t b = rgb[2];         // Blue color channel value
  // Serial.println(r);
  // Serial.println(g);
  // Serial.println(b);
  // Serial.println();
	strip.setPixelColor(n, (BRIGHTNESS*r/255) , (BRIGHTNESS*g/255), (BRIGHTNESS*b/255));
}


// Animation functions -----------------

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


void updatePattern1() { // rain
  uint32_t colorArray [] = {
    outer_pixels.Color(0, 0, 255),
    outer_pixels.Color(0, 2, 24),
    outer_pixels.Color(0, 2, 8),
    outer_pixels.Color(0, 1, 2)
  };

  int colorArrayLength = sizeof(colorArray) / sizeof(int);
  int numPixels = outer_pixels.numPixels() + 8;
  int numPixelsExtend = numPixels + colorArrayLength;  
  
  setPixelColor(outer_pixels, p1State, colorArray[0]); // turn on next led in pattern
  
  if ( 0 < p1StateExtend && p1StateExtend <= numPixels ) {
    // second
    int secondLedExtend = p1StateExtend - 1;
    if (secondLedExtend < 0) {               // wrap round count
      secondLedExtend = numPixelsExtend - 1;
    }
    setPixelColor(outer_pixels, secondLedExtend, colorArray[1]);     // turn off last LED we set

    // third
    int thirdLedExtend = p1StateExtend - 2;
    if (thirdLedExtend < 0) {               // wrap round count
      thirdLedExtend = numPixelsExtend - 2;
    }
    setPixelColor(outer_pixels, thirdLedExtend, colorArray[2]);     // turn off last LED we set

    // fourth
    int fourthLedExtend = p1StateExtend - 3;
    if (fourthLedExtend < 0) {               // wrap round count
      fourthLedExtend = numPixelsExtend - 3;
    }
    setPixelColor(outer_pixels, fourthLedExtend, colorArray[3]);     // turn off last LED we set

    // last
    int lastLedExtend = p1StateExtend - colorArrayLength;        // find LED to turn off
    if (lastLedExtend < 0) {               // wrap round count
      lastLedExtend = numPixelsExtend - colorArrayLength;
    }
    setPixelColor(outer_pixels, lastLedExtend, offColor);     // turn off last LED we set


  } else {
    // second
    int secondLed = p1State - 1;
    if (secondLed < 0) {               // wrap round count
      secondLed = numPixels - 1;
    }
    setPixelColor(outer_pixels, secondLed, colorArray[1]);     // turn off last LED we set

    // third
    int thirdLed = p1State - 2;
    if (thirdLed < 0) {               // wrap round count
      thirdLed = numPixels - 2;
    }
    setPixelColor(outer_pixels, thirdLed, colorArray[2]);     // turn off last LED we set

    // fourth
    int fourthLed = p1State - 3;
    if (fourthLed < 0) {               // wrap round count
      fourthLed = numPixels - 3;
    }
    setPixelColor(outer_pixels, fourthLed, colorArray[3]);     // turn off last LED we set

    // last
    int lastLed = p1State - colorArrayLength;        // find LED to turn off
    if (lastLed < 0) {               // wrap round count
      lastLed = numPixels - colorArrayLength;
    }
    setPixelColor(outer_pixels, lastLed, offColor);     // turn off last LED we set
  }

  

  if ( p1State == numPixels - 1 ) {
    p1StateExtend = p1State;
  }


  p1State ++;                 // move on state variable for the next time we enter this
  if (p1State >= numPixels){   // wrap round the state
  p1State = 0;
  }

  if ( p1StateExtend != 0 ) {
    p1StateExtend++;
  }  
  if ( p1StateExtend >= numPixelsExtend ) {
    p1StateExtend = 0;
  }

  outer_pixels.show(); // update display
  lastUpdateP1 = millis();               // to calculate next update
}






void updatePattern2() { // pattern 1 a walking green led
  uint32_t color2 = inner_pixels.Color(0, 64, 0);
  uint32_t offColor = inner_pixels.Color(0, 0, 0);
  setPixelColor(inner_pixels, p2State, color2); // turn on next led in pattern
  int lastLed2 = p2State - 1;        // find LED to turn off
  if (lastLed2 < 0) {               // wrap round count
    lastLed2 = inner_pixels.numPixels() - 1;
  }
  setPixelColor(inner_pixels, lastLed2, offColor);     // turn off last LED we set
  p2State ++;                 // move on state variable for the next time we enter this
  if(p2State >= inner_pixels.numPixels()){   // wrap round the state
  p2State = 0;
  }
  inner_pixels.show(); // update display
  lastUpdateP2 = millis();               // to calculate next update
}

void updatePattern3() { // pattern 1 a walking blue led
  uint32_t color2 = middle_pixels.Color(64, 0, 0);
  uint32_t offColor = middle_pixels.Color(0, 0, 0);
  setPixelColor(middle_pixels, p3State, color2); // turn on next led in pattern
  int lastLed2 = p3State - 1;        // find LED to turn off
  if (lastLed2 < 0) {               // wrap round count
    lastLed2 = middle_pixels.numPixels() - 1;
  }
  setPixelColor(middle_pixels, lastLed2, offColor);     // turn off last LED we set
  p3State ++;                 // move on state variable for the next time we enter this
  if(p3State >= middle_pixels.numPixels()){   // wrap round the state
  p3State = 0;
  }
  middle_pixels.show(); // update display
  lastUpdateP3 = millis();               // to calculate next update
}

void updatePattern4() { // pattern 1 a walking purple led
  uint32_t color2 = bottom_pixels.Color(32, 0, 32);
  uint32_t offColor = bottom_pixels.Color(0, 0, 0);
  setPixelColor(bottom_pixels, p4State, color2); // turn on next led in pattern
  int lastLed2 = p4State - 1;        // find LED to turn off
  if (lastLed2 < 0) {               // wrap round count
    lastLed2 = bottom_pixels.numPixels() - 1;
  }
  setPixelColor(bottom_pixels, lastLed2, offColor);     // turn off last LED we set
  p4State ++;                 // move on state variable for the next time we enter this
  if(p4State >= bottom_pixels.numPixels()){   // wrap round the state
  p4State = 0;
  }
  bottom_pixels.show(); // update display
  lastUpdateP4 = millis();               // to calculate next update
}


void getData( void * parameter) {

  for(;;) {
    // Send an HTTP POST request every 5 seconds
    if ((millis() - lastTime) > timerDelay) {

      Serial.print("getData() running on core ");
      Serial.println(xPortGetCoreID());
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;

        String serverPath = serverName + "?r=" + String(random(1410065407));
        Serial.println(serverPath);
        
        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          // Serial.println(payload);

          XMLDocument xmlDocument;

          if(xmlDocument.Parse(payload.c_str())!= XML_SUCCESS){
            Serial.println("Error parsing");  
            return;
          };
          
          XMLNode * root = xmlDocument.FirstChild();
          XMLElement * station = root->FirstChildElement("station");
          XMLElement * dish = root->FirstChildElement("dish");
          XMLElement * target = dish->FirstChildElement("target");
          XMLElement * timestamp = root->FirstChildElement("timestamp");

          const char * stationFriendlyName;
          stationFriendlyName = station->Attribute("friendlyName");

          const char * dishName;
          dishName = dish->Attribute("name");

          const char * targetName;
          targetName = target->Attribute("name");

          uint64_t timestampInt;      
          timestamp->QueryUnsigned64Text(&timestampInt);


          Serial.println(timestampInt);
          Serial.println(stationFriendlyName);
          Serial.println(dishName);
          Serial.println(targetName);

        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");

        WiFi.begin(ssid, password);
        Serial.println("Connecting");
        while(WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
      }
      lastTime = millis();
    }
  }
}


// setup() function -- runs once at startup --------------------------------
void setup() {
  Serial.begin(115200);

  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());

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




  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());


  xTaskCreatePinnedToCore(
      getData, /* Function to implement the task */
      "getData", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &HandleData,  /* Task handle. */
      0); /* Core where the task should run */


  
}




// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  // Serial.print("loop() running on core ");
  // Serial.println(xPortGetCoreID());




  if(millis() - lastUpdateP1 > pattern1Interval) updatePattern1();
  if(millis() - lastUpdateP2 > pattern2Interval) updatePattern2();
  if(millis() - lastUpdateP3 > pattern3Interval) updatePattern3();
  if(millis() - lastUpdateP4 > pattern4Interval) updatePattern4();





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
