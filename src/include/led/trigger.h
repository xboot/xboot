#ifndef __LED_TRIGGER_H__
#define __LED_TRIGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <xboot/list.h>
#include <led/led.h>

/*
 * the struct of trigger.
 */
struct trigger_t
{
	/* the trigger name */
	const char * name;

	/* activate led trigger */
	void (*activate)(struct trigger_t * trigger);

	/* activate led trigger */
	void (*deactivate)(struct trigger_t * trigger);

	/* led device */
	struct led_t * led;

	/* private data */
	void * priv;
};

/*
 * the list of led trigger
 */
struct led_trigger_list
{
	struct trigger_t * trigger;
	struct list_head entry;
};

bool_t trigger_register(struct trigger_t * trigger);
bool_t trigger_unregister(struct trigger_t * trigger);

#ifdef __cplusplus
}
#endif

#endif /* __LED_TRIGGER_H__ */
