#ifndef __FB_LOGO_H__
#define __FB_LOGO_H__

#include <xboot.h>
#include <fb/fb.h>
#include <graphic/surface.h>

bool_t display_logo(struct fb * fb);
bool_t register_logo(const struct gimage * logo);

#endif /* __FB_LOGO_H__ */
