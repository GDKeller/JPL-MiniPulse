#include <Arduino.h>
#include <AnimationUtils.h>
#include <Adafruit_NeoPixel.h>

AnimationUtils::AnimationUtils(int pin) {
  potPin = pin;
  brightness = 255;
}

int AnimationUtils::readBrightness() {
  int potValue = analogRead(potPin);
  int potBrightness = map(potValue, 0, 4095, 32, 255);
  return potBrightness;
}

void AnimationUtils::updateBrightness() {
  brightness = readBrightness();
}