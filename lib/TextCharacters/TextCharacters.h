/*
    Alphanumeric characters for low resolution display
    Created by Grant Keller, October 2022
*/

#include <Arduino.h>
#include <array>

#ifndef TextCharacter_h
#define TextCharacter_h

class TextCharacter {
    public:
    TextCharacter();
    struct TextCharacterInfo {
        const int8_t* characterArray;
        int characterTotalPixels;
    };
    const TextCharacterInfo getCharacter(char theChar, int width);
    private:
    const int8_t* getCharacterArray(char character, int width);
};

#endif
