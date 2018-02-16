#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct servo_t
{
	/* The servo name */
	char * name;

	/* Set servo's angle */
	void (*set)(struct servo_t * servo, int angle);

	/* Get servo's angle */
	int (*get)(struct servo_t * servo);

	/* Private data */
	void * priv;
};

struct servo_t * search_servo(const char * name);
bool_t register_servo(struct device_t ** device, struct servo_t * servo);
bool_t unregister_servo(struct servo_t * servo);

void servo_set_angle(struct servo_t * servo, int angle);
int servo_get_angle(struct servo_t * servo);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_H__ */
