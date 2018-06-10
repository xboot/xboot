#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct watchdog_t
{
	/* The watchdog name */
	char * name;

	/* Set watchdog's timeout in seconds, zero means stop */
	void (*set)(struct watchdog_t * wdg, int timeout);

	/* Get watchdog's timeout in seconds, remaining time */
	int (*get)(struct watchdog_t * wdg);

	/* Private data */
	void * priv;
};

struct watchdog_t * search_watchdog(const char * name);
struct watchdog_t * search_first_watchdog(void);
bool_t register_watchdog(struct device_t ** device,struct watchdog_t * wdg);
bool_t unregister_watchdog(struct watchdog_t * wdg);

void watchdog_set_timeout(struct watchdog_t * wdg, int timeout);
int watchdog_get_timeout(struct watchdog_t * wdg);

#ifdef __cplusplus
}
#endif

#endif /* __WATCHDOG_H__ */
