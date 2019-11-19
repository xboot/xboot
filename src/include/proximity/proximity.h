#ifndef __PROXIMITY_H__
#define __PROXIMITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct proximity_t
{
	char * name;
	int (*get)(struct proximity_t * p);
	void * priv;
};

struct proximity_t * search_proximity(const char * name);
struct proximity_t * search_first_proximity(void);
struct device_t * register_proximity(struct proximity_t * p, struct driver_t * drv);
void unregister_proximity(struct proximity_t * p);

int proximity_get_distance(struct proximity_t * p);

#ifdef __cplusplus
}
#endif

#endif /* __PROXIMITY_H__ */
