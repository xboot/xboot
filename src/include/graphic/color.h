#ifndef __GRAPHIC_COLOR_H__
#define __GRAPHIC_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_COLOR_H__ */
