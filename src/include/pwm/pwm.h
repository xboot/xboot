#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct pwm_t
{
	char * name;

	void (*config)(struct pwm_t * pwm, int duty, int period, int polarity);
	void (*enable)(struct pwm_t * pwm);
	void (*disable)(struct pwm_t * pwm);
	void * priv;
};

struct pwm_t * search_pwm(const char * name);
struct device_t * register_pwm(struct pwm_t * pwm, struct driver_t * drv);
void unregister_pwm(struct pwm_t * pwm);

void pwm_config(struct pwm_t * pwm, int duty, int period, int polarity);
void pwm_enable(struct pwm_t * pwm);
void pwm_disable(struct pwm_t * pwm);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */
