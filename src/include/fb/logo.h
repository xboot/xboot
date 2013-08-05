#ifndef __FB_LOGO_H__
#define __FB_LOGO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <fb/render.h>

bool_t register_logo(const struct gimage_t * logo);
void render_show_logo(struct render_t * render);

#ifdef __cplusplus
}
#endif

#endif /* __FB_LOGO_H__ */
