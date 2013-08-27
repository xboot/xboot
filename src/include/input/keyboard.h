#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum key_code_t {
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

	KEY_0						= 0x0030,	/* 0 */
	KEY_1						= 0x0031,	/* 1 */
	KEY_2						= 0x0032,	/* 2 */
	KEY_3						= 0x0033,	/* 3 */
	KEY_4						= 0x0034,	/* 4 */
	KEY_5						= 0x0035,	/* 5 */
	KEY_6						= 0x0036,	/* 6 */
	KEY_7						= 0x0037,	/* 7 */
	KEY_8						= 0x0038,	/* 8 */
	KEY_9						= 0x0039,	/* 9 */

	KEY_COLON					= 0x003a,	/* : */
	KEY_SEMICOLON				= 0x003b,	/* ; */
	KEY_LESS_THAN				= 0x003c,	/* < */
	KEY_EQUAL					= 0x003d,	/* = */
	KEY_GREATER_THAN			= 0x003e,	/* > */
	KEY_QUESTION_MARK			= 0x003f,	/* ? */
	KEY_AT						= 0x0040,	/* @ */

	KEY_A						= 0x0041,	/* A */
	KEY_B						= 0x0042,	/* B */
	KEY_C						= 0x0043,	/* C */
	KEY_D						= 0x0044,	/* D */
	KEY_E						= 0x0045,	/* E */
	KEY_F						= 0x0046,	/* F */
	KEY_G						= 0x0047,	/* G */
	KEY_H						= 0x0048,	/* H */
	KEY_I						= 0x0049,	/* I */
	KEY_J						= 0x004a,	/* J */
	KEY_K						= 0x004b,	/* K */
	KEY_L						= 0x004c,	/* L */
	KEY_M						= 0x004d,	/* M */
	KEY_N						= 0x004e,	/* N */
	KEY_O						= 0x004f,	/* O */
	KEY_P						= 0x0050,	/* P */
	KEY_Q						= 0x0051,	/* Q */
	KEY_R						= 0x0052,	/* R */
	KEY_S						= 0x0053,	/* S */
	KEY_T						= 0x0054,	/* T */
	KEY_U						= 0x0055,	/* U */
	KEY_V						= 0x0056,	/* V */
	KEY_W						= 0x0057,	/* W */
	KEY_X						= 0x0058,	/* X */
	KEY_Y						= 0x0059,	/* Y */
	KEY_Z						= 0x005a,	/* Z */

	KEY_SQUARE_BRACKET_LEFT		= 0x005b,	/* [ */
	KEY_REVERSE_SOLIDUS			= 0x005c,	/* \ */
	KEY_SQUARE_BRACKET_RIGHT	= 0x005d,	/* ] */
	KEY_CIRCUMFLEX_ACCENT		= 0x005e,	/* ^ */
	KEY_LOW_LINE				= 0x005f,	/* _ */
	KEY_GRAVE_ACCENT			= 0x0060,	/* ` */

	KEY_a						= 0x0061,	/* a */
	KEY_b						= 0x0062,	/* b */
	KEY_c						= 0x0063,	/* c */
	KEY_d						= 0x0064,	/* d */
	KEY_e						= 0x0065,	/* e */
	KEY_f						= 0x0066,	/* f */
	KEY_g						= 0x0067,	/* g */
	KEY_h						= 0x0068,	/* h */
	KEY_i						= 0x0069,	/* i */
	KEY_j						= 0x006a,	/* j */
	KEY_k						= 0x006b,	/* k */
	KEY_l						= 0x006c,	/* l */
	KEY_m						= 0x006d,	/* m */
	KEY_n						= 0x006e,	/* n */
	KEY_o						= 0x006f,	/* o */
	KEY_p						= 0x0070,	/* p */
	KEY_q						= 0x0071,	/* q */
	KEY_r						= 0x0072,	/* r */
	KEY_s						= 0x0073,	/* s */
	KEY_t						= 0x0074,	/* t */
	KEY_u						= 0x0075,	/* u */
	KEY_v						= 0x0076,	/* v */
	KEY_w						= 0x0077,	/* w */
	KEY_x						= 0x0078,	/* x */
	KEY_y						= 0x0079,	/* y */
	KEY_z						= 0x007a,	/* z */

	KEY_CURLY_BRACKET_LEFT		= 0x007b,	/* { */
	KEY_VERTICAL_LINE			= 0x007c,	/* | */
	KEY_CURLY_BRACKET_RIGHT		= 0x007d,	/* } */
	KEY_TILDE					= 0x007e,	/* ~ */

	/* Latin 1 extensions */
	KEY_NO_BREAK_SPACE			= 0x00a0,	/*   */
	KEY_EXCLAM_DOWN				= 0x00a1,	/* ¡ */
	KEY_CENT					= 0x00a2,	/* ¢ */
	KEY_STERLING				= 0x00a3,	/* £ */
	KEY_CURRENCY				= 0x00a4,	/* ¤ */
	KEY_YEN						= 0x00a5,	/* ¥ */
	KEY_BROKEN_BAR				= 0x00a6,	/* ¦ */
	KEY_SECTION					= 0x00a7,	/* § */
	KEY_DIAERESIS				= 0x00a8,	/* ¨ */
	KEY_COPYRIGHT				= 0x00a9,	/* © */
	KEY_ORDFEMININE				= 0x00aa,	/* ª */
	KEY_GUILLEMOTLEFT			= 0x00ab,	/* « */
	KEY_NOTSIGN					= 0x00ac,	/* ¬ */
	KEY_HYPHEN					= 0x00ad,	/* ­  */
	KEY_REGISTERED				= 0x00ae,	/* ® */
	KEY_MACRON					= 0x00af,	/* ¯ */
	KEY_DEGREE					= 0x00b0,	/* ° */
	KEY_PLUSMINUS				= 0x00b1,	/* ± */
	KEY_TWOSUPERIOR				= 0x00b2,	/* ² */
	KEY_THREESUPERIOR			= 0x00b3,	/* ³ */
	KEY_ACUTE					= 0x00b4,	/* ´ */
	KEY_MU						= 0x00b5,	/* µ */
	KEY_PARAGRAPH				= 0x00b6,	/* ¶ */
	KEY_PERIODCENTERED			= 0x00b7,	/* · */
	KEY_CEDILLA					= 0x00b8,	/* ¸ */
	KEY_ONESUPERIOR				= 0x00b9,	/* ¹ */
	KEY_MASCULINE				= 0x00ba,	/* º */
	KEY_GUILLEMOTRIGHT			= 0x00bb,	/* » */
	KEY_ONEQUARTER 				= 0x00bc,	/* ¼ */
	KEY_ONEHALF 				= 0x00bd,	/* ½ */
	KEY_THREEQUARTERS 			= 0x00be,	/* ¾ */
	KEY_QUESTIONDOWN 			= 0x00bf,	/* ¿ */
	KEY_AGRAVE 					= 0x00c0,	/* À */
	KEY_AACUTE 					= 0x00c1,	/* Á */
	KEY_ACIRCUMFLEX 			= 0x00c2,	/* Â */
	KEY_ATILDE 					= 0x00c3,	/* Ã */
	KEY_ADIAERESIS 				= 0x00c4,	/* Ä */
	KEY_ARING 					= 0x00c5,	/* Å */
	KEY_AE 						= 0x00c6,	/* Æ */
	KEY_CCEDILLA 				= 0x00c7,	/* Ç */
	KEY_EGRAVE 					= 0x00c8,	/* È */
	KEY_EACUTE 					= 0x00c9,	/* É */
	KEY_ECIRCUMFLEX 			= 0x00ca,	/* Ê */
	KEY_EDIAERESIS 				= 0x00cb,	/* Ë */
	KEY_IGRAVE 					= 0x00cc,	/* Ì */
	KEY_IACUTE 					= 0x00cd,	/* Í */
	KEY_ICIRCUMFLEX 			= 0x00ce,	/* Î */
	KEY_IDIAERESIS 				= 0x00cf,	/* Ï */
	KEY_ETH 					= 0x00d0,	/* Ð */
	KEY_NTILDE 					= 0x00d1,	/* Ñ */
	KEY_OGRAVE 					= 0x00d2,	/* Ò */
	KEY_OACUTE 					= 0x00d3,	/* Ó */
	KEY_OCIRCUMFLEX 			= 0x00d4,	/* Ô */
	KEY_OTILDE 					= 0x00d5,	/* Õ */
	KEY_ODIAERESIS 				= 0x00d6,	/* Ö */
	KEY_MULTIPLY 				= 0x00d7,	/* × */
	KEY_OOBLIQUE 				= 0x00d8,	/* Ø */
	KEY_UGRAVE 					= 0x00d9,	/* Ù */
	KEY_UACUTE 					= 0x00da,	/* Ú */
	KEY_UCIRCUMFLEX 			= 0x00db,	/* Û */
	KEY_UDIAERESIS 				= 0x00dc,	/* Ü */
	KEY_YACUTE 					= 0x00dd,	/* Ý */
	KEY_THORN 					= 0x00de,	/* Þ */
	KEY_ssharp 					= 0x00df,	/* ß */
	KEY_agrave 					= 0x00e0,	/* à */
	KEY_aacute 					= 0x00e1,	/* á */
	KEY_acircumflex				= 0x00e2,	/* â */
	KEY_atilde 					= 0x00e3,	/* ã */
	KEY_adiaeresis 				= 0x00e4,	/* ä */
	KEY_aring					= 0x00e5,	/* å */
	KEY_ae 						= 0x00e6,	/* æ */
	KEY_ccedilla 				= 0x00e7,	/* ç */
	KEY_egrave 					= 0x00e8,	/* è */
	KEY_eacute 					= 0x00e9,	/* é */
	KEY_ecircumflex 			= 0x00ea,	/* ê */
	KEY_ediaeresis 				= 0x00eb,	/* ë */
	KEY_igrave					= 0x00ec,	/* ì */
	KEY_iacute					= 0x00ed,	/* í */
	KEY_icircumflex 			= 0x00ee,	/* î */
	KEY_idiaeresis				= 0x00ef,	/* ï */
	KEY_eth 					= 0x00f0,	/* ð */
	KEY_ntilde 					= 0x00f1,	/* ñ */
	KEY_ograve 					= 0x00f2,	/* ò */
	KEY_oacute 					= 0x00f3,	/* ó */
	KEY_ocircumflex 			= 0x00f4,	/* ô */
	KEY_otilde 					= 0x00f5,	/* õ */
	KEY_odiaeresis 				= 0x00f6,	/* ö */
	KEY_DIVISION 				= 0x00f7,	/* ÷ */
	KEY_ooblique 				= 0x00f8,	/* ø */
	KEY_ugrave 					= 0x00f9,	/* ù */
	KEY_uacute 					= 0x00fa,	/* ú */
	KEY_ucircumflex 			= 0x00fb,	/* û */
	KEY_udiaeresis				= 0x00fc,	/* ü */
	KEY_yacute 					= 0x00fd,	/* ý */
	KEY_thorn 					= 0x00fe,	/* þ */
	KEY_ydiaeresis 				= 0x00ff,	/* ÿ */

	/* special keys */
	KEY_L_CTRL					= 0x0080,
	KEY_R_CTRL					= 0x0081,
	KEY_L_ALT					= 0x0082,
	KEY_R_ALT					= 0x0083,
	KEY_L_SHIFT					= 0x0084,
	KEY_R_SHIFT					= 0x0085,
	KEY_LOCK_CAPS				= 0x0086,
	KEY_LOCK_NUM				= 0x0087,
	KEY_LOCK_SCROLL				= 0x0088,

	KEY_BACKSPACE				= 0x0090,
	KEY_TAB						= 0x0091,
	KEY_ENTER					= 0x0092,
	KEY_UP						= 0x0093,
	KEY_DOWN					= 0x0094,
	KEY_LEFT					= 0x0095,
	KEY_RIGHT					= 0x0096,
	KEY_PAGE_UP					= 0x0097,
	KEY_PAGE_DOWN				= 0x0098,
	KEY_HOME					= 0x0099,
	KEY_END						= 0x009a,
	KEY_VOLUME_UP				= 0x009b,
	KEY_VOLUME_DOWN				= 0x009c,
	KEY_MENU					= 0x009d,
	KEY_BACK					= 0x009e,
	KEY_POWER					= 0x009f,
};

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H__ */
