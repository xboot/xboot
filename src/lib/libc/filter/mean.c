/*
 * libc/filter/mean.c
 */

#include <mean.h>

struct mean_filter_t * mean_alloc(int length)
{
	struct mean_filter_t * filter;
	int i;

	if(length <= 0)
		return NULL;

	filter = malloc(sizeof(struct mean_filter_t));
	if(!filter)
		return NULL;

	filter->buffer = malloc(sizeof(int) * length);
	if(!filter->buffer)
	{
		free(filter);
		return NULL;
	}
	for(i = 0; i < length; i++)
		filter->buffer[i] = 0;
	filter->length = length;
	filter->index = 0;
	filter->count = 0;
	filter->sum = 0;

	return filter;
}

void mean_free(struct mean_filter_t * filter)
{
	if(filter)
	{
		if(filter->buffer)
			free(filter->buffer);
		free(filter);
	}
}

int mean_update(struct mean_filter_t * filter, int value)
{
	filter->sum -= filter->buffer[filter->index];
	filter->sum += value;
	filter->buffer[filter->index] = value;
	filter->index = (filter->index + 1) % filter->length;

	if(filter->count < filter->length)
		filter->count++;
	return filter->sum / filter->count;
}

void mean_clear(struct mean_filter_t * filter)
{
	int i;

	if(filter)
	{
		for(i = 0; i < filter->length; i++)
			filter->buffer[i] = 0;
		filter->index = 0;
		filter->count = 0;
		filter->sum = 0;
	}
}
