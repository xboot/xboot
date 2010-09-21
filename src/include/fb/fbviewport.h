#ifndef __FBVIEWPORT_H__
#define __FBVIEWPORT_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

x_bool bitmap_set_viewport(struct bitmap * bitmap, struct rect * rect);
x_bool bitmap_get_viewport(struct bitmap * bitmap, struct rect * rect);

#endif /* __FBVIEWPORT_H__ */
