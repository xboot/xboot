#ifndef __VIBRATOR_H__
#define __VIBRATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct vibrator_t
{
	/* The vibrator name */
	char * name;

	/* Set vibrator's state */
	void (*set)(struct vibrator_t * vib, int state);

	/* Get vibrator's state */
	int (*get)(struct vibrator_t * vib);

	/* Vibrator vibrate with queue, all zero means clear and stop */
	void (*vibrate)(struct vibrator_t * vib, int state, int millisecond);

	/* Private data */
	void * priv;
};

struct vibrator_t * search_vibrator(const char * name);
struct vibrator_t * search_first_vibrator(void);
struct device_t * register_vibrator(struct vibrator_t * vib, struct driver_t * drv);
void unregister_vibrator(struct vibrator_t * vib);

void vibrator_set_state(struct vibrator_t * vib, int state);
int vibrator_get_state(struct vibrator_t * vib);
void vibrator_vibrate(struct vibrator_t * vib, int state, int millisecond);
void vibrator_play(struct vibrator_t * vib, const char * morse);

#ifdef __cplusplus
}
#endif

#endif /* __VIBRATOR_H__ */
