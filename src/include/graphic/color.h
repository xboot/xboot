#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

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

static inline void color_mix(struct color_t * c, struct color_t * a, struct color_t * b, unsigned char weight)
{
	int u = (weight << 1) - 255;
	int v = idiv255(a->a - b->a);
	int w = idiv255(u * v);
	unsigned char wa = ((u * v == -255) ? u : (u + (a->a - b->a)) / (1 + w) + 255) >> 1;
	unsigned char wb = 255 - wa;

	c->r = idiv255(a->r * wa) + idiv255(b->r * wb);
	c->g = idiv255(a->g * wa) + idiv255(b->g * wb);
	c->b = idiv255(a->b * wa) + idiv255(b->b * wb);
	c->a = idiv255(a->a * weight) + idiv255(b->a * (255 - weight));
}

static inline void color_level(struct color_t * c, struct color_t * o, int level)
{
	if(level < 0)
		color_mix(c, &(struct color_t){ 0xff, 0xff, 0xff, 0xff }, o, -level * 25);
	else
		color_mix(c, &(struct color_t){ 0x00, 0x00, 0x00, 0xff }, o, level * 25);
}

/*
 * String: [#RGB], [#RGBA], [#RRGGBB], [#RRGGBBAA], [r, g, b, a], [NAME]
 * http://www.w3.org/TR/css3-color/#svg-color
 */
void color_init_string(struct color_t * c, const char * s);

/*
 * h, s, v and a from 0 to 1
 */
void color_set_hsva(struct color_t * c, float h, float s, float v, float a);
void color_get_hsva(struct color_t * c, float * h, float * s, float * v, float * a);

/*
 * Random color, s and v and a from 0 to 1
 */
void color_random(struct color_t * c, float s, float v, float a);

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
