#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>
#include <framework/event/l-event.h>
#include <framework/logger/l-logger.h>
#include <framework/stopwatch/l-stopwatch.h>
#include <framework/base64/l-base64.h>
#include <framework/cairo/l-cairo.h>
#include <framework/display/l-matrix.h>
#include <framework/display/l-texture.h>
#include <framework/hardware/l-hardware.h>

int luaopen_xboot(lua_State * L);
int luaopen_boot(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
