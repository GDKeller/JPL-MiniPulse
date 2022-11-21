#include <Arduino.h>
#include <TextCharacters.h>

TextCharacter::TextCharacter()
{
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
	0, 0, 0, 0};

int character_a[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_b[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0};

int character_c[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1};

int character_d[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0};

int character_e[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1};

int character_f[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0};

int character_g[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1};

int character_h[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_i[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 1};

int character_j[28] = {
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1};

int character_k[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 0,
	1, 1, 0, 0,
	1, 0, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_l[28] = {
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1};

int character_m[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_n[28] = {
	1, 0, 0, 1,
	1, 1, 0, 1,
	1, 1, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_o[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0};

int character_p[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0};

int character_q[28] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 0,
	1, 1, 1, 1};

int character_r[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_s[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0};

int character_t[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0};

int character_u[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0};

int character_v[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	0, 1, 1, 0};

int character_w[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_x[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1};

int character_y[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 1};

int character_z[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1};

int character_0[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 1, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0};

int character_1[28] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 1};

int character_2[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1};

int character_3[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0};

int character_4[28] = {
	0, 0, 0, 1,
	0, 0, 1, 1,
	0, 1, 0, 1,
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1};

int character_5[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0};

int character_6[28] = {
	0, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0};

int character_7[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0};

int character_8[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0};

int character_9[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0};



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

int *TextCharacter::getCharacter(char character)
{

	int *pCharacterArray = &characterArray[28];

	switch (character)
	{
	case ' ':
		pCharacterArray = character_space;
		break;
	case 'A':
	case 'a':
		pCharacterArray = character_a;
		break;
	case 'B':
	case 'b':
		pCharacterArray = character_b;
		break;
	case 'C':
	case 'c':
		pCharacterArray = character_c;
		break;
	case 'D':
	case 'd':
		pCharacterArray = character_d;
		break;
	case 'E':
	case 'e':
		pCharacterArray = character_e;
		break;
	case 'F':
	case 'f':
		pCharacterArray = character_f;
		break;
	case 'G':
	case 'g':
		pCharacterArray = character_g;
		break;
	case 'H':
	case 'h':
		pCharacterArray = character_h;
		break;
	case 'I':
	case 'i':
		pCharacterArray = character_i;
		break;
	case 'J':
	case 'j':
		pCharacterArray = character_j;
		break;
	case 'K':
	case 'k':
		pCharacterArray = character_k;
		break;
	case 'L':
	case 'l':
		pCharacterArray = character_l;
		break;
	case 'M':
	case 'm':
		pCharacterArray = character_m;
		break;
	case 'N':
	case 'n':
		pCharacterArray = character_n;
		break;
	case 'O':
	case 'o':
		pCharacterArray = character_o;
		break;
	case 'P':
	case 'p':
		pCharacterArray = character_p;
		break;
	case 'Q':
	case 'q':
		pCharacterArray = character_q;
		break;
	case 'R':
	case 'r':
		pCharacterArray = character_r;
		break;
	case 'S':
	case 's':
		pCharacterArray = character_s;
		break;
	case 'T':
	case 't':
		pCharacterArray = character_t;
		break;
	case 'U':
	case 'u':
		pCharacterArray = character_u;
		break;
	case 'V':
	case 'v':
		pCharacterArray = character_v;
		break;
	case 'W':
	case 'w':
		pCharacterArray = character_w;
		break;
	case 'X':
	case 'x':
		pCharacterArray = character_x;
		break;
	case 'Y':
	case 'y':
		pCharacterArray = character_y;
		break;
	case 'Z':
	case 'z':
		pCharacterArray = character_z;
		break;
	case '0':
		pCharacterArray = character_0;
		break;
	case '1':
		pCharacterArray = character_1;
		break;
	case '2':
		pCharacterArray = character_2;
		break;
	case '3':
		pCharacterArray = character_3;
		break;
	case '4':
		pCharacterArray = character_4;
		break;
	case '5':
		pCharacterArray = character_5;
		break;
	case '6':
		pCharacterArray = character_6;
		break;
	case '7':
		pCharacterArray = character_7;
		break;
	case '8':
		pCharacterArray = character_8;
		break;
	case '9':
		pCharacterArray = character_9;
		break;
	default:
		pCharacterArray = character_space;
	}

	// int output[20] = pCharacterArray;

	return pCharacterArray;
}
