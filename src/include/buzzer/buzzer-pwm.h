#ifndef __BUZZER_PWM_H__
#define __BUZZER_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <pwm/pwm.h>
#include <buzzer/buzzer.h>

struct buzzer_pwm_data_t
{
	const char * pwm;
	bool_t polarity;
};

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_PWM_H__ */
