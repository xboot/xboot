#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct motor_t
{
	char * name;

	void (*enable)(struct motor_t * m);
	void (*disable)(struct motor_t * m);
	void (*set)(struct motor_t * m, int speed);
	void * priv;
};

struct motor_t * search_motor(const char * name);
bool_t register_motor(struct device_t ** device, struct motor_t * m);
bool_t unregister_motor(struct motor_t * m);

void motor_enable(struct motor_t * m);
void motor_disable(struct motor_t * m);
void motor_set_speed(struct motor_t * m, int speed);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */
