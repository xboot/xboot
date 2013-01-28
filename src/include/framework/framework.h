#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/*
 * Lua helper functions
 */
const char * lua_helper_get_strfield(lua_State * L, const char * key, const char * def);
lua_Number lua_helper_get_numfield(lua_State * L, const char * key, lua_Number def);
lua_Integer lua_helper_get_intfield(lua_State * L, const char * key, lua_Integer def);
lua_Unsigned lua_helper_get_uintfield(lua_State * L, const char * key, lua_Unsigned def);

void lua_helper_set_strfield(lua_State * L, const char * key, const char * value);
void lua_helper_set_numfield(lua_State * L, const char * key, lua_Number value);
void lua_helper_set_intfield(lua_State * L, const char * key, lua_Integer value);
void lua_helper_set_uintfield(lua_State * L, const char * key, lua_Unsigned value);


/* Event */
#define	LUA_TYPE_EVENT				"event"
#define	LUA_TYPE_EVENT_LISTENER		"event_listener"

int luaopen_event(lua_State * L);
int luaopen_event_listener(lua_State * L);
int luaopen_event_dispatcher(lua_State * L);

/* Base64 */
int luaopen_base64(lua_State * L);

/* Framerate */
int luaopen_framerate(lua_State * L);

/* Xboot */
int luaopen_boot(lua_State * L);
int luaopen_xboot(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
