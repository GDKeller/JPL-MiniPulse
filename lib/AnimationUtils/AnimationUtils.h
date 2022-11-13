/*
    Animation Utilities
    Created by Grant Keller, November 2022
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifndef AnimationUtils_h
#define AnimationUtils_h



class AnimationUtils {
    public:
        int brightness;
        int potPin;
        int readBrightness();
        void updateBrightness();
        class Colors {
            struct Color {
                uint32_t value;
                uint32_t* pointer;
            };
            public:
                // Color yellow;
                Color yellow = {
                    Adafruit_NeoPixel::ColorHSV((110 * 65535) / 360, 255, 255),
                    &yellow.value,
                };
        };

        AnimationUtils(int pin);
};

#endif
