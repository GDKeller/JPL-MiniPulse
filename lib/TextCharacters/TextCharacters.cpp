#include <Arduino.h>
#include <TextCharacters.h>

TextCharacter::TextCharacter() {
    // Serial.println(thing);
    // char theChar = character;
    // int *pCharArray = _getCharacter(theChar);
}





// Letter Typography arrays



// 4 wide

int character_space[28] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

int character_a[28] = {
  0, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_b[28] = {
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0
};

int character_c[28] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_d[28] = {
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0
};

int character_e[28] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_f[28] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0
};

int character_g[28] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_h[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_i[28] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

int character_j[28] = {
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1
};

int character_k[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 1, 0,
  1, 1, 0, 0,
  1, 0, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_l[28] = {
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};

int character_m[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_n[28] = {
  1, 0, 0, 1,
  1, 1, 0, 1,
  1, 1, 0, 1,
  1, 0, 1, 1,
  1, 0, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_o[28] = {
  0, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0
};

int character_p[28] = {
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 0, 0, 0
};

int character_q[28] = {
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 1, 1,
  1, 0, 1, 0,
  1, 1, 1, 1
};

int character_r[28] = {
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_s[28] = {
  1, 1, 1, 1,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  1, 1, 1, 1
};

int character_t[28] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

int character_u[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0
};

int character_v[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0,
  0, 1, 1, 0
};

int character_w[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 1, 1, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_x[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 0,
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1
};

int character_y[28] = {
  1, 0, 0, 1,
  1, 0, 0, 1,
  1, 0, 0, 1,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

int character_z[28] = {
  1, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 1, 1, 0,
  1, 0, 0, 0,
  1, 0, 0, 0,
  1, 1, 1, 1
};






// 3 x 5
// int character_space[20] = {
//   0, 0, 0, 0,
//   0, 0, 0, 0,
//   0, 0, 0, 0,
//   0, 0, 0, 0,
//   0, 0, 0, 0
// };

// int character_a[20] = {
//   0, 0, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_b[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 1, 1
// };

// int character_c[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 0, 0,
//   0, 1, 0, 0,
//   0, 1, 1, 1
// };

// int character_d[20] = {
//   0, 1, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 0
// };

// int character_e[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 1, 0,
//   0, 1, 0, 0,
//   0, 1, 1, 1
// };

// int character_f[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 0, 0
// };

// int character_g[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1
// };

// int character_h[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_i[20] = {
//   0, 1, 1, 1,
//   0, 0, 1, 0,
//   0, 0, 1, 0,
//   0, 0, 1, 0,
//   0, 1, 1, 1
// };

// int character_j[20] = {
//   0, 0, 0, 1,
//   0, 0, 0, 1,
//   0, 0, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1
// };

// int character_k[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_l[20] = {
//   0, 1, 0, 0,
//   0, 1, 0, 0,
//   0, 1, 0, 0,
//   0, 1, 0, 0,
//   0, 1, 1, 1
// };

// int character_m[20] = {
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_n[20] = {
//   0, 1, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_o[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1
// };

// int character_p[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 0, 0
// };

// int character_q[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 0, 0, 1
// };

// int character_r[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 1, 0,
//   0, 1, 0, 1
// };

// int character_s[20] = {
//   0, 1, 1, 1,
//   0, 1, 0, 0,
//   0, 1, 1, 1,
//   0, 0, 0, 1,
//   0, 1, 1, 1
// };

// int character_t[20] = {
//   0, 1, 1, 1,
//   0, 0, 1, 0,
//   0, 0, 1, 0,
//   0, 0, 1, 0,
//   0, 0, 1, 0
// };

// int character_u[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1
// };

// int character_v[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 0, 1, 0
// };

// int character_w[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 1, 1, 1,
//   0, 1, 0, 1
// };

// int character_x[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 0, 1, 0,
//   0, 1, 0, 1,
//   0, 1, 0, 1
// };

// int character_y[20] = {
//   0, 1, 0, 1,
//   0, 1, 0, 1,
//   0, 0, 1, 0,
//   0, 0, 1, 0,
//   0, 0, 1, 0
// };

// int character_z[20] = {
//   0, 1, 1, 1,
//   0, 0, 0, 1,
//   0, 0, 1, 0,
//   0, 1, 0, 0,
//   0, 1, 1, 1
// };





// 3 x 7
// int character_space[21] = {
//   0, 0, 0,
//   0, 0, 0,
//   0, 0, 0,
//   0, 0, 0,
//   0, 0, 0,
//   0, 0, 0,
//   0, 0, 0
// };

// int character_a[21] = {
//   0, 1, 0,
//   0, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_b[21] = {
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1
// };

// int character_c[21] = {
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1
// };

// int character_d[21] = {
//   1, 0, 0,
//   1, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 0,
//   1, 0, 0
// };

// int character_e[21] = {
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1
// };

// int character_f[21] = {
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0
// };

// int character_g[21] = {
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1
// };

// int character_h[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_i[21] = {
//   1, 1, 1,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   1, 1, 1
// };

// int character_j[21] = {
//   0, 0, 1,
//   0, 0, 1,
//   0, 0, 1,
//   0, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1
// };

// int character_k[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_l[21] = {
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1
// };

// int character_m[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_n[21] = {
//   1, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_o[21] = {
//   0, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   0, 1, 0
// };

// int character_p[21] = {
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 0, 0
// };

// int character_q[21] = {
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   0, 1, 0,
//   0, 0, 1
// };

// int character_r[21] = {
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_s[21] = {
//   1, 1, 1,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1,
//   0, 0, 1,
//   0, 0, 1,
//   1, 1, 1
// };

// int character_t[21] = {
//   1, 1, 1,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0
// };

// int character_u[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1
// };

// int character_v[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   0, 1, 0,
//   0, 1, 0
// };

// int character_w[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   1, 1, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_x[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   0, 1, 0,
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1
// };

// int character_y[21] = {
//   1, 0, 1,
//   1, 0, 1,
//   1, 0, 1,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0,
//   0, 1, 0
// };

// int character_z[21] = {
//   1, 1, 1,
//   0, 0, 1,
//   0, 0, 1,
//   0, 1, 0,
//   1, 0, 0,
//   1, 0, 0,
//   1, 1, 1
// };











int characterArray[28] = {};

int* TextCharacter::getCharacter(char character) {
    
    int *pCharacterArray = &characterArray[28];

    switch(character) {
      case ' ':
            pCharacterArray = character_space;
            break;
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
