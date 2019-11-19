#ifndef __GYROSCOPE_H__
#define __GYROSCOPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct gyroscope_t
{
	char * name;

	bool_t (*get)(struct gyroscope_t * g, int * x, int * y, int * z);
	void * priv;
};

struct gyroscope_t * search_gyroscope(const char * name);
struct gyroscope_t * search_first_gyroscope(void);
struct device_t * register_gyroscope(struct gyroscope_t * g, struct driver_t * drv);
void unregister_gyroscope(struct gyroscope_t * g);

bool_t gyroscope_get_palstance(struct gyroscope_t * g, int * x, int * y, int * z);

#ifdef __cplusplus
}
#endif

#endif /* __GYROSCOPE_H__ */
