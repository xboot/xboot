/*
 * libc/ctype/ctype.c
 */

#include <ctype.h>

const char __const_ctype[] = {
	0,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_S|(char)_B,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_N,	_N,	_N,	_N,	_N,	_N,	_N,	_N,
	_N,	_N,	_P,	_P,	_P,	_P,	_P,	_P,
	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,
	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C,

	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C, /* 80 */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C, /* 88 */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C, /* 90 */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C, /* 98 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* A0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* A8 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* B0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* B8 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* C0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* C8 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* D0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* D8 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* E0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* E8 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P, /* F0 */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P  /* F8 */
};
