#ifndef __PRESSURE_H__
#define __PRESSURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct pressure_t
{
	char * name;
	int (*get)(struct pressure_t * p);
	void * priv;
};

struct pressure_t * search_pressure(const char * name);
struct pressure_t * search_first_pressure(void);
struct device_t * register_pressure(struct pressure_t * p, struct driver_t * drv);
void unregister_pressure(struct pressure_t * p);

int pressure_get_pascal(struct pressure_t * p);

#ifdef __cplusplus
}
#endif

#endif /* __PRESSURE_H__ */
