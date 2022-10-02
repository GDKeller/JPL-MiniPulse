#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define OUTER_PIN 17
#define MIDDLE_PIN 18
#define INNER_PIN 19
#define BOTTOM_PIN 16

#define WIFI_RST 21
#define OUTPUT_ENABLE 22
#define BRIGHTNESS 127        // Global brightness value. 8bit, 0-255

// Define NeoPixel objects - NAME(PIXEL_COUNT, PIN, PIXEL_TYPE)
Adafruit_NeoPixel
  outer_pixels(10, OUTER_PIN, NEO_GRB + NEO_KHZ800),
  inner_pixels(10, INNER_PIN, NEO_GRB + NEO_KHZ800),
  middle_pixels(10, MIDDLE_PIN, NEO_GRB + NEO_KHZ800),
  bottom_pixels(10, BOTTOM_PIN, NEO_GRB + NEO_KHZ800);



// Custom function to calculate color relative to global BRIGHTNESS variable 
void setPixelColor( Adafruit_NeoPixel &strip, uint16_t n, uint32_t &color ) {
  uint8_t rgb[4];             // Create array that will hold color channel values
  *(uint32_t*)&rgb = color;   // Assigns color value to the color channel array
  uint8_t r = rgb[0];         // Red color channel value
  uint8_t g = rgb[1];         // Green color channel value
  uint8_t b = rgb[2];         // Blue color channel value
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




// setup() function -- runs once at startup --------------------------------
void setup() {
  Serial.begin(115200);
  

  pinMode(OUTER_PIN, OUTPUT);
  pinMode(INNER_PIN, OUTPUT);
  pinMode(MIDDLE_PIN, OUTPUT);
  pinMode(BOTTOM_PIN, OUTPUT);
  pinMode(OUTPUT_ENABLE, OUTPUT);
  pinMode(WIFI_RST, INPUT);
  pinMode(BRIGHTNESS, INPUT);

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
}




// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {

  // Fill along the length of the strip in various colors...
  colorWipe(outer_pixels, outer_pixels.Color(255,   0,   0), 50); // Red
  colorWipe(inner_pixels, outer_pixels.Color(255,   0,   0), 50); // Red
  colorWipe(middle_pixels, outer_pixels.Color(255,   0,   0), 50); // Red
  colorWipe(bottom_pixels, outer_pixels.Color(255,   0,   0), 50); // Red

  colorWipe(outer_pixels, outer_pixels.Color(  0, 255,   0), 50); // Green
  colorWipe(inner_pixels, outer_pixels.Color(  0, 255,   0), 50); // Green
  colorWipe(middle_pixels, outer_pixels.Color(  0, 255,   0), 50); // Green
  colorWipe(bottom_pixels, outer_pixels.Color(  0, 255,   0), 50); // Green
  
  colorWipe(outer_pixels, outer_pixels.Color(  0,   0, 255), 50); // Blue
  colorWipe(inner_pixels, outer_pixels.Color(  0,   0, 255), 50); // Blue
  colorWipe(middle_pixels, outer_pixels.Color(  0,   0, 255), 50); // Blue
  colorWipe(bottom_pixels, outer_pixels.Color(  0,   0, 255), 50); // Blue


  // Do a theater marquee effect in various colors...
  theaterChase(outer_pixels, outer_pixels.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(inner_pixels, outer_pixels.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(middle_pixels, outer_pixels.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(bottom_pixels, outer_pixels.Color(127, 127, 127), 50); // White, half brightness  
  
  theaterChase(outer_pixels, outer_pixels.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(inner_pixels, outer_pixels.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(middle_pixels, outer_pixels.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(bottom_pixels, outer_pixels.Color(127,   0,   0), 50); // Red, half brightness
    
  theaterChase(outer_pixels, outer_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
  theaterChase(inner_pixels, outer_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
  theaterChase(middle_pixels, outer_pixels.Color(  0,   0, 127), 50); // Blue, half brightness
  theaterChase(bottom_pixels, outer_pixels.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(outer_pixels, 10);             // Flowing rainbow cycle along the whole strip
  rainbow(inner_pixels, 10);             // Flowing rainbow cycle along the whole strip
  rainbow(middle_pixels, 10);             // Flowing rainbow cycle along the whole strip
  rainbow(bottom_pixels, 10);             // Flowing rainbow cycle along the whole strip  
  
  theaterChaseRainbow(outer_pixels, 50); // Rainbow-enhanced theaterChase variant
  theaterChaseRainbow(inner_pixels, 50); // Rainbow-enhanced theaterChase variant
  theaterChaseRainbow(middle_pixels, 50); // Rainbow-enhanced theaterChase variant
  theaterChaseRainbow(bottom_pixels, 50); // Rainbow-enhanced theaterChase variant
}
