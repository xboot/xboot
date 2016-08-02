#ifndef __FRAMEWORK_L_HARDWARE_H__
#define __FRAMEWORK_L_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_HARDWARE_BATTERY		"mt_hardware_battery"
#define	MT_HARDWARE_BUZZER		"mt_hardware_buzzer"
#define	MT_HARDWARE_GPIO		"mt_hardware_gpio"
#define	MT_HARDWARE_I2C			"mt_hardware_i2c"
#define	MT_HARDWARE_LED			"mt_hardware_led"
#define	MT_HARDWARE_LEDTRIG		"mt_hardware_ledtrig"
#define	MT_HARDWARE_PWM			"mt_hardware_pwm"
#define	MT_HARDWARE_UART		"mt_hardware_uart"
#define	MT_HARDWARE_VIBRATOR	"mt_hardware_vibrator"
#define	MT_HARDWARE_WATCHDOG	"mt_hardware_watchdog"

int luaopen_hardware_battery(lua_State * L);
int luaopen_hardware_buzzer(lua_State * L);
int luaopen_hardware_gpio(lua_State * L);
int luaopen_hardware_i2c(lua_State * L);
int luaopen_hardware_led(lua_State * L);
int luaopen_hardware_ledtrig(lua_State * L);
int luaopen_hardware_pwm(lua_State * L);
int luaopen_hardware_uart(lua_State * L);
int luaopen_hardware_vibrator(lua_State * L);
int luaopen_hardware_watchdog(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_HARDWARE_H__ */
