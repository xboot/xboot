#ifndef __FRAMEWORK_CORE_L_SPRING_H__
#define __FRAMEWORK_CORE_L_SPRING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define	MT_SPRING	"__mt_spring__"

int luaopen_spring(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_SPRING_H__ */
