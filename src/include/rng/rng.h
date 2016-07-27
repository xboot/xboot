#ifndef __RNG_H__
#define __RNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct rng_t
{
	/* The rng name */
	char * name;

	/* Read rng data */
	int (*read)(struct rng_t * rng, void * buf, int max, int wait);

	/* Private data */
	void * priv;
};

struct rng_t * search_rng(const char * name);
struct rng_t * search_first_rng(void);
bool_t register_rng(struct device_t ** device, struct rng_t * rng);
bool_t unregister_rng(struct rng_t * rng);
int rng_read_data(struct rng_t * rng, void * buf, int max, int wait);

#ifdef __cplusplus
}
#endif

#endif /* __RNG_H__ */
