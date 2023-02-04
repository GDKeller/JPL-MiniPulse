/*
    Animation Methods
    Created by Grant Keller, November 2022
*/

#ifndef ADAFRUIT_NEOPIXEL_H
#include <Adafruit_NeoPixel.h>
#endif

#ifndef ANIMATIONUTILS_H
#include <AnimationUtils.h>
#endif

#ifndef ANIMATE_H
#define ANIMATE_H

struct Meteor {
    bool directionDown;
    int firstPixel;
    int region;
    int regionLength;
    uint32_t *pColor;
    int meteorSize;
    bool meteorTrailDecay;
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
        Meteor* ActiveMeteors[500] = {nullptr};
        size_t ActiveMeteorsSize = 0;
        void animateMeteor(Meteor* meteor);

        Animate();
};

#endif