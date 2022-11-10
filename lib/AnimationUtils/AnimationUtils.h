/*
    Animation Utilities
    Created by Grant Keller, November 2022
*/

#include <Arduino.h>

#ifndef AnimationUtils_h
#define AnimationUtils_h

class AnimationUtils { 
    
    public:
        int brightness;
        int thePin;
        int readBrightness();
        void updateBrightness();
        AnimationUtils(int thePin);
};

#endif
