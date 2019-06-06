#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	KEY_POWER					= 1,
	KEY_UP						= 2,
	KEY_DOWN					= 3,
	KEY_LEFT					= 4,
	KEY_RIGHT					= 5,
	KEY_VOLUME_UP				= 6,
	KEY_VOLUME_DOWN				= 7,
	KEY_VOLUME_MUTE				= 8,
	KEY_TAB						= 9,
	KEY_TASK					= 10,
	KEY_HOME					= 11,
	KEY_BACK					= 12,
	KEY_ENTER					= 13,

	KEY_L_CTRL					= 14,
	KEY_R_CTRL					= 15,
	KEY_L_ALT					= 16,
	KEY_R_ALT					= 17,
	KEY_L_SHIFT					= 18,
	KEY_R_SHIFT					= 19,

	KEY_F1						= 20,
	KEY_F2						= 21,
	KEY_F3						= 22,
	KEY_F4						= 23,
	KEY_F5						= 24,
	KEY_F6						= 25,
	KEY_F7						= 26,
	KEY_F8						= 27,
	KEY_F9						= 28,
	KEY_F10						= 29,
	KEY_F11						= 30,
	KEY_F12						= 31,

	KEY_SPACE					= 32,	/*   */
	KEY_EXCLAMATION_MARK		= 33,	/* ! */
	KEY_QUOTATION_MARK			= 34,	/* " */
	KEY_POUNDSIGN				= 35,	/* # */
	KEY_DOLLAR					= 36,	/* $ */
	KEY_PERCENT					= 37,	/* % */
	KEY_AMPERSAND				= 38,	/* & */
	KEY_APOSTROPHE				= 39,	/* ' */
	KEY_PARENTHESIS_LEFT		= 40,	/* ( */
	KEY_PARENTHESIS_RIGHT		= 41,	/* ) */
	KEY_ASTERISK				= 42,	/* * */
	KEY_PLUS					= 43,	/* + */
	KEY_COMMA					= 44,	/* , */
	KEY_MINUS					= 45,	/* - */
	KEY_FULL_STOP				= 46,	/* . */
	KEY_SOLIDUS					= 47,	/* / */

	KEY_0						= 48,	/* 0 */
	KEY_1						= 49,	/* 1 */
	KEY_2						= 50,	/* 2 */
	KEY_3						= 51,	/* 3 */
	KEY_4						= 52,	/* 4 */
	KEY_5						= 53,	/* 5 */
	KEY_6						= 54,	/* 6 */
	KEY_7						= 55,	/* 7 */
	KEY_8						= 56,	/* 8 */
	KEY_9						= 57,	/* 9 */

	KEY_COLON					= 58,	/* : */
	KEY_SEMICOLON				= 59,	/* ; */
	KEY_LESS_THAN				= 60,	/* < */
	KEY_EQUAL					= 61,	/* = */
	KEY_GREATER_THAN			= 62,	/* > */
	KEY_QUESTION_MARK			= 63,	/* ? */
	KEY_AT						= 64,	/* @ */

	KEY_A						= 65,	/* A */
	KEY_B						= 66,	/* B */
	KEY_C						= 67,	/* C */
	KEY_D						= 68,	/* D */
	KEY_E						= 69,	/* E */
	KEY_F						= 70,	/* F */
	KEY_G						= 71,	/* G */
	KEY_H						= 72,	/* H */
	KEY_I						= 73,	/* I */
	KEY_J						= 74,	/* J */
	KEY_K						= 75,	/* K */
	KEY_L						= 76,	/* L */
	KEY_M						= 77,	/* M */
	KEY_N						= 78,	/* N */
	KEY_O						= 79,	/* O */
	KEY_P						= 80,	/* P */
	KEY_Q						= 81,	/* Q */
	KEY_R						= 82,	/* R */
	KEY_S						= 83,	/* S */
	KEY_T						= 84,	/* T */
	KEY_U						= 85,	/* U */
	KEY_V						= 86,	/* V */
	KEY_W						= 87,	/* W */
	KEY_X						= 88,	/* X */
	KEY_Y						= 89,	/* Y */
	KEY_Z						= 90,	/* Z */

	KEY_SQUARE_BRACKET_LEFT		= 91,	/* [ */
	KEY_REVERSE_SOLIDUS			= 92,	/* \ */
	KEY_SQUARE_BRACKET_RIGHT	= 93,	/* ] */
	KEY_CIRCUMFLEX_ACCENT		= 94,	/* ^ */
	KEY_LOW_LINE				= 95,	/* _ */
	KEY_GRAVE_ACCENT			= 96,	/* ` */

	KEY_a						= 97,	/* a */
	KEY_b						= 98,	/* b */
	KEY_c						= 99,	/* c */
	KEY_d						= 100,	/* d */
	KEY_e						= 101,	/* e */
	KEY_f						= 102,	/* f */
	KEY_g						= 103,	/* g */
	KEY_h						= 104,	/* h */
	KEY_i						= 105,	/* i */
	KEY_j						= 106,	/* j */
	KEY_k						= 107,	/* k */
	KEY_l						= 108,	/* l */
	KEY_m						= 109,	/* m */
	KEY_n						= 110,	/* n */
	KEY_o						= 111,	/* o */
	KEY_p						= 112,	/* p */
	KEY_q						= 113,	/* q */
	KEY_r						= 114,	/* r */
	KEY_s						= 115,	/* s */
	KEY_t						= 116,	/* t */
	KEY_u						= 117,	/* u */
	KEY_v						= 118,	/* v */
	KEY_w						= 119,	/* w */
	KEY_x						= 120,	/* x */
	KEY_y						= 121,	/* y */
	KEY_z						= 122,	/* z */

	KEY_CURLY_BRACKET_LEFT		= 123,	/* { */
	KEY_VERTICAL_LINE			= 124,	/* | */
	KEY_CURLY_BRACKET_RIGHT		= 125,	/* } */
	KEY_TILDE					= 126,	/* ~ */
	KEY_DELETE					= 127,

	/* Latin 1 extensions */
	KEY_NO_BREAK_SPACE			= 160,	/*   */
	KEY_EXCLAM_DOWN				= 161,	/* ¡ */
	KEY_CENT					= 162,	/* ¢ */
	KEY_STERLING				= 163,	/* £ */
	KEY_CURRENCY				= 164,	/* ¤ */
	KEY_YEN						= 165,	/* ¥ */
	KEY_BROKEN_BAR				= 166,	/* ¦ */
	KEY_SECTION					= 167,	/* § */
	KEY_DIAERESIS				= 168,	/* ¨ */
	KEY_COPYRIGHT				= 169,	/* © */
	KEY_ORDFEMININE				= 170,	/* ª */
	KEY_GUILLEMOTLEFT			= 171,	/* « */
	KEY_NOTSIGN					= 172,	/* ¬ */
	KEY_HYPHEN					= 173,	/* ­  */
	KEY_REGISTERED				= 174,	/* ® */
	KEY_MACRON					= 175,	/* ¯ */
	KEY_DEGREE					= 176,	/* ° */
	KEY_PLUSMINUS				= 177,	/* ± */
	KEY_TWOSUPERIOR				= 178,	/* ² */
	KEY_THREESUPERIOR			= 179,	/* ³ */
	KEY_ACUTE					= 180,	/* ´ */
	KEY_MU						= 181,	/* µ */
	KEY_PARAGRAPH				= 182,	/* ¶ */
	KEY_PERIODCENTERED			= 183,	/* · */
	KEY_CEDILLA					= 184,	/* ¸ */
	KEY_ONESUPERIOR				= 185,	/* ¹ */
	KEY_MASCULINE				= 186,	/* º */
	KEY_GUILLEMOTRIGHT			= 187,	/* » */
	KEY_ONEQUARTER 				= 188,	/* ¼ */
	KEY_ONEHALF 				= 189,	/* ½ */
	KEY_THREEQUARTERS 			= 190,	/* ¾ */
	KEY_QUESTIONDOWN 			= 191,	/* ¿ */
	KEY_AGRAVE 					= 192,	/* À */
	KEY_AACUTE 					= 193,	/* Á */
	KEY_ACIRCUMFLEX 			= 194,	/* Â */
	KEY_ATILDE 					= 195,	/* Ã */
	KEY_ADIAERESIS 				= 196,	/* Ä */
	KEY_ARING 					= 197,	/* Å */
	KEY_AE 						= 198,	/* Æ */
	KEY_CCEDILLA 				= 199,	/* Ç */
	KEY_EGRAVE 					= 200,	/* È */
	KEY_EACUTE 					= 201,	/* É */
	KEY_ECIRCUMFLEX 			= 202,	/* Ê */
	KEY_EDIAERESIS 				= 203,	/* Ë */
	KEY_IGRAVE 					= 204,	/* Ì */
	KEY_IACUTE 					= 205,	/* Í */
	KEY_ICIRCUMFLEX 			= 206,	/* Î */
	KEY_IDIAERESIS 				= 207,	/* Ï */
	KEY_ETH 					= 208,	/* Ð */
	KEY_NTILDE 					= 209,	/* Ñ */
	KEY_OGRAVE 					= 210,	/* Ò */
	KEY_OACUTE 					= 211,	/* Ó */
	KEY_OCIRCUMFLEX 			= 212,	/* Ô */
	KEY_OTILDE 					= 213,	/* Õ */
	KEY_ODIAERESIS 				= 214,	/* Ö */
	KEY_MULTIPLY 				= 215,	/* × */
	KEY_OOBLIQUE 				= 216,	/* Ø */
	KEY_UGRAVE 					= 217,	/* Ù */
	KEY_UACUTE 					= 218,	/* Ú */
	KEY_UCIRCUMFLEX 			= 219,	/* Û */
	KEY_UDIAERESIS 				= 220,	/* Ü */
	KEY_YACUTE 					= 221,	/* Ý */
	KEY_THORN 					= 222,	/* Þ */
	KEY_ssharp 					= 223,	/* ß */
	KEY_agrave 					= 224,	/* à */
	KEY_aacute 					= 225,	/* á */
	KEY_acircumflex				= 226,	/* â */
	KEY_atilde 					= 227,	/* ã */
	KEY_adiaeresis 				= 228,	/* ä */
	KEY_aring					= 229,	/* å */
	KEY_ae 						= 230,	/* æ */
	KEY_ccedilla 				= 231,	/* ç */
	KEY_egrave 					= 232,	/* è */
	KEY_eacute 					= 233,	/* é */
	KEY_ecircumflex 			= 234,	/* ê */
	KEY_ediaeresis 				= 235,	/* ë */
	KEY_igrave					= 236,	/* ì */
	KEY_iacute					= 237,	/* í */
	KEY_icircumflex 			= 238,	/* î */
	KEY_idiaeresis				= 239,	/* ï */
	KEY_eth 					= 240,	/* ð */
	KEY_ntilde 					= 241,	/* ñ */
	KEY_ograve 					= 242,	/* ò */
	KEY_oacute 					= 243,	/* ó */
	KEY_ocircumflex 			= 244,	/* ô */
	KEY_otilde 					= 245,	/* õ */
	KEY_odiaeresis 				= 246,	/* ö */
	KEY_DIVISION 				= 247,	/* ÷ */
	KEY_ooblique 				= 248,	/* ø */
	KEY_ugrave 					= 249,	/* ù */
	KEY_uacute 					= 250,	/* ú */
	KEY_ucircumflex 			= 251,	/* û */
	KEY_udiaeresis				= 252,	/* ü */
	KEY_yacute 					= 253,	/* ý */
	KEY_thorn 					= 254,	/* þ */
	KEY_ydiaeresis 				= 255,	/* ÿ */
};

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H__ */
