#ifndef __FRAMEWORK_L_JSON_H__
#define __FRAMEWORK_L_JSON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

extern int luaopen_cjson_safe(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_JSON_H__ */
