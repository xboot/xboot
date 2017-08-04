#ifndef __MEAN_H__
#define __MEAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <math.h>
#include <malloc.h>

struct mean_filter_t {
	float * buffer;
	int length;
	int index;
	int count;
	float sum;
};

struct mean_filter_t * mean_alloc(int length);
void mean_free(struct mean_filter_t * filter);
float mean_update(struct mean_filter_t * filter, float value);
void mean_clear(struct mean_filter_t * filter);

#ifdef __cplusplus
}
#endif

#endif /* __MEAN_H__ */
