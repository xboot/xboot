#ifndef __COLOR_H__
#define __COLOR_H__

#include <configs.h>
#include <default.h>

/*
 * the structure of color
 */
struct color {
	x_u8	r;
	x_u8	g;
	x_u8 	b;
	x_u8	a;
};

struct color * get_named_color(const char * name);

#endif /* __COLOR_H__ */
