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

class Animate {
    static AnimationUtils aUtilAnimate;
    public:
        Animate();

        class Meteor {
            public:
                Adafruit_NeoPixel* &strip;
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
                int firstPixel;

                void fireMeteor(
                    Adafruit_NeoPixel* &strip,
                    int region,
                    int regionLength,
                    uint32_t *pColor,
                    int meteorSize,
                    int meteorTrailDecay,
                    bool meteorRandomDecay,
                    int tailHueStart,
                    bool tailHueAdd,
                    double tailHueExponent,
                    int tailHueSaturation
                );
                void animateMeteor(Meteor* meteor);

                Meteor();
        };
};

#endif