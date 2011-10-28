#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#include <xboot.h>

struct color_t {
	u8_t r;
	u8_t g;
	u8_t b;
	u8_t a;
};

struct color_t * get_color_by_name(const char * name);

#endif /* __GRAPHIC_COLOR_H__ */
