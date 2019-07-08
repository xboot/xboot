#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct color_t {
	double r;
	double g;
	double b;
	double a;
};

static inline void color_init(struct color_t * c, double r, double g, double b, double a)
{
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = a;
}

/*
 * String: [#RGB], [#RGBA], [#RRGGBB], [#RRGGBBAA], [r, g, b, a], [NAME]
 * http://www.w3.org/TR/css3-color/#svg-color
 */
void color_init_string(struct color_t * c, const char * s);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_COLOR_H__ */
