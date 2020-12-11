#ifndef __FRAMEWORK_CORE_L_IMAGE_H__
#define __FRAMEWORK_CORE_L_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define MT_IMAGE	"__mt_image__"

struct limage_t {
	struct surface_t * s;
};

int luaopen_image(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_IMAGE_H__ */
