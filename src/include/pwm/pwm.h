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
	u32_t __duty;
	u32_t __period;
	bool_t __enable;

	void (*enable)(struct pwm_t * pwm);
	void (*disable)(struct pwm_t * pwm);
	void (*config)(struct pwm_t * pwm, u32_t duty, u32_t period);

	void * priv;
};

struct pwm_list_t
{
	struct pwm_t * pwm;
	struct list_head entry;
};

extern struct pwm_list_t __pwm_list;

struct pwm_t * search_pwm(const char * name);
bool_t register_pwm(struct pwm_t * pwm);
bool_t unregister_pwm(struct pwm_t * pwm);
void pwm_enable(struct pwm_t * pwm);
void pwm_disable(struct pwm_t * pwm);
void pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */
