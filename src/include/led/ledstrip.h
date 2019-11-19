#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct ledstrip_t
{
	char * name;

	void (*set_count)(struct ledstrip_t * strip, int n);
	int (*get_count)(struct ledstrip_t * strip);
	void (*set_color)(struct ledstrip_t * strip, int i, struct color_t * c);
	void (*get_color)(struct ledstrip_t * strip, int i, struct color_t * c);
	void (*refresh)(struct ledstrip_t * strip);

	void * priv;
};

struct ledstrip_t * search_ledstrip(const char * name);
struct device_t * register_ledstrip(struct ledstrip_t * strip, struct driver_t * drv);
void unregister_ledstrip(struct ledstrip_t * strip);

void ledstrip_set_count(struct ledstrip_t * strip, int n);
int ledstrip_get_count(struct ledstrip_t * strip);
void ledstrip_set_color(struct ledstrip_t * strip, int i, struct color_t * c);
void ledstrip_get_color(struct ledstrip_t * strip, int i, struct color_t * c);
void ledstrip_refresh(struct ledstrip_t * strip);

#ifdef __cplusplus
}
#endif

#endif /* __LED_STRIP_H__ */
