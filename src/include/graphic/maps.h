#ifndef __MAPS_H__
#define __MAPS_H__

#include <xboot.h>
#include <graphic/rect.h>

struct surface_t;
struct surface_maps;

enum blend_mode {
	BLEND_MODE_REPLACE	= 0x0,
	BLEND_MODE_ALPHA	= 0x1,
};

struct surface_maps {
	bool_t (*point)(struct surface_t * surface, s32_t x, s32_t y, u32_t c, enum blend_mode mode);
	bool_t (*hline)(struct surface_t * surface, s32_t x0, s32_t y0, u32_t x1, u32_t c, enum blend_mode mode);
	bool_t (*vline)(struct surface_t * surface, s32_t x0, s32_t y0, u32_t y1, u32_t c, enum blend_mode mode);
	bool_t (*fill)(struct surface_t * surface, struct rect_t * rect, u32_t c, enum blend_mode mode);
	bool_t (*blit)(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode);
	struct surface_t * (*zoom)(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h);
};

bool_t surface_set_maps(struct surface_maps * maps);

#endif /* __MAPS_H__ */
