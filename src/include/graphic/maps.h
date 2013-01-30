#ifndef __GRAPHIC_MAPS_H__
#define __GRAPHIC_MAPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct surface_t;
struct surface_maps;

enum blend_mode {
	BLEND_MODE_REPLACE	= 0x0,
	BLEND_MODE_ALPHA	= 0x1,
};

enum rotate_type {
	/* rotate 0 degrees */
    ROTATE_DEGREE_0		= 0x0,

    /* rotate 90 degrees */
    ROTATE_DEGREE_90	= 0x1,

    /* rotate 180 degrees */
    ROTATE_DEGREE_180	= 0x2,

    /* rotate 270 degrees */
    ROTATE_DEGREE_270	= 0x3,

    /* horizontally mirror */
    ROTATE_MIRROR_H 	= 0x4,

    /* vertically mirror */
    ROTATE_MIRROR_V 	= 0x5,
};

struct surface_maps {
	bool_t (*point)(struct surface_t * surface, s32_t x, s32_t y, u32_t c, enum blend_mode mode);
	bool_t (*hline)(struct surface_t * surface, s32_t x0, s32_t y0, u32_t x1, u32_t c, enum blend_mode mode);
	bool_t (*vline)(struct surface_t * surface, s32_t x0, s32_t y0, u32_t y1, u32_t c, enum blend_mode mode);
	bool_t (*fill)(struct surface_t * surface, struct rect_t * rect, u32_t c, enum blend_mode mode);
	bool_t (*blit)(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode);
	struct surface_t * (*scale)(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h);
	struct surface_t * (*rotate)(struct surface_t * surface, struct rect_t * rect, enum rotate_type type);
	struct surface_t * (*transform)(struct surface_t * surface);
};

bool_t surface_set_maps(struct surface_maps * maps);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_MAPS_H__ */
