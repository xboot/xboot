#ifndef __FB_CONSOLE_H__
#define __FB_CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>

bool_t register_framebuffer_console(struct fb_t * fb);
bool_t unregister_framebuffer_console(struct fb_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FB_CONSOLE_H__ */
