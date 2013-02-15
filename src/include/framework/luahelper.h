#ifndef __FRAMEWORK_LUAHELPER_H__
#define __FRAMEWORK_LUAHELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

const char * luahelper_get_strfield(lua_State * L, const char * key, const char * def);
lua_Number luahelper_get_numfield(lua_State * L, const char * key, lua_Number def);
lua_Integer luahelper_get_intfield(lua_State * L, const char * key, lua_Integer def);
lua_Unsigned luahelper_get_uintfield(lua_State * L, const char * key, lua_Unsigned def);

void luahelper_set_strfield(lua_State * L, const char * key, const char * value);
void luahelper_set_numfield(lua_State * L, const char * key, lua_Number value);
void luahelper_set_intfield(lua_State * L, const char * key, lua_Integer value);
void luahelper_set_uintfield(lua_State * L, const char * key, lua_Unsigned value);

void luahelper_create_metatable(lua_State * L, const char * name, const luaL_Reg * funcs);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_LUAHELPER_H__ */
