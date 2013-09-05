#ifndef __FRAMEWORK_L_HARDWARE_H__
#define __FRAMEWORK_L_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_HARDWARE_LED	"mt_name_hardware_led"

int luaopen_hardware_led(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_HARDWARE_H__ */
