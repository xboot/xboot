#ifndef __GRAPHIC_SOFTWARE_H__
#define __GRAPHIC_SOFTWARE_H__

#include <xboot.h>
#include <graphic/surface.h>


inline void surface_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c);
inline u32_t surface_get_pixel(struct surface_t * surface, s32_t x, s32_t y);

bool_t software_draw_points(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c);;
bool_t software_draw_lines(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c);
bool_t software_fill_rects(struct surface_t * surface, const struct rect_t * rects, u32_t count, u32_t c);
bool_t software_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blit_mode mode);

#endif /* __GRAPHIC_SOFTWARE_H__ */
