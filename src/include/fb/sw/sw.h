#ifndef __FB_SW_H__
#define __FB_SW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/render.h>
#include <pixman.h>

pixman_format_code_t pixel_format_to_pixman_format_code(enum pixel_format_t format);

void sw_render_create_data(struct render_t * render);
void sw_render_destroy_data(struct render_t * render);
void sw_render_clear(struct render_t * render, struct rect_t * r, struct color_t * c);
struct texture_t * sw_render_snapshot(struct render_t * render);

struct texture_t * sw_render_alloc_texture(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format);
struct texture_t * sw_render_alloc_texture_similar(struct render_t * render, u32_t w, u32_t h);
void sw_render_free_texture(struct render_t * render, struct texture_t * texture);
void sw_render_fill_texture(struct render_t * render, struct texture_t * texture, struct rect_t * r, struct color_t * c);
void sw_render_blit_texture(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);

#ifdef __cplusplus
}
#endif

#endif /* __FB_SW_H__ */
