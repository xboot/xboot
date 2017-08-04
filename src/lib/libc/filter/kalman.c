/*
 * libc/filter/filter.c
 */

#include <kalman.h>

struct kalman_filter_t * kalman_alloc(float a, float h, float q, float r)
{
	struct kalman_filter_t * filter;

	filter = malloc(sizeof(struct kalman_filter_t));
	if(!filter)
		return NULL;

	filter->a = a;
	filter->h = h;
	filter->q = q;
	filter->r = r;
	filter->x = NAN;
	filter->p = q;
	filter->k = 1;
	filter->a2 = a * a;
	filter->h2 = h * h;

	return filter;
}

void kalman_free(struct kalman_filter_t * filter)
{
	if(filter)
		free(filter);
}

float kalman_update(struct kalman_filter_t * filter, float value)
{
	if(isnan(filter->x))
		filter->x = value;
	filter->x = filter->a * filter->x;
	filter->p = filter->a2 * filter->p + filter->q;
	filter->k = filter->p * filter->h / (filter->h2 * filter->p + filter->r);
	filter->x = filter->x + filter->k * (value - filter->h * filter->x);
	filter->p = (1 - filter->k * filter->h) * filter->p;

	return filter->h * filter->x;
}

void kalman_clear(struct kalman_filter_t * filter)
{
	if(filter)
		filter->x = NAN;
}
