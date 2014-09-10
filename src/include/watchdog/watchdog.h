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
	void (*set)(struct watchdog_t * watchdog, int timeout);

	/* Get watchdog's timeout in seconds, remaining time */
	int (*get)(struct watchdog_t * watchdog);

	/* Suspend watchdog */
	void (*suspend)(struct watchdog_t * watchdog);

	/* Resume watchdog */
	void (*resume)(struct watchdog_t * watchdog);

	/* Private data */
	void * priv;
};

bool_t register_watchdog(struct watchdog_t * watchdog);
bool_t unregister_watchdog(struct watchdog_t * watchdog);
struct watchdog_t * search_default_watchdog_with_name(const char * name);
void watchdog_set_timeout(struct watchdog_t * watchdog, int timeout);
int watchdog_get_timeout(struct watchdog_t * watchdog);

#ifdef __cplusplus
}
#endif

#endif /* __WATCHDOG_H__ */
