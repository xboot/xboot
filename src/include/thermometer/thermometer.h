#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct thermometer_t
{
	char * name;

	int (*get)(struct thermometer_t * thermometer);
	void * priv;
};

struct thermometer_t * search_thermometer(const char * name);
struct thermometer_t * search_first_thermometer(void);
struct device_t * register_thermometer(struct thermometer_t * thermometer, struct driver_t * drv);
void unregister_thermometer(struct thermometer_t * thermometer);

int thermometer_get_temperature(struct thermometer_t * thermometer);

#ifdef __cplusplus
}
#endif

#endif /* __THERMOMETER_H__ */
