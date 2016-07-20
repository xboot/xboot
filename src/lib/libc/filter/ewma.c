/*
 * libc/filter/ewma.c
 */

#include <ewma.h>

void ewma_init(struct ewma_filter_t * filter, float weight)
{
	filter->weight = weight;
	filter->last = NAN;
}

float ewma_update(struct ewma_filter_t * filter, float value)
{
	if(isnan(filter->last))
		filter->last = value;
	else
		filter->last = filter->weight * value + (1 - filter->weight) * filter->last;
	return filter->last;
}
