#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct pwm_t
{
	struct kobj_t * kobj;
	const char * name;

	void (*enable)(struct pwm_t * pwm);
	void (*disable)(struct pwm_t * pwm);
	void (*config)(struct pwm_t * pwm, int duty_ns, int period_ns);

	void * pirv;
};

bool_t register_pwm(struct pwm_t * pwm);
bool_t unregister_pwm(struct pwm_t * pwm);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */
