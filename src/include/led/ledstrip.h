#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct ledstrip_t
{
	char * name;

	void (*set_count)(struct ledstrip_t * strip, int c);
	int  (*get_count)(struct ledstrip_t * strip);
	void (*set_color)(struct ledstrip_t * strip, int i, uint32_t color);
	uint32_t (*get_color)(struct ledstrip_t * strip, int i);
	void (*refresh)(struct ledstrip_t * strip);

	void * priv;
};

struct ledstrip_t * search_ledstrip(const char * name);
bool_t register_ledstrip(struct device_t ** device, struct ledstrip_t * strip);
bool_t unregister_ledstrip(struct ledstrip_t * strip);

void ledstrip_set_count(struct ledstrip_t * strip, int c);
int ledstrip_get_count(struct ledstrip_t * strip);
void ledstrip_set_color(struct ledstrip_t * strip, int i, uint32_t color);
uint32_t ledstrip_get_color(struct ledstrip_t * strip, int i);
void ledstrip_refresh(struct ledstrip_t * strip);

#ifdef __cplusplus
}
#endif

#endif /* __LED_STRIP_H__ */
