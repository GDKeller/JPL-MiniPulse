#include <Arduino.h>
#include <TextCharacters.h>

TextCharacter::TextCharacter() {
    // Serial.println(thing);
    // char theChar = character;
    // int *pCharArray = _getCharacter(theChar);
}





// Letter Typography arrays
int character_g[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_j[20] = {
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_l[20] = {
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_p[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0
};

int character_v[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 1, 1,
  0, 1, 1, 0,
  0, 1, 1, 0
};



int characterArray[20] = {};

int * TextCharacter::getCharacter(char character) {
    
    int *pCharacterArray = &characterArray[20];

    switch(character) {
        case 'g':
            pCharacterArray = character_g;
            break;
        case 'v':
            pCharacterArray = character_v;
            break;
    }

    // int output[20] = pCharacterArray;

    return pCharacterArray;

}
