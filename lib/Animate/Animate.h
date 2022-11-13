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
    public:
        AnimationUtils aUtil;
        void meteorRainRegions(
            Adafruit_NeoPixel *&strip,
            int region,
            int regionLength,
            int beginPixel,
            const uint32_t *pColor,
            int meteorSize,
            int meteorTrailDecay,
            bool meteorRandomDecay,
            int tailHueStart,
            bool tailHueAdd,
            float tailHueExponent,
            int tailHueSaturation
        );
        Animate() {
            aUtil = AnimationUtils();
        };
};

#endif