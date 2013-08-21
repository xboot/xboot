#ifndef __LED_TRIGGER_H__
#define __LED_TRIGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct led_trigger_data_t
{
	const char * name;
	const char * led;
};

struct led_trigger_t
{
	/* The led trigger name */
	char * name;

	/* Initial led trigger */
	void (*init)(struct led_trigger_t * trigger);

	/* Clean up led trigger */
	void (*exit)(struct led_trigger_t * trigger);

	/* Activity led trigger */
	void (*activity)(struct led_trigger_t * trigger);

	/* Bind to led device */
	struct led_t * led;

	/* Private data */
	void * priv;
};

struct led_trigger_t * search_led_trigger(const char * name);
bool_t register_led_trigger(struct led_trigger_t * trigger);
bool_t unregister_led_trigger(struct led_trigger_t * trigger);
void led_trigger_activity(struct led_trigger_t * trigger);

#ifdef __cplusplus
}
#endif

#endif /* __LED_TRIGGER_H__ */
