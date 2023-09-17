#include "MathHelpers.h">

/**
 * Converts a hue int (0-360 degrees) to a 16 bit int
 * The NeoPixel HSV color value takes a 16bit int
 * 
 * @param degree The degree of the hue value
 * @return A 16bit int for use in Adafruit_NeoPixel::ColorsHSV
*/
int MathHelpers::degreeToSixteenbit(int degree) {
  // Max value of uint16_t is 65535
  return (degree * 65535) / 360;
}

/**
 * Custom exponent function
 * The C++ pow() function is very slow, this should be quicker
 * 
 * @param float a The base, float between 0 and 1
 * @param int b The Exponent
 * @return Float result
*/
float MathHelpers::mPower(float a, int b) {
  float result = a;
  for (int i = 1; i < b; i++) {
    result = result * a;
  }
  return result;
}
long MathHelpers::map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long run = in_max - in_min;
    if(run == 0){
        return -1; // AVR returns -1, SAM returns 0
    }
    const long rise = out_max - out_min;
    const long delta = x - in_min;
    return (delta * rise) / run + out_min;
}