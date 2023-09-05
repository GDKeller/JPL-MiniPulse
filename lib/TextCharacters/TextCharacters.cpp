#include <TextCharacters.h>

TextCharacter::TextCharacter() {}

/* Letter Typography arrays */
// 3 x 5
#pragma region -- 3 x 5

int character_space_3[20] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

int character_a_3[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_b_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_c_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

int character_d_3[20] = {
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 0
};

int character_e_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

int character_f_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0
};

int character_g_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_h_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_i_3[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

int character_j_3[20] = {
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_k_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_l_3[20] = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

int character_m_3[20] = {
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_n_3[20] = {
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_o_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_p_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0
};

int character_q_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 0, 0, 1
};

int character_r_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 1, 0,
  0, 1, 0, 1
};

int character_s_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

int character_t_3[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

int character_u_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_v_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0
};

int character_w_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1
};

int character_x_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1
};

int character_y_3[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

int character_z_3[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

int character_0_3[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0
};

int character_1_3[20] = {
  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

int character_2_3[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

int character_3_3[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

int character_4_3[20] = {
  0, 0, 0, 1,
  0, 0, 1, 1,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1
};

int character_5_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 1,
  0, 1, 1, 0
};

int character_6_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_7_3[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 0, 0
};

int character_8_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

int character_9_3[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1
};

#pragma endregion -- 3 x 5


// 4 wide
#pragma region -- 4 wide
int character_space_4[28] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 };

int character_a_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_b_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0 };

int character_c_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

int character_d_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0 };

int character_e_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

int character_f_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0 };

int character_g_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1 };

int character_h_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_i_4[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 1 };

int character_j_4[28] = {
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1 };

int character_k_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 0,
	1, 1, 0, 0,
	1, 0, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_l_4[28] = {
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

int character_m_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_n_4[28] = {
	1, 0, 0, 1,
	1, 1, 0, 1,
	1, 1, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_o_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

int character_p_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0 };

int character_q_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 0,
	1, 1, 1, 1 };

int character_r_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_s_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

int character_t_4[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0 };

int character_u_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

int character_v_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	0, 1, 1, 0 };

int character_w_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_x_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

int character_y_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 1 };

int character_z_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

int character_0_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 1, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0 };

int character_1_4[28] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 1 };

int character_2_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

int character_3_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0 };

int character_4_4[28] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	1, 0, 1, 0,
	1, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0 };

int character_5_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0 };

int character_6_4[28] = {
	0, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

int character_7_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0 };

int character_8_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

int character_9_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0 };

#pragma endregion -- 4 wide


/* Get character array
 * @param character - character to get array for
 * @param width - width of character
 * @return pointer to character array
 */
int* TextCharacter::getCharacterArray(char character, int width) {
	switch (width) {
		case 3:
			switch (character) {
				case ' ':
					return character_space_3;
				case 'A':
				case 'a':
					return character_a_3;
				case 'B':
				case 'b':
					return character_b_3;
				case 'C':
				case 'c':
					return character_c_3;
				case 'D':
				case 'd':
					return character_d_3;
				case 'E':
				case 'e':
					return character_e_3;
				case 'F':
				case 'f':
					return character_f_3;
				case 'G':
				case 'g':
					return character_g_3;
				case 'H':
				case 'h':
					return character_h_3;
				case 'I':
				case 'i':
					return character_i_3;
				case 'J':
				case 'j':
					return character_j_3;
				case 'K':
				case 'k':
					return character_k_3;
				case 'L':
				case 'l':
					return character_l_3;
				case 'M':
				case 'm':
					return character_m_3;
				case 'N':
				case 'n':
					return character_n_3;
				case 'O':
				case 'o':
					return character_o_3;
				case 'P':
				case 'p':
					return character_p_3;
				case 'Q':
				case 'q':
					return character_q_3;
				case 'R':
				case 'r':
					return character_r_3;
				case 'S':
				case 's':
					return character_s_3;
				case 'T':
				case 't':
					return character_t_3;
				case 'U':
				case 'u':
					return character_u_3;
				case 'V':
				case 'v':
					return character_v_3;
				case 'W':
				case 'w':
					return character_w_3;
				case 'X':
				case 'x':
					return character_x_3;
				case 'Y':
				case 'y':
					return character_y_3;
				case 'Z':
				case 'z':
					return character_z_3;
				case '0':
					return character_0_3;
				case '1':
					return character_1_3;
				case '2':
					return character_2_3;
				case '3':
					return character_3_3;
				case '4':
					return character_4_3;
				case '5':
					return character_5_3;
				case '6':
					return character_6_3;
				case '7':
					return character_7_3;
				case '8':
					return character_8_3;
				case '9':
					return character_9_3;
				default:
					return character_space_3;
			}
		case 4:
			switch (character) {
				case ' ':
					return character_space_4;
				case 'A':
				case 'a':
					return character_a_4;
				case 'B':
				case 'b':
					return character_b_4;
				case 'C':
				case 'c':
					return character_c_4;
				case 'D':
				case 'd':
					return character_d_4;
				case 'E':
				case 'e':
					return character_e_4;
				case 'F':
				case 'f':
					return character_f_4;
				case 'G':
				case 'g':
					return character_g_4;
				case 'H':
				case 'h':
					return character_h_4;
				case 'I':
				case 'i':
					return character_i_4;
				case 'J':
				case 'j':
					return character_j_4;
				case 'K':
				case 'k':
					return character_k_4;
				case 'L':
				case 'l':
					return character_l_4;
				case 'M':
				case 'm':
					return character_m_4;
				case 'N':
				case 'n':
					return character_n_4;
				case 'O':
				case 'o':
					return character_o_4;
				case 'P':
				case 'p':
					return character_p_4;
				case 'Q':
				case 'q':
					return character_q_4;
				case 'R':
				case 'r':
					return character_r_4;
				case 'S':
				case 's':
					return character_s_4;
				case 'T':
				case 't':
					return character_t_4;
				case 'U':
				case 'u':
					return character_u_4;
				case 'V':
				case 'v':
					return character_v_4;
				case 'W':
				case 'w':
					return character_w_4;
				case 'X':
				case 'x':
					return character_x_4;
				case 'Y':
				case 'y':
					return character_y_4;
				case 'Z':
				case 'z':
					return character_z_4;
				case '0':
					return character_0_4;
				case '1':
					return character_1_4;
				case '2':
					return character_2_4;
				case '3':
					return character_3_4;
				case '4':
					return character_4_4;
				case '5':
					return character_5_4;
				case '6':
					return character_6_4;
				case '7':
					return character_7_4;
				case '8':
					return character_8_4;
				case '9':
					return character_9_4;
				default:
					return character_space_4;
			}
	}
}


// int characterArray[28] = {};


/* Get character
 * @param character - character to get array for
 * @param width - width of character
 * @return pointer to character array
 */
const TextCharacter::TextCharacterInfo TextCharacter::getCharacter(char character, int width)
{
	TextCharacterInfo returnCharacter;
	int* pCharacterArray = getCharacterArray(character, width);
	returnCharacter.characterArray = pCharacterArray;
	int characterTotalPixels;

	switch (width) {
		case 3:
			characterTotalPixels = 20;
			break;
		case 4:
			characterTotalPixels = 28;
	}

	returnCharacter.characterTotalPixels = characterTotalPixels;

	return returnCharacter;
}
