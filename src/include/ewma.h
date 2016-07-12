#ifndef __EWMA_H__
#define __EWMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

struct ewma_filter_t {
	int factor;
	int weight;
	int internal;
};

void ewma_init(struct ewma_filter_t * filter, int factor, int weight);
int ewma_filter(struct ewma_filter_t * filter, int value);

#ifdef __cplusplus
}
#endif

#endif /* __EWMA_H__ */
