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
	bool_t (*draw_points)(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c, enum blend_mode mode);
	bool_t (*draw_lines)(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c, enum blend_mode mode);
	bool_t (*fill_rects)(struct surface_t * surface, const struct rect_t * rects, u32_t count, u32_t c, enum blend_mode mode);
	bool_t (*blit)(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode);
};

bool_t set_surface_maps(struct surface_maps * maps);

#endif /* __MAPS_H__ */
