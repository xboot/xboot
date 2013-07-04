#ifndef __FRAMEWORK_TIMECOUNTER_H__
#define __FRAMEWORK_TIMECOUNTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_TIMECOUNTER		"mt_name_timecounter"

int luaopen_timecounter(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_TIMECOUNTER_H__ */
