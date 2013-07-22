#ifndef __FB_SW_H__
#define __FB_SW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/render.h>

struct texture_t * render_sw_alloc(struct render_t * render, u32_t w, u32_t h);
void render_sw_free(struct texture_t * texture);
void render_sw_fill(struct render_t * render, struct rect_t * rect, u32_t c);
void render_sw_blit(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);
struct texture_t * render_sw_scale(struct render_t * render, struct texture_t * texture, u32_t w, u32_t h);
struct texture_t * render_sw_rotate(struct render_t * render, struct rect_t * rect, u32_t angle);

/*
u8_t * surface_sw_get_pointer(struct surface_t * surface, s32_t x, s32_t y);
u32_t surface_sw_get_pixel(struct surface_t * surface, s32_t x, s32_t y);
void surface_sw_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c);
void surface_sw_set_pixel_with_alpha(struct surface_t * surface, s32_t x, s32_t y, u32_t c);

bool_t map_software_point(struct surface_t * surface, s32_t x, s32_t y, u32_t c, enum blend_mode_t mode);
bool_t map_software_hline(struct surface_t * surface, s32_t x0, s32_t y0, u32_t x1, u32_t c, enum blend_mode_t mode);
bool_t map_software_vline(struct surface_t * surface, s32_t x0, s32_t y0, u32_t y1, u32_t c, enum blend_mode_t mode);
bool_t map_software_fill(struct surface_t * surface, struct rect_t * rect, u32_t c, enum blend_mode_t mode);
bool_t map_software_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode_t mode);
struct surface_t * map_software_scale(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h);
struct surface_t * map_software_rotate(struct surface_t * surface, struct rect_t * rect, enum rotate_type_t type);
struct surface_t * map_software_transform(struct surface_t * surface);
*/

#ifdef __cplusplus
}
#endif

#endif /* __FB_SW_H__ */
