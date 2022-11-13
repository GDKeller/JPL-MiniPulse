int degreeToSixteenbit(int degree) {
  // Max value of uint16_t is 65535
  return (degree * 65535) / 360;
}

float mPower(float a, int b) {
  float result = a;
  for (int i = 1; i < b; i++) {
    result = result * a;
  }
  return result;
}