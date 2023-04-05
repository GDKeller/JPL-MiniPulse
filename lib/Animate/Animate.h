/*
    Animation Methods
    Created by Grant Keller, November 2022
*/

#ifndef ADAFRUIT_NEOPIXEL_H
#include <Adafruit_NeoPixel.h>
#endif

#ifndef FASTLED_H
#include <FastLED.h>
#endif

#ifndef ANIMATIONUTILS_H
#include <AnimationUtils.h>
#endif

#ifndef ANIMATE_H
#define ANIMATE_H

struct ColorTheme {
    CHSV letter;
    CHSV meteor;
    uint16_t tailHue;
    uint8_t tailSaturation;
};

struct Meteor {
    bool directionDown;
    int firstPixel;
    int region;
    int regionLength;
    CHSV pColor;
    int meteorSize;
    bool hasTail;
    bool meteorTrailDecay;
    float meteorTrailDecayValue;
    bool meteorRandomDecay;
    int tailHueStart;
    bool tailHueAdd;
    double tailHueExponent;
    int tailHueSaturation;
    CRGB *&rStrip;
};

class Animate {
    static AnimationUtils aUtilAnimate;
    public:
        Meteor* ActiveMeteors[500] = {nullptr};
        size_t ActiveMeteorsSize = 0;
        void animateMeteor(Meteor* meteor);

        Animate();
};

#endif
