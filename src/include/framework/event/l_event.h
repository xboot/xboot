#ifndef __FRAMEWORK_L_EVENT_H__
#define __FRAMEWORK_L_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_EVENT				"mt_name_event"
#define	MT_NAME_EVENT_LISTENER		"mt_name_event_listener"

int luaopen_event(lua_State * L);
int luaopen_event_listener(lua_State * L);
int luaopen_event_dispatcher(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_EVENT_H__ */
