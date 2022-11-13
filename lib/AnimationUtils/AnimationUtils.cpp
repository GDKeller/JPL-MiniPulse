#include <Arduino.h>
#include <AnimationUtils.h>
#include <Adafruit_NeoPixel.h>

int AnimationUtils::brightness = 255;

AnimationUtils::AnimationUtils(int pin) {
  potPin = pin;
}

int AnimationUtils::readBrightness() {
  int potValue = analogRead(potPin);
  int potBrightness = map(potValue, 0, 4095, 32, 255);
  return potBrightness;
}

void AnimationUtils::updateBrightness() {
  brightness = readBrightness();
}