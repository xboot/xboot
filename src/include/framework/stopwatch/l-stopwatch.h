#ifndef __FRAMEWORK_L_STOPWATCH_H__
#define __FRAMEWORK_L_STOPWATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_STOPWATCH		"mt_name_stopwatch"

int luaopen_stopwatch(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_STOPWATCH_H__ */
