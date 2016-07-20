#ifndef __KALMAN_H__
#define __KALMAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <math.h>

struct kalman_filter_t {
	float a, h;
	float q, r;
	float x, p;
	float k, a2, h2;
};

void kalman_init(struct kalman_filter_t * filter, float a, float h, float q, float r);
float kalman_update(struct kalman_filter_t * filter, float value);

#ifdef __cplusplus
}
#endif

#endif /* __KALMAN_H__ */
