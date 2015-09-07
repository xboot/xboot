#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>
#include <framework/lang/l-class.h>
#include <framework/event/l-event.h>
#include <framework/stopwatch/l-stopwatch.h>
#include <framework/base64/l-base64.h>
#include <framework/display/l-display.h>
#include <framework/hardware/l-hardware.h>

int luaopen_xboot(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
