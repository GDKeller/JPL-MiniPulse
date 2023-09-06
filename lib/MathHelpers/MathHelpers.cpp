/**
 * Converts a hue int (0-360 degrees) to a 16 bit int
 * The NeoPixel HSV color value takes a 16bit int
 * 
 * @param degree The degree of the hue value
 * @return A 16bit int for use in Adafruit_NeoPixel::ColorsHSV
*/
int degreeToSixteenbit(int degree) {
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
float mPower(float a, int b) {
  float result = a;
  for (int i = 1; i < b; i++) {
    result = result * a;
  }
  return result;
}