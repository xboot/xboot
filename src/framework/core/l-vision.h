#ifndef __FRAMEWORK_CORE_L_VISION_H__
#define __FRAMEWORK_CORE_L_VISION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define MT_VISION	"__mt_vision__"

struct lvision_t {
	struct vision_t * v;
};

int luaopen_vision(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_VISION_H__ */
