/*
    Animation Utilities
    Created by Grant Keller, November 2022
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MathHelpers.h>

#ifndef AnimationUtils_h
#define AnimationUtils_h


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
                uint32_t value;
                uint32_t* pointer;
            };
            public:
                Color red = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(0),
                        255,
                        255
                    ),
                    &red.value,
                };

                Color redFaint = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(0),
                        128,
                        255
                    ),
                    &redFaint.value,
                };

                Color orange = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(30),
                        255,
                        255
                    ),
                    &orange.value,
                };

                Color yellow = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(60),
                        255,
                        255
                    ),
                    &yellow.value,
                };

                Color green = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(120),
                        255,
                        255
                    ),
                    &green.value,
                };

                Color teal = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(150),
                        255,
                        255
                    ),
                    &teal.value,
                };

                Color cyan = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(180),
                        255,
                        255
                    ),
                    &cyan.value,
                };

                Color sky = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(210),
                        255,
                        255
                    ),
                    &sky.value,
                };

                Color blue = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(240),
                        255,
                        255
                    ),
                    &blue.value,
                };

                Color violet = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(270),
                        255,
                        255
                    ),
                    &violet.value,
                };

                Color purple = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(300),
                        255,
                        255
                    ),
                    &purple.value,
                };

                Color pink = {
                    Adafruit_NeoPixel::ColorHSV(
                        degreeToSixteenbit(330),
                        255,
                        255
                    ),
                    &pink.value,
                };

                Color white = {
                    Adafruit_NeoPixel::ColorHSV(
                        0,
                        0,
                        255
                    ),
                    &white.value,
                };

                Color dim = {
                    Adafruit_NeoPixel::ColorHSV(
                        0,
                        0,
                        64
                    ),
                    &dim.value,
                };

                Color off = {
                    Adafruit_NeoPixel::ColorHSV(
                        0,
                        0,
                        0
                    ),
                    &off.value,
                };
        };
};

#endif