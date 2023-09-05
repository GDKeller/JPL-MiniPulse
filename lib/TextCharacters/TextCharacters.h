/*
    Alphanumeric characters for low resolution display
    Created by Grant Keller, October 2022
*/

#include <Arduino.h>

#ifndef TextCharacter_h
#define TextCharacter_h

class TextCharacter {
    public:
    TextCharacter();
    int* getCharacter(char theChar, int width);
    private:
    int* getCharacterArray(char character, int width);
};

#endif
