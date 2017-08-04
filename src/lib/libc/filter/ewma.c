/*
 * libc/filter/ewma.c
 */

#include <ewma.h>

struct ewma_filter_t * ewma_alloc(float weight)
{
	struct ewma_filter_t * filter;

	filter = malloc(sizeof(struct ewma_filter_t));
	if(!filter)
		return NULL;

	filter->weight = weight;
	filter->last = NAN;

	return filter;
}

void ewma_free(struct ewma_filter_t * filter)
{
	if(filter)
		free(filter);
}

float ewma_update(struct ewma_filter_t * filter, float value)
{
	if(isnan(filter->last))
		filter->last = value;
	else
		filter->last = filter->weight * value + (1 - filter->weight) * filter->last;
	return filter->last;
}

void ewma_clear(struct ewma_filter_t * filter)
{
	if(filter)
		filter->last = NAN;
}
