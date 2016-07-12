/*
 * libc/filter/ewma.c
 */

#include <sha1.h>

/*
 * Exponentially weighted moving average (EWMA)
 */
struct ewma_filter_t {
	int factor;
	int weight;
	int internal;
};

void ewma_init(struct ewma_filter_t * filter, int factor, int weight)
{
	filter->factor = factor;
	filter->weight = weight;
	filter->internal = 0;
}

int ewma_filter(struct ewma_filter_t * filter, int value)
{
	filter->internal = filter->internal ? (((filter->internal * (filter->weight - 1)) + (value * filter->factor)) / filter->weight) : (value * filter->factor);
	return filter->internal / filter->factor;
}
