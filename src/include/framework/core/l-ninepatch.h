#ifndef __FRAMEWORK_CORE_L_NINEPATCH_H__
#define __FRAMEWORK_CORE_L_NINEPATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <framework/luahelper.h>

#define MT_NINEPATCH	"__mt_ninepatch__"

struct lninepatch_t {
	int width, height;
	int left, top, right, bottom;
	cairo_surface_t * lt;
	cairo_surface_t * mt;
	cairo_surface_t * rt;
	cairo_surface_t * lm;
	cairo_surface_t * mm;
	cairo_surface_t * rm;
	cairo_surface_t * lb;
	cairo_surface_t * mb;
	cairo_surface_t * rb;
	double __w, __h;
	double __sx, __sy;
};

void ninepatch_stretch(struct lninepatch_t * ninepatch, double width, double height);
int luaopen_ninepatch(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_NINEPATCH_H__ */
