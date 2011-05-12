#ifndef __LOGO_H__
#define __LOGO_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>
#include <fb/bitmap.h>

bool_t display_logo(struct fb * fb);
bool_t register_logo(const struct picture * logo);

#endif /* __LOGO_H__ */
