#ifndef __COLOR_H__
#define __COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct color_t {
	u8_t r;
	u8_t g;
	u8_t b;
	u8_t a;
};

struct color_t * get_named_color(const char * name);
void color_init_rgb(struct color_t * color, u8_t r, u8_t g, u8_t b);
void color_init_rgba(struct color_t * color, u8_t r, u8_t g, u8_t b, u8_t a);
void color_init_colstr(struct color_t * color, const char * colstr);

#ifdef __cplusplus
}
#endif

#endif /* __COLOR_H__ */
