#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct led_t
{
	/* The LED name */
	char * name;

	/* Initialize the LED */
	void (*init)(struct led_t * led);

	/* Clean up the LED */
	void (*exit)(struct led_t * led);

    /*
     * Set LED's color, The color of the LED in ARGB and high byte should be ignored.
     *
     * If led can only do red or green, if they ask for blue, you should do green.
     * If you can only do a brightness ramp, then use this formula:
     *     unsigned char brightness = ((77*((color>>16)&0x00ff))
     *     + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
     * If you can only do on or off, 0 is off, anything else is on.
     */
	void (*set)(struct led_t * led, u32_t color);

	/* Get LED's color */
	u32_t (*get)(struct led_t * led);

	/* Suspend LED */
	void (*suspend)(struct led_t * led);

	/* Resume LED */
	void (*resume)(struct led_t * led);

	/* Private data */
	void * priv;
};

struct led_t * search_led(const char * name);
bool_t register_led(struct led_t * led);
bool_t unregister_led(struct led_t * led);
void led_set_color(struct led_t * led, u32_t color);
u32_t led_get_color(struct led_t * led);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H__ */
