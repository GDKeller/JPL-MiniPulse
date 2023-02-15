/*
    Animation Utilities
    Created by Grant Keller, November 2022
*/

#ifndef ADAFRUIT_NEOPIXEL_H
    #include <Adafruit_NeoPixel.h>
#endif

#ifndef FASTLED_H
    #include <FastLED.h>
#endif

#ifndef MATHHELPERS_H
    #include <MathHelpers.h>
#endif

#ifndef ANIMATIONUTILS_H
#define ANIMATIONUTILS_H

class AnimationUtils {
    public:
        static int brightness;
        int potPin;
        int readBrightness();
        void updateBrightness();
        void setPixelColor(
            Adafruit_NeoPixel &strip,
            uint16_t n,
            const uint32_t *color
        );
        AnimationUtils();
        AnimationUtils(int pin);

        class Colors {
            struct Color {
                CHSV value;
            };
            public:
                Color red = {
                    CHSV(0, 255, 255)

                };

                Color redFaint = {
                    CHSV(0, 128, 255)
                };

                Color orange = {
                    CHSV(32, 255, 255)
                };

                Color yellow = {
                    CHSV(64, 255, 255)
                };

                Color green = {
                    CHSV(96, 255, 255)
                };

                Color aqua = {
                    CHSV(128, 255, 255)
                };

                Color blue = {
                    CHSV(160, 255, 255)
                };

                Color purple = {
                    CHSV(192, 255, 255)
                };

                Color pink = {
                    CHSV(224, 255, 255)
                };

                Color white = {
                    CHSV(0, 0, 255)
                };

                Color dim = {
                    CHSV(0, 0, 64)
                };

                Color off = {
                    CHSV(0, 0, 0)
                };
        };
};

#endif