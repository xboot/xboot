#ifndef __FRAMEWORK_CORE_L_IMAGE_H__
#define __FRAMEWORK_CORE_L_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <framework/luahelper.h>

#define MT_IMAGE	"__mt_image__"

struct limage_t {
	cairo_surface_t * cs;
};

int luaopen_image(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_IMAGE_H__ */
