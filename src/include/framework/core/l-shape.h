#ifndef __FRAMEWORK_CORE_L_SHAPE_H__
#define __FRAMEWORK_CORE_L_SHAPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <framework/luahelper.h>

#define MT_SHAPE	"__mt_shape__"

struct lshape_t {
	cairo_surface_t * cs;
	cairo_t * cr;
};

int luaopen_shape(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_SHAPE_H__ */
