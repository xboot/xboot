#ifndef __COLOR_H__
#define __COLOR_H__

#include <xboot.h>

/*
 * the structure of color
 */
struct color {
	u8_t	r;
	u8_t	g;
	u8_t 	b;
	u8_t	a;
};

struct color * get_color_by_name(const char * name);

#endif /* __COLOR_H__ */
