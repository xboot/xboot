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
struct device_t * register_resetchip(struct resetchip_t * chip, struct driver_t * drv);
void unregister_resetchip(struct resetchip_t * chip);

int reset_is_valid(int rst);
void reset_assert(int rst);
void reset_deassert(int rst);
void reset_reset(int rst, int us);

struct resets_t {
	int n;
	int rst[0];
};

struct resets_t * resets_alloc(struct dtnode_t * n, const char * name);
void resets_free(struct resets_t * rsts);
void resets_assert(struct resets_t * rsts);
void resets_deassert(struct resets_t * rsts);
void resets_reset(struct resets_t * rsts, int us);

#ifdef __cplusplus
}
#endif

#endif /* __RESET_H__ */
