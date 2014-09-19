#ifndef __LED_PWM_H__
#define __LED_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <pwm/pwm.h>
#include <led/led.h>

struct led_pwm_data_t
{
	const char * pwm;
	u32_t period;
	bool_t polarity;
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_PWM_H__ */
