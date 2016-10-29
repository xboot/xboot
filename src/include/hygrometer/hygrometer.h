#ifndef __HYGROMETER_H__
#define __HYGROMETER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct hygrometer_t
{
	char * name;

	int (*get)(struct hygrometer_t * hygrometer);
	void * priv;
};

struct hygrometer_t * search_hygrometer(const char * name);
struct hygrometer_t * search_first_hygrometer(void);
bool_t register_hygrometer(struct device_t ** device,struct hygrometer_t * hygrometer);
bool_t unregister_hygrometer(struct hygrometer_t * hygrometer);

int hygrometer_get_humidity(struct hygrometer_t * hygrometer);

#ifdef __cplusplus
}
#endif

#endif /* __HYGROMETER_H__ */
