#include <TextCharacters.h>

TextCharacter::TextCharacter() {}

/* Letter Typography arrays */
// 3 x 5
#pragma region -- 3 x 5

const int8_t character_space_3x5[20] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

const int8_t character_a_3x5[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_b_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_c_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

const int8_t character_d_3x5[20] = {
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 0
};

const int8_t character_e_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

const int8_t character_f_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0
};

const int8_t character_g_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_h_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_i_3x5[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

const int8_t character_j_3x5[20] = {
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 0, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_k_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_l_3x5[20] = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

const int8_t character_m_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_n_3x5[20] = {
  0, 1, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_o_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_p_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 0, 0
};

const int8_t character_q_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 0, 0, 1
};

const int8_t character_r_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 1, 0,
  0, 1, 0, 1
};

const int8_t character_s_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

const int8_t character_t_3x5[20] = {
  0, 1, 1, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

const int8_t character_u_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_v_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0
};

const int8_t character_w_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1
};

const int8_t character_x_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1
};

const int8_t character_y_3x5[20] = {
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

const int8_t character_z_3x5[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

const int8_t character_0_3x5[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 1, 0, 1,
  0, 0, 1, 0
};

const int8_t character_1_3x5[20] = {
  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 1, 1
};

const int8_t character_2_3x5[20] = {
  0, 0, 1, 0,
  0, 1, 0, 1,
  0, 0, 1, 0,
  0, 1, 0, 0,
  0, 1, 1, 1
};

const int8_t character_3x5_3x5[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 1,
  0, 0, 0, 1,
  0, 1, 1, 1
};

const int8_t character_4_3x5[20] = {
  0, 0, 0, 1,
  0, 0, 1, 1,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1
};

const int8_t character_5_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 1,
  0, 1, 1, 0
};

const int8_t character_6_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 0,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_7_3x5[20] = {
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 1, 0, 0
};

const int8_t character_8_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1
};

const int8_t character_9_3x5[20] = {
  0, 1, 1, 1,
  0, 1, 0, 1,
  0, 1, 1, 1,
  0, 0, 0, 1,
  0, 0, 0, 1
};

#pragma endregion -- 3 x 5

#pragma ragion -- 3 x 7
const int8_t character_space_3x7[21] = {
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0
};

const int8_t character_a_3x7[21] = {
  0, 1, 0,
  0, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_b_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_c_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1
};

const int8_t character_d_3x7[21] = {
  1, 0, 0,
  1, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 0,
  1, 0, 0
};

const int8_t character_e_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 0,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1
};

const int8_t character_f_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0
};

const int8_t character_g_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_h_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_i_3x7[21] = {
  1, 1, 1,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  1, 1, 1
};

const int8_t character_j_3x7[21] = {
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_k_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_l_3x7[21] = {
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1
};

const int8_t character_m_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_n_3x7[21] = {
  1, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_o_3x7[21] = {
  0, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  0, 1, 0
};

const int8_t character_p_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 0, 0
};

const int8_t character_q_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  0, 1, 0,
  0, 0, 1
};

const int8_t character_r_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_s_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  1, 1, 1
};

const int8_t character_t_3x7[21] = {
  1, 1, 1,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0
};

const int8_t character_u_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_v_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  0, 1, 0,
  0, 1, 0
};

const int8_t character_w_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_x_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  0, 1, 0,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1
};

const int8_t character_y_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0
};

const int8_t character_z_3x7[21] = {
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  0, 1, 0,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1
};

const int8_t character_0_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_1_3x7[21] = {
  0, 1, 0,
  1, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  1, 1, 1
};

const int8_t character_2_3x7[21] = {
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1
};

const int8_t character_3_3x7[21] = {
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  1, 1, 1
};

const int8_t character_4_3x7[21] = {
  1, 0, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1
};

const int8_t character_5_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  1, 1, 1
};

const int8_t character_6_3x7[21] = {
  1, 1, 1,
  1, 0, 0,
  1, 0, 0,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_7_3x7[21] = {
  1, 1, 1,
  0, 0, 1,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  1, 0, 0,
  1, 0, 0
};

const int8_t character_8_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1
};

const int8_t character_9_3x7[21] = {
  1, 1, 1,
  1, 0, 1,
  1, 0, 1,
  1, 1, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1
};
#pragma endregion -- 3 x 7

// 4 wide
#pragma region -- 4 wide
const int8_t character_space_4[28] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 };

const int8_t character_a_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_b_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0 };

const int8_t character_c_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

const int8_t character_d_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0 };

const int8_t character_e_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

const int8_t character_f_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0 };

const int8_t character_g_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1 };

const int8_t character_h_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_i_4[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 1 };

const int8_t character_j_4[28] = {
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1 };

const int8_t character_k_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 0,
	1, 1, 0, 0,
	1, 0, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_l_4[28] = {
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

const int8_t character_m_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_n_4[28] = {
	1, 0, 0, 1,
	1, 1, 0, 1,
	1, 1, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_o_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

const int8_t character_p_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0 };

const int8_t character_q_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 0, 1, 0,
	1, 1, 1, 1 };

const int8_t character_r_4[28] = {
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_s_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

const int8_t character_t_4[28] = {
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0 };

const int8_t character_u_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

const int8_t character_v_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	0, 1, 1, 0 };

const int8_t character_w_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_x_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1 };

const int8_t character_y_4[28] = {
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 1 };

const int8_t character_z_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

const int8_t character_0_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 1, 1,
	1, 1, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 0,
	0, 1, 1, 0 };

const int8_t character_1_4[28] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 1 };

const int8_t character_2_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 1 };

const int8_t character_3_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0 };

const int8_t character_4_4[28] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	1, 0, 1, 0,
	1, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0 };

const int8_t character_5_4[28] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1,
	1, 1, 1, 0 };

const int8_t character_6_4[28] = {
	0, 1, 1, 1,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

const int8_t character_7_4[28] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0 };

const int8_t character_8_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0 };

const int8_t character_9_4[28] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 1, 0,
	0, 1, 0, 0 };

#pragma endregion -- 4 wide

// 5 wide
#pragma region -- 5 wide
const int8_t character_space_5[45] = {
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0 };

const int8_t character_a_5[45] = {
	0, 0, 1, 0, 0,
	0, 1, 0, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_b_5[45] = {
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 0 };

const int8_t character_c_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_d_5[45] = {
	1, 1, 1, 0, 0,
	1, 0, 0, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 1, 0,
	1, 1, 1, 0, 0 };

const int8_t character_e_5[45] = {
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 1 };

const int8_t character_f_5[45] = {
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0 };

const int8_t character_g_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 1, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 1 };

const int8_t character_h_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_i_5[45] = {
	1, 1, 1, 1, 1,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	1, 1, 1, 1, 1 };

const int8_t character_j_5[45] = {
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	1, 0, 0, 1, 0,
	1, 0, 0, 1, 0,
	0, 1, 1, 0, 0 };

const int8_t character_k_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 1, 1,
	1, 0, 1, 1, 0,
	1, 1, 1, 0, 0,
	1, 1, 0, 0, 0,
	1, 1, 1, 0, 0,
	1, 0, 1, 1, 0,
	1, 0, 0, 1, 1,
	1, 0, 0, 0, 1 };

const int8_t character_l_5[45] = {
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 1 };

const int8_t character_m_5[45] = {
	1, 0, 0, 0, 1,
	1, 1, 0, 1, 1,
	1, 1, 0, 1, 1,
	1, 0, 1, 0, 1,
	1, 0, 1, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_n_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 0, 0, 1,
	1, 1, 1, 0, 1,
	1, 0, 1, 1, 1,
	1, 0, 0, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_o_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_p_5[45] = {
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0 };

const int8_t character_q_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 1, 0, 1,
	1, 0, 1, 0, 1,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 1 };

const int8_t character_r_5[45] = {
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 0,
	1, 0, 0, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_s_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_t_5[45] = {
	1, 1, 1, 1, 1,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0 };

const int8_t character_u_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_v_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 0, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0 };

const int8_t character_w_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 1, 0, 1,
	1, 0, 1, 0, 1,
	1, 1, 0, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_x_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 0, 1, 0,
	0, 1, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 1, 0, 1, 0,
	0, 1, 0, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1 };

const int8_t character_y_5[45] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0 };

const int8_t character_z_5[45] = {
	1, 1, 1, 1, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 1, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 1 };

const int8_t character_0_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 1, 1,
	1, 0, 1, 0, 1,
	1, 1, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_1_5[45] = {
	0, 0, 1, 0, 0,
	0, 1, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 1, 1, 1, 0 };

const int8_t character_2_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 1, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 1 };

const int8_t character_3_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_4_5[45] = {
	1, 0, 0, 1, 0,
	1, 0, 0, 1, 0,
	1, 0, 0, 1, 0,
	1, 1, 1, 1, 1,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0 };

const int8_t character_5_5[45] = {
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 0,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_6_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 0,
	1, 0, 0, 0, 0,
	1, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_7_5[45] = {
	1, 1, 1, 1, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0 };

const int8_t character_8_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

const int8_t character_9_5[45] = {
	0, 1, 1, 1, 0,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 1,
	0, 0, 0, 0, 1,
	0, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	0, 1, 1, 1, 0 };

#pragma endregion -- 5 wide


/* Get character array
 * @param character - character to get array for
 * @param width - width of character
 * @return pointer to character array
 */
const int8_t* TextCharacter::getCharacterArray(char character, int width) {
	switch (width) {
		case 3:
			switch (character) {
				case ' ':
					return character_space_3x7;
				case 'A':
				case 'a':
					return character_a_3x7;
				case 'B':
				case 'b':
					return character_b_3x7;
				case 'C':
				case 'c':
					return character_c_3x7;
				case 'D':
				case 'd':
					return character_d_3x7;
				case 'E':
				case 'e':
					return character_e_3x7;
				case 'F':
				case 'f':
					return character_f_3x7;
				case 'G':
				case 'g':
					return character_g_3x7;
				case 'H':
				case 'h':
					return character_h_3x7;
				case 'I':
				case 'i':
					return character_i_3x7;
				case 'J':
				case 'j':
					return character_j_3x7;
				case 'K':
				case 'k':
					return character_k_3x7;
				case 'L':
				case 'l':
					return character_l_3x7;
				case 'M':
				case 'm':
					return character_m_3x7;
				case 'N':
				case 'n':
					return character_n_3x7;
				case 'O':
				case 'o':
					return character_o_3x7;
				case 'P':
				case 'p':
					return character_p_3x7;
				case 'Q':
				case 'q':
					return character_q_3x7;
				case 'R':
				case 'r':
					return character_r_3x7;
				case 'S':
				case 's':
					return character_s_3x7;
				case 'T':
				case 't':
					return character_t_3x7;
				case 'U':
				case 'u':
					return character_u_3x7;
				case 'V':
				case 'v':
					return character_v_3x7;
				case 'W':
				case 'w':
					return character_w_3x7;
				case 'X':
				case 'x':
					return character_x_3x7;
				case 'Y':
				case 'y':
					return character_y_3x7;
				case 'Z':
				case 'z':
					return character_z_3x7;
				case '0':
					return character_0_3x7;
				case '1':
					return character_1_3x7;
				case '2':
					return character_2_3x7;
				case '3':
					return character_3_3x7;
				case '4':
					return character_4_3x7;
				case '5':
					return character_5_3x7;
				case '6':
					return character_6_3x7;
				case '7':
					return character_7_3x7;
				case '8':
					return character_8_3x7;
				case '9':
					return character_9_3x7;
				default:
					return character_space_3x7;
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
		case 5:
			switch (character) {
				case ' ':
					return character_space_5;
				case 'A':
				case 'a':
					return character_a_5;
				case 'B':
				case 'b':
					return character_b_5;
				case 'C':
				case 'c':
					return character_c_5;
				case 'D':
				case 'd':
					return character_d_5;
				case 'E':
				case 'e':
					return character_e_5;
				case 'F':
				case 'f':
					return character_f_5;
				case 'G':
				case 'g':
					return character_g_5;
				case 'H':
				case 'h':
					return character_h_5;
				case 'I':
				case 'i':
					return character_i_5;
				case 'J':
				case 'j':
					return character_j_5;
				case 'K':
				case 'k':
					return character_k_5;
				case 'L':
				case 'l':
					return character_l_5;
				case 'M':
				case 'm':
					return character_m_5;
				case 'N':
				case 'n':
					return character_n_5;
				case 'O':
				case 'o':
					return character_o_5;
				case 'P':
				case 'p':
					return character_p_5;
				case 'Q':
				case 'q':
					return character_q_5;
				case 'R':
				case 'r':
					return character_r_5;
				case 'S':
				case 's':
					return character_s_5;
				case 'T':
				case 't':
					return character_t_5;
				case 'U':
				case 'u':
					return character_u_5;
				case 'V':
				case 'v':
					return character_v_5;
				case 'W':
				case 'w':
					return character_w_5;
				case 'X':
				case 'x':
					return character_x_5;
				case 'Y':
				case 'y':
					return character_y_5;
				case 'Z':
				case 'z':
					return character_z_5;
				case '0':
					return character_0_5;
				case '1':
					return character_1_5;
				case '2':
					return character_2_5;
				case '3':
					return character_3_5;
				case '4':
					return character_4_5;
				case '5':
					return character_5_5;
				case '6':
					return character_6_5;
				case '7':
					return character_7_5;
				case '8':
					return character_8_5;
				case '9':
					return character_9_5;
				default:
					return character_space_5;
			}
	}
}

/* Get character
 * @param character - character to get array for
 * @param width - width of character
 * @return pointer to character array
 */
const TextCharacter::TextCharacterInfo TextCharacter::getCharacter(char character, int width)
{
	TextCharacterInfo returnCharacter;
	const int8_t* pCharacterArray = getCharacterArray(character, width);
	returnCharacter.characterArray = pCharacterArray;
	int characterTotalPixels;

	switch (width) {
		case 3:
			characterTotalPixels = 20;
			break;
		case 4:
			characterTotalPixels = 28;
			break;
		case 5:
			characterTotalPixels = 45;
			break;
	}

	returnCharacter.characterTotalPixels = characterTotalPixels;

	return returnCharacter;
}
