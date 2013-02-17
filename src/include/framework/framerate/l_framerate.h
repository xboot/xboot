#ifndef __FRAMEWORK_FRAMERATE_H__
#define __FRAMEWORK_FRAMERATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_FRAMERATE			"mt_name_framerate"

int luaopen_framerate(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_FRAMERATE_H__ */
