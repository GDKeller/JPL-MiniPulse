#include <Arduino.h>
#include <AnimationUtils.h>

AnimationUtils::AnimationUtils(int potPin) {
  thePin = potPin;
  brightness = 255;
}

int AnimationUtils::readBrightness() {
  int potValue = analogRead(thePin);
  int potBrightness = map(potValue, 0, 4095, 32, 255);
  return potBrightness;
}


void AnimationUtils::updateBrightness() {
  brightness = readBrightness();
}