#ifndef __FRAMEWORK_CORE_L_EASING_H__
#define __FRAMEWORK_CORE_L_EASING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define	MT_EASING	"__mt_easing__"

int luaopen_easing(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_EASING_H__ */
