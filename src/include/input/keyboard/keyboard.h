#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <configs.h>
#include <default.h>

enum keycode {
	KEY_NULL = 0,

	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_a,
	KEY_b,
	KEY_c,
	KEY_d,
	KEY_e,
	KEY_f,
	KEY_g,
	KEY_h,
	KEY_i,
	KEY_j,
	KEY_k,
	KEY_l,
	KEY_m,
	KEY_n,
	KEY_o,
	KEY_p,
	KEY_q,
	KEY_r,
	KEY_s,
	KEY_t,
	KEY_u,
	KEY_v,
	KEY_w,
	KEY_x,
	KEY_y,
	KEY_z,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_UNQUOTE,				/* ` */
	KEY_WAVE,					/* ~ */
	KEY_EXCLAMATION_MARK,		/* ! */
	KEY_AT,						/* @ */
	KEY_POUNDSIGN,				/* # */
	KEY_DOLLAR,					/* $ */
	KEY_PERCENT,				/* % */
	KEY_POWER,					/* ^ */
	KEY_AND,					/* & */
	KEY_STAR,					/* * */
	KEY_MINUS,					/* - */
	KEY_PLUS,					/* + */
	KEY_EQUAL,					/* = */
	KEY_OR,						/* | */
	KEY_SLASH,					/* / */
	KEY_BACKLASH,				/* \ */
	KEY_UNDERLINE,				/* _ */
	KEY_COMMA,					/* , */
	KEY_PERIOD,					/* . */
	KEY_QUESTION_MARK,			/* ? */
	KEY_COLON,					/* : */
	KEY_SEMICOLON,				/* ; */
	KEY_SINGLE_QUOTES,			/* ' */
	KEY_DOUBLE_QUOTES,			/* " */
	KEY_ANGLE_BRACKET_LEFT,		/* < */
	KEY_ANGLE_BRACKET_RIGHT,	/* > */
	KEY_ROUND_BRACKET_LEFT,		/* ( */
	KEY_ROUND_BRACKET_RIGHT,	/* ) */
	KEY_SQUARE_BRACKET_LEFT,	/* [ */
	KEY_SQUARE_BRACKET_RIGHT,	/* ] */
	KEY_BRACE_LEFT,				/* { */
	KEY_BRACE_RIGHT,			/* } */

	KEY_CTRL_0,					/* ctrl-0 */
	KEY_CTRL_1,					/* ctrl-1 */
	KEY_CTRL_2,					/* ctrl-2 */
	KEY_CTRL_3,					/* ctrl-3 */
	KEY_CTRL_4,					/* ctrl-4 */
	KEY_CTRL_5,					/* ctrl-5 */
	KEY_CTRL_6,					/* ctrl-6 */
	KEY_CTRL_7,					/* ctrl-7 */
	KEY_CTRL_8,					/* ctrl-8 */
	KEY_CTRL_9,					/* ctrl-9 */

	KEY_CTRL_A,					/* ctrl-a */
	KEY_CTRL_B,					/* ctrl-b */
	KEY_CTRL_C,					/* ctrl-c */
	KEY_CTRL_D,					/* ctrl-d */
	KEY_CTRL_E,					/* ctrl-e */
	KEY_CTRL_F,					/* ctrl-f */
	KEY_CTRL_G,					/* ctrl-g */
	KEY_CTRL_H,					/* ctrl-h */
	KEY_CTRL_I,					/* ctrl-i */
	KEY_CTRL_J,					/* ctrl-j */
	KEY_CTRL_K,					/* ctrl-k */
	KEY_CTRL_L,					/* ctrl-l */
	KEY_CTRL_M,					/* ctrl-m */
	KEY_CTRL_N,					/* ctrl-n */
	KEY_CTRL_O,					/* ctrl-o */
	KEY_CTRL_P,					/* ctrl-p */
	KEY_CTRL_Q,					/* ctrl-q */
	KEY_CTRL_R,					/* ctrl-r */
	KEY_CTRL_S,					/* ctrl-s */
	KEY_CTRL_T,					/* ctrl-t */
	KEY_CTRL_U,					/* ctrl-u */
	KEY_CTRL_V,					/* ctrl-v */
	KEY_CTRL_W,					/* ctrl-w */
	KEY_CTRL_X,					/* ctrl-x */
	KEY_CTRL_Y,					/* ctrl-y */
	KEY_CTRL_Z,					/* ctrl-z */

	KEY_ALT_0,					/* alt-0 */
	KEY_ALT_1,					/* alt-1 */
	KEY_ALT_2,					/* alt-2 */
	KEY_ALT_3,					/* alt-3 */
	KEY_ALT_4,					/* alt-4 */
	KEY_ALT_5,					/* alt-5 */
	KEY_ALT_6,					/* alt-6 */
	KEY_ALT_7,					/* alt-7 */
	KEY_ALT_8,					/* alt-8 */
	KEY_ALT_9,					/* alt-9 */

	KEY_ALT_A,					/* alt-a */
	KEY_ALT_B,					/* alt-b */
	KEY_ALT_C,					/* alt-c */
	KEY_ALT_D,					/* alt-d */
	KEY_ALT_E,					/* alt-e */
	KEY_ALT_F,					/* alt-f */
	KEY_ALT_G,					/* alt-g */
	KEY_ALT_H,					/* alt-h */
	KEY_ALT_I,					/* alt-i */
	KEY_ALT_J,					/* alt-j */
	KEY_ALT_K,					/* alt-k */
	KEY_ALT_L,					/* alt-l */
	KEY_ALT_M,					/* alt-m */
	KEY_ALT_N,					/* alt-n */
	KEY_ALT_O,					/* alt-o */
	KEY_ALT_P,					/* alt-p */
	KEY_ALT_Q,					/* alt-q */
	KEY_ALT_R,					/* alt-r */
	KEY_ALT_S,					/* alt-s */
	KEY_ALT_T,					/* alt-t */
	KEY_ALT_U,					/* alt-u */
	KEY_ALT_V,					/* alt-v */
	KEY_ALT_W,					/* alt-w */
	KEY_ALT_X,					/* alt-x */
	KEY_ALT_Y,					/* alt-y */
	KEY_ALT_Z,					/* alt-z */

	KEY_ESC,
	KEY_TAB,
	KEY_SPACE,
	KEY_ENTER,
	KEY_BACKSPACE,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_INSERT,
	KEY_DELETE,

	KEY_STANDBY,				/* pm - standby */
	KEY_RESUME,					/* pm - resume */
	KEY_HALT,					/* pm - halt */
	KEY_RESET,					/* pm - reset */
};

/**
 * defined the struct of keyboard driver, which contains
 * low level operating fuction.
 */
struct keyboard_driver
{
	/* the keyboard name */
	const char * name;

	/*initialize the keyboard */
	void (*init)(void);

	/* clean up the keyboard */
	void (*exit)(void);

	/* read keyboard */
	x_bool (*read)(enum keycode * code);

	/* flush keyboard's buffer */
	void (*flush)(void);

	/* ioctl keyboard */
	x_s32 (*ioctl)(x_u32 cmd, x_u32 arg);

	/* driver device */
	void * device;
};


x_bool register_keyboard(struct keyboard_driver * drv);
x_bool unregister_keyboard(struct keyboard_driver * drv);


#endif /* __KEYBOARD_H__ */
