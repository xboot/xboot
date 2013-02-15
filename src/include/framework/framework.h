#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>
#include <framework/cairo/l_cairo.h>

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
