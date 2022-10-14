#include <Arduino.h>
#include <TextCharacters.h>

TextCharacter::TextCharacter() {
    // Serial.println(thing);
    // char theChar = character;
    // int *pCharArray = _getCharacter(theChar);
}





// Letter Typography arrays
int character_a[20] = {
  0, 1, 1, 0,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_b[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_c[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_d[20] = {
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0
};

int character_e[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_f[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  1, 0, 0, 0
};

int character_g[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_h[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_i[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

int character_j[20] = {
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_k[20] = {
  1, 0, 0, 1,
  1, 0, 1, 0,
  1, 1, 0, 0,
  1, 0, 1, 0,
  1, 0, 0, 1
};

int character_l[20] = {
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_m[20] = {
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_n[20] = {
  1, 0, 0, 1,
  1, 1, 0, 1,
  1, 0, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_o[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_p[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0
};

int character_q[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 1, 1,
  1, 1, 1, 1
};

int character_r[20] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_s[20] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 1,
  1, 1, 1, 1
};

int character_t[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

int character_u[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0
};

int character_v[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0,
  0, 0, 1, 0
};

int character_w[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1
};

int character_x[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_y[20] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

int character_z[20] = {
  1, 1, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};



int characterArray[20] = {};

int * TextCharacter::getCharacter(char character) {
    
    int *pCharacterArray = &characterArray[20];

    switch(character) {
        case 'a':
            pCharacterArray = character_a;
            break;
        case 'b':
            pCharacterArray = character_b;
            break;
        case 'c':
            pCharacterArray = character_c;
            break;
        case 'd':
            pCharacterArray = character_d;
            break;
        case 'e':
            pCharacterArray = character_e;
            break;
        case 'f':
            pCharacterArray = character_f;
            break;
        case 'g':
            pCharacterArray = character_g;
            break;
        case 'h':
            pCharacterArray = character_h;
            break;
        case 'i':
            pCharacterArray = character_i;
            break;
        case 'j':
            pCharacterArray = character_j;
            break;
        case 'k':
            pCharacterArray = character_k;
            break;
        case 'l':
            pCharacterArray = character_l;
            break;
        case 'm':
            pCharacterArray = character_m;
            break;
        case 'n':
            pCharacterArray = character_n;
            break;
        case 'o':
            pCharacterArray = character_o;
            break;
        case 'p':
            pCharacterArray = character_p;
            break;
        case 'q':
            pCharacterArray = character_q;
            break;
        case 'r':
            pCharacterArray = character_r;
            break;
        case 's':
            pCharacterArray = character_s;
            break;
        case 't':
            pCharacterArray = character_t;
            break;
        case 'u':
            pCharacterArray = character_u;
            break;
        case 'v':
            pCharacterArray = character_v;
            break;
        case 'w':
            pCharacterArray = character_w;
            break;
        case 'x':
            pCharacterArray = character_x;
            break;
        case 'y':
            pCharacterArray = character_y;
            break;
        case 'z':
            pCharacterArray = character_z;
            break;
        
    }

    // int output[20] = pCharacterArray;

    return pCharacterArray;

}
