#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>

enum key_code {
	KEY_CTRL_A					= 0x0001,
	KEY_CTRL_B					= 0x0002,
	KEY_CTRL_C					= 0x0003,
	KEY_CTRL_D					= 0x0004,
	KEY_CTRL_E					= 0x0005,
	KEY_CTRL_F					= 0x0006,
	KEY_CTRL_G					= 0x0007,
	KEY_CTRL_H					= 0x0008,
	KEY_CTRL_I					= 0x0009,
	KEY_CTRL_J					= 0x000a,
	KEY_CTRL_K					= 0x000b,
	KEY_CTRL_L					= 0x000c,
	KEY_CTRL_M					= 0x000d,
	KEY_CTRL_N					= 0x000e,
	KEY_CTRL_O					= 0x000f,
	KEY_CTRL_P					= 0x0010,
	KEY_CTRL_Q					= 0x0011,
	KEY_CTRL_R					= 0x0012,
	KEY_CTRL_S					= 0x0013,
	KEY_CTRL_T					= 0x0014,
	KEY_CTRL_U					= 0x0015,
	KEY_CTRL_V					= 0x0016,
	KEY_CTRL_W					= 0x0017,
	KEY_CTRL_X					= 0x0018,
	KEY_CTRL_Y					= 0x0019,
	KEY_CTRL_Z					= 0x001a,

	KEY_SPACE					= 0x0020,	/*   */
	KEY_EXCLAMATION_MARK		= 0x0021,	/* ! */
	KEY_QUOTATION_MARK			= 0x0022,	/* " */
	KEY_POUNDSIGN				= 0x0023,	/* # */
	KEY_DOLLAR					= 0x0024,	/* $ */
	KEY_PERCENT					= 0x0025,	/* % */
	KEY_AMPERSAND				= 0x0026,	/* & */
	KEY_APOSTROPHE				= 0x0027,	/* ' */
	KEY_PARENTHESIS_LEFT		= 0x0028,	/* ( */
	KEY_PARENTHESIS_RIGHT		= 0x0029,	/* ) */
	KEY_ASTERISK				= 0x002a,	/* * */
	KEY_PLUS					= 0x002b,	/* + */
	KEY_COMMA					= 0x002c,	/* , */
	KEY_MINUS					= 0x002d,	/* - */
	KEY_FULL_STOP				= 0x002e,	/* . */
	KEY_SOLIDUS					= 0x002f,	/* / */

	KEY_0						= 0x0030,
	KEY_1						= 0x0031,
	KEY_2						= 0x0032,
	KEY_3						= 0x0033,
	KEY_4						= 0x0034,
	KEY_5						= 0x0035,
	KEY_6						= 0x0036,
	KEY_7						= 0x0037,
	KEY_8						= 0x0038,
	KEY_9						= 0x0039,

	KEY_COLON					= 0x003a,	/* : */
	KEY_SEMICOLON				= 0x003b,	/* ; */
	KEY_LESS_THAN				= 0x003c,	/* < */
	KEY_EQUAL					= 0x003d,	/* = */
	KEY_GREATER_THAN			= 0x003e,	/* > */
	KEY_QUESTION_MARK			= 0x003f,	/* ? */
	KEY_AT						= 0x0040,	/* @ */

	KEY_A						= 0x0041,
	KEY_B						= 0x0042,
	KEY_C						= 0x0043,
	KEY_D						= 0x0044,
	KEY_E						= 0x0045,
	KEY_F						= 0x0046,
	KEY_G						= 0x0047,
	KEY_H						= 0x0048,
	KEY_I						= 0x0049,
	KEY_J						= 0x004a,
	KEY_K						= 0x004b,
	KEY_L						= 0x004c,
	KEY_M						= 0x004d,
	KEY_N						= 0x004e,
	KEY_O						= 0x004f,
	KEY_P						= 0x0050,
	KEY_Q						= 0x0051,
	KEY_R						= 0x0052,
	KEY_S						= 0x0053,
	KEY_T						= 0x0054,
	KEY_U						= 0x0055,
	KEY_V						= 0x0056,
	KEY_W						= 0x0057,
	KEY_X						= 0x0058,
	KEY_Y						= 0x0059,
	KEY_Z						= 0x005a,

	KEY_SQUARE_BRACKET_LEFT		= 0x005b,	/* [ */
	KEY_REVERSE_SOLIDUS			= 0x005c,	/* \ */
	KEY_SQUARE_BRACKET_RIGHT	= 0x005d,	/* ] */
	KEY_CIRCUMFLEX_ACCENT		= 0x005e,	/* ^ */
	KEY_LOW_LINE				= 0x005f,	/* _ */
	KEY_GRAVE_ACCENT			= 0x0060,	/* ` */

	KEY_a						= 0x0061,
	KEY_b						= 0x0062,
	KEY_c						= 0x0063,
	KEY_d						= 0x0064,
	KEY_e						= 0x0065,
	KEY_f						= 0x0066,
	KEY_g						= 0x0067,
	KEY_h						= 0x0068,
	KEY_i						= 0x0069,
	KEY_j						= 0x006a,
	KEY_k						= 0x006b,
	KEY_l						= 0x006c,
	KEY_m						= 0x006d,
	KEY_n						= 0x006e,
	KEY_o						= 0x006f,
	KEY_p						= 0x0070,
	KEY_q						= 0x0071,
	KEY_r						= 0x0072,
	KEY_s						= 0x0073,
	KEY_t						= 0x0074,
	KEY_u						= 0x0075,
	KEY_v						= 0x0076,
	KEY_w						= 0x0077,
	KEY_x						= 0x0078,
	KEY_y						= 0x0079,
	KEY_z						= 0x007a,

	KEY_CURLY_BRACKET_LEFT		= 0x007b,	/* { */
	KEY_VERTICAL_LINE			= 0x007c,	/* | */
	KEY_CURLY_BRACKET_RIGHT		= 0x007d,	/* } */
	KEY_TILDE					= 0x007e,	/* ~ */

	KEY_UP						= 0x0080,
	KEY_DOWN					= 0x0081,
	KEY_LEFT					= 0x0082,
	KEY_RIGHT					= 0x0083,
	KEY_TAB						= 0x0084,
	KEY_BACKSPACE				= 0x0085,
	KEY_ENTER					= 0x0086,
	KEY_HOME					= 0x0087,
	KEY_MENU					= 0x0088,
	KEY_BACK					= 0x0089,
	KEY_POWER					= 0x008a,
	KEY_RESET					= 0x008b,
};

enum key_value {
	KEY_BUTTON_UP				= 0,
	KEY_BUTTON_DOWN				= 1,
};

typedef void (*handler_onkeyraw)(struct input_event * event);
typedef void (*handler_onkeyup)(enum key_code key);
typedef void (*handler_onkeydown)(enum key_code key);


bool_t install_listener_onkeyraw(handler_onkeyraw raw);
bool_t remove_listener_onkeyraw(handler_onkeyraw raw);
bool_t install_listener_onkeyup(handler_onkeyup keyup);
bool_t remove_listener_onkeyup(handler_onkeyup keyup);
bool_t install_listener_onkeydown(handler_onkeydown keydown);
bool_t remove_listener_onkeydown(handler_onkeydown keydown);

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H__ */
