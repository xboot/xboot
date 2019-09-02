#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

struct color_t {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

static inline void color_init(struct color_t * c, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
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

/*
 * h from 0 - 360, s and v from 0 - 100
 */
void color_set_hsv(struct color_t * c, int h, int s, int v);
void color_get_hsv(struct color_t * c, int * h, int * s, int * v);

/*
 * The value with pre-multiplied alpha
 */
static inline void color_set_premult(struct color_t * c, uint32_t v)
{
	unsigned char a = (v >> 24) & 0xff;

	if(a != 0)
	{
		if(a == 255)
		{
			c->r = (v >> 16) & 0xff;
			c->g = (v >> 8) & 0xff;
			c->b = (v >> 0) & 0xff;
			c->a = a;
		}
		else
		{
			c->r = c->r * 255 / a;
			c->g = c->g * 255 / a;
			c->b = c->b * 255 / a;
			c->a = a;
		}
	}
	else
	{
		c->r = 0;
		c->g = 0;
		c->b = 0;
		c->a = 0;
	}
}

static inline uint32_t color_get_premult(struct color_t * c)
{
	unsigned char a = c->a;

	if(a != 0)
	{
		if(a == 255)
			return (a << 24) | (c->r << 16) | (c->g << 8) | (c->b << 0);
		return (a << 24) | (idiv255(c->r * a) << 16) | (idiv255(c->g * a) << 8) | (idiv255(c->b * a) << 0);
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_COLOR_H__ */
