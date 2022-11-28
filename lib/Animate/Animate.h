/*
    Animation Methods
    Created by Grant Keller, November 2022
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MathHelpers.h>
#include <AnimationUtils.h>

#ifndef Animate_h
#define Animate_h

struct Meteor {
    int arrayPos;
    int firstPixel;
    int region;
    int regionLength;
    uint32_t *pColor;
    int meteorSize;
    int meteorTrailDecay;
    bool meteorRandomDecay;
    int tailHueStart;
    bool tailHueAdd;
    double tailHueExponent;
    int tailHueSaturation;
    Adafruit_NeoPixel *&rStrip;
};

class Animate {
    static AnimationUtils aUtilAnimate;
    public:
        Meteor* ActiveMeteors[100] = {nullptr};
        size_t ActiveMeteorsSize = 100;
        void animateMeteor(Meteor* meteor);

        Animate();
};

#endif