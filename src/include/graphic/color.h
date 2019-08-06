#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct color_t {
	unsigned char r;
	unsigned char g;
	unsigned char b;
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
 * The color value pre-multiplied alpha is used
 */
static inline void color_set_pma(struct color_t * c, uint32_t v)
{
	unsigned char a = (v >> 24) & 0xff;

	if(a == 0)
	{
		c->r = 0;
		c->g = 0;
		c->b = 0;
		c->a = 0;
	}
	else if(a == 255)
	{
		c->r = (v >> 16) & 0xff;
		c->g = (v >> 8) & 0xff;
		c->b = (v >> 0) & 0xff;
		c->a = 255;
	}
	else
	{
		c->r = ((v >> 16) & 0xff) * 255 / a;
		c->g = ((v >> 8) & 0xff) * 255 / a;
		c->b = ((v >> 0) & 0xff) * 255 / a;
		c->a = a;
	}
}

static inline void color_get_pma(struct color_t * c, uint32_t * v)
{
	unsigned char a = c->a;

	if(a == 0)
		*v = 0;
	else if(a == 255)
		*v = (255 << 24) | (c->r << 16) | (c->g << 8) | (c->b << 0);
	else
		*v = (a << 24) | ((c->r * a / 255) << 16) | ((c->g * a / 255) << 8) | ((c->b * a / 255) << 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_COLOR_H__ */
