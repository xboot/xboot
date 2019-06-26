#ifndef __FRAMEWORK_CORE_L_NINEPATCH_H__
#define __FRAMEWORK_CORE_L_NINEPATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_NINEPATCH	"__mt_ninepatch__"

struct lninepatch_t {
	int width, height;
	int left, top, right, bottom;
	struct surface_t * lt;
	struct surface_t * mt;
	struct surface_t * rt;
	struct surface_t * lm;
	struct surface_t * mm;
	struct surface_t * rm;
	struct surface_t * lb;
	struct surface_t * mb;
	struct surface_t * rb;
	double __w, __h;
	double __sx, __sy;
};

void ninepatch_stretch(struct lninepatch_t * ninepatch, double width, double height);
int luaopen_ninepatch(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_NINEPATCH_H__ */
