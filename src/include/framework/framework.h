#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>
#include <framework/base64/l_base64.h>
#include <framework/cairo/l_cairo.h>
#include <framework/event/l_event.h>
#include <framework/framerate/l_framerate.h>

int luaopen_boot(lua_State * L);
int luaopen_xboot(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
