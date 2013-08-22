#ifndef __CONSOLE_FB_H__
#define __CONSOLE_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>

bool_t register_console_framebuffer(struct fb_t * fb);
bool_t unregister_console_framebuffer(struct fb_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_FB_H__ */
