#ifndef __FBVIEWPORT_H__
#define __FBVIEWPORT_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <fb/fb.h>

bool_t bitmap_set_viewport(struct bitmap * bitmap, struct rect * rect);
bool_t bitmap_get_viewport(struct bitmap * bitmap, struct rect * rect);

#endif /* __FBVIEWPORT_H__ */
