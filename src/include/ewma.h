#ifndef __EWMA_H__
#define __EWMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <math.h>

/*
 * Exponentially weighted moving average (EWMA)
 */
struct ewma_filter_t {
	float weight;
	float last;
};

void ewma_init(struct ewma_filter_t * filter, float weight);
float ewma_update(struct ewma_filter_t * filter, float value);

#ifdef __cplusplus
}
#endif

#endif /* __EWMA_H__ */
