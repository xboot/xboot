#ifndef __STEPPER_H__
#define __STEPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct stepper_t
{
	char * name;

	void (*enable)(struct stepper_t * m);
	void (*disable)(struct stepper_t * m);
	void (*move)(struct stepper_t * m, int step, int speed);
	int (*busying)(struct stepper_t * m);

	void * priv;
};

struct stepper_t * search_stepper(const char * name);
struct device_t * register_stepper(struct stepper_t * m, struct driver_t * drv);
void unregister_stepper(struct stepper_t * m);

void stepper_enable(struct stepper_t * m);
void stepper_disable(struct stepper_t * m);
void stepper_move(struct stepper_t * m, int step, int speed);
int stepper_busying(struct stepper_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_H__ */
