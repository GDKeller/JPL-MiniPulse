/*
    Animation Methods
    Created by Grant Keller, November 2022
*/

#ifndef ALGORITHM_H
#include <algorithm>
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
    CRGB*& rStrip;
    int rateClass;
    uint8_t animationType;
};

class Animate {
    static AnimationUtils aUtilAnimate;
    const uint8_t tailBrightnessMap[8] = {
        200,
        180,
        160,
        140,
        135,
        130,
        125,
        120,
    };
    int tailBrightnessMapLength = sizeof(tailBrightnessMap) / sizeof(tailBrightnessMap[0]); // length of array


    public:
    uint8_t forcedAnimationType = 0;
    static const uint16_t ActiveMeteorArraySize = 500;
    Meteor* ActiveMeteors[ActiveMeteorArraySize] = { nullptr };
    size_t ActiveMeteorsSize = 0;
    void animateMeteor(Meteor* meteor);

    Animate();
};

#endif
