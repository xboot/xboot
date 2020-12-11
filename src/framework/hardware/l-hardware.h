#ifndef __FRAMEWORK_HARDWARE_L_HARDWARE_H__
#define __FRAMEWORK_HARDWARE_L_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define	MT_HARDWARE_ADC			"__mt_hardware_adc__"
#define	MT_HARDWARE_BATTERY		"__mt_hardware_battery__"
#define	MT_HARDWARE_BUZZER		"__mt_hardware_buzzer__"
#define	MT_HARDWARE_COMPASS		"__mt_hardware_compass__"
#define	MT_HARDWARE_DAC			"__mt_hardware_dac__"
#define	MT_HARDWARE_GMETER		"__mt_hardware_gmeter__"
#define	MT_HARDWARE_GPIO		"__mt_hardware_gpio__"
#define	MT_HARDWARE_GYROSCOPE	"__mt_hardware_gyroscope__"
#define	MT_HARDWARE_HYGROMETER	"__mt_hardware_hygrometer__"
#define	MT_HARDWARE_I2C			"__mt_hardware_i2c__"
#define	MT_HARDWARE_LED			"__mt_hardware_led__"
#define	MT_HARDWARE_LEDSTRIP	"__mt_hardware_ledstrip__"
#define	MT_HARDWARE_LEDTRIGGER	"__mt_hardware_ledtrigger__"
#define	MT_HARDWARE_LIGHT		"__mt_hardware_light__"
#define	MT_HARDWARE_MOTOR		"__mt_hardware_motor__"
#define	MT_HARDWARE_NVMEM		"__mt_hardware_nvmem__"
#define	MT_HARDWARE_PRESSURE	"__mt_hardware_pressure__"
#define	MT_HARDWARE_PROXIMITY	"__mt_hardware_proximity__"
#define	MT_HARDWARE_PWM			"__mt_hardware_pwm__"
#define	MT_HARDWARE_SERVO		"__mt_hardware_servo__"
#define	MT_HARDWARE_SPI			"__mt_hardware_spi__"
#define	MT_HARDWARE_STEPPER		"__mt_hardware_stepper__"
#define	MT_HARDWARE_THERMOMETER	"__mt_hardware_thermometer__"
#define	MT_HARDWARE_UART		"__mt_hardware_uart__"
#define	MT_HARDWARE_VIBRATOR	"__mt_hardware_vibrator__"
#define	MT_HARDWARE_WATCHDOG	"__mt_hardware_watchdog__"

int luaopen_hardware_adc(lua_State * L);
int luaopen_hardware_battery(lua_State * L);
int luaopen_hardware_buzzer(lua_State * L);
int luaopen_hardware_compass(lua_State * L);
int luaopen_hardware_dac(lua_State * L);
int luaopen_hardware_gmeter(lua_State * L);
int luaopen_hardware_gpio(lua_State * L);
int luaopen_hardware_gyroscope(lua_State * L);
int luaopen_hardware_hygrometer(lua_State * L);
int luaopen_hardware_i2c(lua_State * L);
int luaopen_hardware_led(lua_State * L);
int luaopen_hardware_ledstrip(lua_State * L);
int luaopen_hardware_ledtrigger(lua_State * L);
int luaopen_hardware_light(lua_State * L);
int luaopen_hardware_motor(lua_State * L);
int luaopen_hardware_nvmem(lua_State * L);
int luaopen_hardware_pressure(lua_State * L);
int luaopen_hardware_proximity(lua_State * L);
int luaopen_hardware_pwm(lua_State * L);
int luaopen_hardware_servo(lua_State * L);
int luaopen_hardware_spi(lua_State * L);
int luaopen_hardware_stepper(lua_State * L);
int luaopen_hardware_thermometer(lua_State * L);
int luaopen_hardware_uart(lua_State * L);
int luaopen_hardware_vibrator(lua_State * L);
int luaopen_hardware_watchdog(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_HARDWARE_L_HARDWARE_H__ */
