#ifndef __LED_TRIGGER_H__
#define __LED_TRIGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct ledtrig_data_t
{
	const char * led;
};

struct ledtrig_t
{
	/* The led trigger name */
	char * name;

	/* Activity led trigger */
	void (*activity)(struct ledtrig_t * trigger);

	/* Private data */
	void * priv;
};

struct ledtrig_t * search_ledtrig(const char * name);
bool_t register_ledtrig(struct device_t ** device, struct ledtrig_t * trigger);
bool_t unregister_ledtrig(struct ledtrig_t * trigger);

void ledtrig_activity(struct ledtrig_t * trigger);

#ifdef __cplusplus
}
#endif

#endif /* __LED_TRIGGER_H__ */
