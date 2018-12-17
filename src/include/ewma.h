#ifndef __EWMA_H__
#define __EWMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exponentially weighted moving average (EWMA)
 */
struct ewma_filter_t {
	float weight;
	float last;
};

struct ewma_filter_t * ewma_alloc(float weight);
void ewma_free(struct ewma_filter_t * filter);
float ewma_update(struct ewma_filter_t * filter, float value);
void ewma_clear(struct ewma_filter_t * filter);

#ifdef __cplusplus
}
#endif

#endif /* __EWMA_H__ */
