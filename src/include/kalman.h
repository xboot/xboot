#ifndef __KALMAN_H__
#define __KALMAN_H__

#ifdef __cplusplus
extern "C" {
#endif

struct kalman_filter_t {
	float a, h;
	float q, r;
	float x, p;
	float k, a2, h2;
};

struct kalman_filter_t * kalman_alloc(float a, float h, float q, float r);
void kalman_free(struct kalman_filter_t * filter);
float kalman_update(struct kalman_filter_t * filter, float value);
void kalman_clear(struct kalman_filter_t * filter);

#ifdef __cplusplus
}
#endif

#endif /* __KALMAN_H__ */
