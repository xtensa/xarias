#ifndef XARIAS_FONTS
#define XARIAS_FONTS

#include "../utils/font5x7/font5x7.h"
#include "../utils/font16x24/font16x24.h"

/*
 * Font 5x7 mapping table
 */
const prog_uint8_t *font5x7[128] = {NULL};

void init_font5x7()
{
	font5x7['A']=PIC_A;
	font5x7['B']=PIC_B;
	font5x7['C']=PIC_C;
	font5x7['D']=PIC_D;
	font5x7['E']=PIC_E;
	font5x7['F']=PIC_F;
	font5x7['G']=PIC_G;
	font5x7['H']=PIC_H;
	font5x7['I']=PIC_I;
	font5x7['J']=PIC_J;
	font5x7['K']=PIC_K;
	font5x7['L']=PIC_L;
	font5x7['M']=PIC_M;
	font5x7['N']=PIC_N;
	font5x7['O']=PIC_O;
	font5x7['P']=PIC_P;
	font5x7['Q']=PIC_Q;
	font5x7['R']=PIC_R;
	font5x7['S']=PIC_S;
	font5x7['T']=PIC_T;
	font5x7['U']=PIC_U;
	font5x7['V']=PIC_V;
	font5x7['W']=PIC_W;
	font5x7['X']=PIC_X;
	font5x7['Y']=PIC_Y;
	font5x7['Z']=PIC_Z;
	font5x7['a']=PIC_a;
	font5x7['b']=PIC_b;
	font5x7['c']=PIC_c;
	font5x7['d']=PIC_d;
	font5x7['e']=PIC_e;
	font5x7['f']=PIC_f;
	font5x7['g']=PIC_g;
	font5x7['h']=PIC_h;
	font5x7['i']=PIC_i;
	font5x7['j']=PIC_j;
	font5x7['k']=PIC_k;
	font5x7['l']=PIC_l;
	font5x7['m']=PIC_m;
	font5x7['n']=PIC_n;
	font5x7['o']=PIC_o;
	font5x7['p']=PIC_p;
	font5x7['q']=PIC_q;
	font5x7['r']=PIC_r;
	font5x7['s']=PIC_s;
	font5x7['t']=PIC_t;
	font5x7['u']=PIC_u;
	font5x7['v']=PIC_v;
	font5x7['w']=PIC_w;
	font5x7['x']=PIC_x;
	font5x7['y']=PIC_y;
	font5x7['z']=PIC_z;
	font5x7['/']=PIC_slash;
	font5x7['\\']=PIC_backslash;
	font5x7[' ']=PIC_space;
	font5x7['[']=PIC_lsqbracket;
	font5x7[']']=PIC_rsqbracket;
	font5x7['(']=PIC_lbracket;
	font5x7[')']=PIC_rbracket;
	font5x7['!']=PIC_bang;
	font5x7['@']=PIC_at;
	font5x7['#']=PIC_hash;
	font5x7['$']=PIC_dollar;
	font5x7['%']=PIC_percent;
	font5x7['^']=PIC_xor;
	font5x7['&']=PIC_and;
	font5x7['*']=PIC_star;
	font5x7['-']=PIC_minus;
	font5x7['_']=PIC_underline;
	font5x7['=']=PIC_equal;
	font5x7['+']=PIC_plus;
	font5x7['|']=PIC_vline;
	font5x7['?']=PIC_question;
	font5x7['<']=PIC_ltrbracket;
	font5x7['>']=PIC_rtrbracket;
	font5x7[':']=PIC_colon;
	font5x7[';']=PIC_semicolon;
	font5x7['\'']=PIC_prime;
	font5x7['"']=PIC_dprime;
	font5x7['.']=PIC_dot;
	font5x7[',']=PIC_comma;
	font5x7['0']=PIC_0;
	font5x7['1']=PIC_1;
	font5x7['2']=PIC_2;
	font5x7['3']=PIC_3;
	font5x7['4']=PIC_4;
	font5x7['5']=PIC_5;
	font5x7['6']=PIC_6;
	font5x7['7']=PIC_7;
	font5x7['8']=PIC_8;
	font5x7['9']=PIC_9;
/*	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
	font5x7['']=PIC_;
*/	
}


/*
 * Font 16x24 apping table
 */
const prog_uint8_t *font16x24[64] = {NULL};

void init_font16x24()
{
	font16x24[' ']=PIC_fb_space;
	font16x24['.']=PIC_fb_dot;
	font16x24[':']=PIC_fb_colon;
	font16x24['0']=PIC_fb_0;
	font16x24['1']=PIC_fb_1;
	font16x24['2']=PIC_fb_2;
	font16x24['3']=PIC_fb_3;
	font16x24['4']=PIC_fb_4;
	font16x24['5']=PIC_fb_5;
	font16x24['6']=PIC_fb_6;
	font16x24['7']=PIC_fb_7;
	font16x24['8']=PIC_fb_8;
	font16x24['9']=PIC_fb_9;
}

#endif /* XARIAS_FONTS */
