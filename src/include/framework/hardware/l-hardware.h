#ifndef __FRAMEWORK_L_HARDWARE_H__
#define __FRAMEWORK_L_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_HARDWARE_ADC			"mt_hardware_adc"
#define	MT_HARDWARE_BATTERY		"mt_hardware_battery"
#define	MT_HARDWARE_BUZZER		"mt_hardware_buzzer"
#define	MT_HARDWARE_COMPASS		"mt_hardware_compass"
#define	MT_HARDWARE_DAC			"mt_hardware_dac"
#define	MT_HARDWARE_GMETER		"mt_hardware_gmeter"
#define	MT_HARDWARE_GPIO		"mt_hardware_gpio"
#define	MT_HARDWARE_GYROSCOPE	"mt_hardware_gyroscope"
#define	MT_HARDWARE_HYGROMETER	"mt_hardware_hygrometer"
#define	MT_HARDWARE_I2C			"mt_hardware_i2c"
#define	MT_HARDWARE_LED			"mt_hardware_led"
#define	MT_HARDWARE_LEDSTRIP	"mt_hardware_ledstrip"
#define	MT_HARDWARE_LEDTRIGGER	"mt_hardware_ledtrigger"
#define	MT_HARDWARE_LIGHT		"mt_hardware_light"
#define	MT_HARDWARE_MOTOR		"mt_hardware_motor"
#define	MT_HARDWARE_NVMEM		"mt_hardware_nvmem"
#define	MT_HARDWARE_PRESSURE	"mt_hardware_pressure"
#define	MT_HARDWARE_PROXIMITY	"mt_hardware_proximity"
#define	MT_HARDWARE_PWM			"mt_hardware_pwm"
#define	MT_HARDWARE_SERVO		"mt_hardware_servo"
#define	MT_HARDWARE_SPI			"mt_hardware_spi"
#define	MT_HARDWARE_STEPPER		"mt_hardware_stepper"
#define	MT_HARDWARE_THERMOMETER	"mt_hardware_thermometer"
#define	MT_HARDWARE_UART		"mt_hardware_uart"
#define	MT_HARDWARE_VIBRATOR	"mt_hardware_vibrator"
#define	MT_HARDWARE_WATCHDOG	"mt_hardware_watchdog"

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

#endif /* __FRAMEWORK_L_HARDWARE_H__ */
