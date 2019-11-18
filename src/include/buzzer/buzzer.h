#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct buzzer_t
{
	/* The buzzer name */
	char * name;

	/* Set buzzer's frequency */
	void (*set)(struct buzzer_t * buzzer, int frequency);

	/* Get buzzer's frequency */
	int (*get)(struct buzzer_t * buzzer);

	/* Buzzer beep in with queue, all zero means clear and stop */
	void (*beep)(struct buzzer_t * buzzer, int frequency, int millisecond);

	/* Private data */
	void * priv;
};

struct buzzer_t * search_buzzer(const char * name);
struct buzzer_t * search_first_buzzer(void);
struct device_t * register_buzzer(struct buzzer_t * buzzer, struct driver_t * drv);
void unregister_buzzer(struct buzzer_t * buzzer);

void buzzer_set_frequency(struct buzzer_t * buzzer, int frequency);
int buzzer_get_frequency(struct buzzer_t * buzzer);
void buzzer_beep(struct buzzer_t * buzzer, int frequency, int millisecond);
void buzzer_play(struct buzzer_t * buzzer, const char * rtttl);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H__ */
