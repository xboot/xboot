#ifndef __RESET_H__
#define __RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct resetchip_t
{
	char * name;
	int base;
	int nreset;

	void (*assert)(struct resetchip_t * chip, int offset);
	void (*deassert)(struct resetchip_t * chip, int offset);

	void * priv;
};

struct resetchip_t * search_resetchip(int rst);
bool_t register_resetchip(struct device_t ** device, struct resetchip_t * chip);
bool_t unregister_resetchip(struct resetchip_t * chip);

int reset_is_valid(int rst);
void reset_assert(int rst);
void reset_deassert(int rst);

#ifdef __cplusplus
}
#endif

#endif /* __RESET_H__ */
