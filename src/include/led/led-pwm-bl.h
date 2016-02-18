#ifndef __LED_PWM_BL_H__
#define __LED_PWM_BL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <pwm/pwm.h>
#include <led/led.h>

struct led_pwm_bl_data_t
{
	const char * pwm;
	u32_t period;
	int from;
	int to;
	int polarity;
	int power;
	int power_active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_PWM_BL_H__ */
