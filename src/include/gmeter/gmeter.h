#ifndef __GMETER_H__
#define __GMETER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct gmeter_t
{
	char * name;

	bool_t (*get)(struct gmeter_t * g, int * x, int * y, int * z);
	void * priv;
};

struct gmeter_t * search_gmeter(const char * name);
struct gmeter_t * search_first_gmeter(void);
bool_t register_gmeter(struct device_t ** device,struct gmeter_t * g);
bool_t unregister_gmeter(struct gmeter_t * g);

bool_t gmeter_get_acceleration(struct gmeter_t * g, int * x, int * y, int * z);

#ifdef __cplusplus
}
#endif

#endif /* __GMETER_H__ */
