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
bool_t register_ledtrigger(struct device_t ** device, struct ledtrigger_t * trigger);
bool_t unregister_ledtrigger(struct ledtrigger_t * trigger);

void ledtrigger_activity(struct ledtrigger_t * trigger);

#ifdef __cplusplus
}
#endif

#endif /* __LED_TRIGGER_H__ */
