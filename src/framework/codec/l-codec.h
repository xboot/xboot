#ifndef __FRAMEWORK_CODEC_L_CODEC_H__
#define __FRAMEWORK_CODEC_L_CODEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

int luaopen_base64(lua_State * L);
int luaopen_cjson_safe(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CODEC_L_CODEC_H__ */
