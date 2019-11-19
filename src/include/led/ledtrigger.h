#ifndef __LED_TRIGGER_H__
#define __LED_TRIGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct ledtrigger_t
{
	/* The led trigger name */
	char * name;

	/* Activity led trigger */
	void (*activity)(struct ledtrigger_t * trigger);

	/* Private data */
	void * priv;
};

struct ledtrigger_t * search_ledtrigger(const char * name);
struct device_t * register_ledtrigger(struct ledtrigger_t * trigger, struct driver_t * drv);
void unregister_ledtrigger(struct ledtrigger_t * trigger);

void ledtrigger_activity(struct ledtrigger_t * trigger);

#ifdef __cplusplus
}
#endif

#endif /* __LED_TRIGGER_H__ */
