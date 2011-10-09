#ifndef __GRAPHIC_SOFTWARE_H__
#define __GRAPHIC_SOFTWARE_H__

#include <xboot.h>
#include <graphic/surface.h>

inline u8_t * surface_sw_get_pointer(struct surface_t * surface, s32_t x, s32_t y);
inline u32_t surface_sw_get_pixel(struct surface_t * surface, s32_t x, s32_t y);
inline void surface_sw_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c);
inline void surface_sw_set_pixel_with_alpha(struct surface_t * surface, s32_t x, s32_t y, u32_t c);

bool_t map_software_point(struct surface_t * surface, s32_t x, s32_t y, u32_t c, enum blend_mode mode);
bool_t map_software_hline(struct surface_t * surface, s32_t x0, s32_t y0, u32_t x1, u32_t c, enum blend_mode mode);
bool_t map_software_vline(struct surface_t * surface, s32_t x0, s32_t y0, u32_t y1, u32_t c, enum blend_mode mode);
bool_t map_software_fill(struct surface_t * surface, const struct rect_t * rect, u32_t c, enum blend_mode mode);
bool_t map_software_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode);

#endif /* __GRAPHIC_SOFTWARE_H__ */
