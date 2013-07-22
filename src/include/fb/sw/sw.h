#ifndef __FB_SW_H__
#define __FB_SW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/render.h>
#include <pixman.h>

pixman_format_code_t pixel_format_to_pixman_format_code(enum pixel_format_t format);

void render_sw_create_priv_data(struct render_t * render);
void render_sw_destroy_priv_data(struct render_t * render);
struct texture_t * render_sw_texture_alloc(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format);
struct texture_t * render_sw_texture_alloc_similar(struct render_t * render, u32_t w, u32_t h);
void render_sw_texture_free(struct render_t * render, struct texture_t * texture);

void render_sw_fill(struct render_t * render, struct rect_t * rect, u32_t c);
void render_sw_blit(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);
struct texture_t * render_sw_scale(struct render_t * render, struct texture_t * texture, u32_t w, u32_t h);
struct texture_t * render_sw_rotate(struct render_t * render, struct rect_t * rect, u32_t angle);

#ifdef __cplusplus
}
#endif

#endif /* __FB_SW_H__ */
