/*
 * libx/region.c
 */

#include <log2.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <region.h>

struct region_list_t * region_list_alloc(unsigned int size)
{
	struct region_list_t * rl;
	struct region_t * r;

	if(size < 2)
		size = 2;
	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	r = malloc(size * sizeof(struct region_t));
	if(!r)
		return NULL;

	rl = malloc(sizeof(struct region_list_t));
	if(!rl)
	{
		free(r);
		return NULL;
	}

	rl->region = r;
	rl->size = size;
	rl->count = 0;
	return rl;
}

void region_list_free(struct region_list_t * rl)
{
	if(rl)
	{
		free(rl->region);
		free(rl);
	}
}

void region_list_add(struct region_list_t * rl, struct region_t * r)
{
	struct region_t region, * p;
	int area = INT_MAX;
	int index = -1;
	int i;

	if(!rl || !r)
		return;

	if(rl->count < rl->size)
	{
		for(i = 0; i < rl->count; i++)
		{
			p = &rl->region[i];
			if(region_intersect(&region, p, r))
			{
				if(region.area >= r->area)
				{
					return;
				}
				else
				{
					region_union(&region, p, r);
					if(region.area < area)
					{
						area = region.area;
						index = i;
					}
				}
			}
		}
	}
	else
	{
		for(i = 0; i < rl->count; i++)
		{
			p = &rl->region[i];
			if(region_union(&region, p, r))
			{
				if(region.area <= p->area)
				{
					return;
				}
				else if(region.area < area)
				{
					area = region.area;
					index = i;
				}
			}
		}
	}

	if(index >= 0)
	{
		p = &rl->region[index];
		region_union(p, p, r);
	}
	else
	{
		if(rl->count >= rl->size)
		{
			rl->size <<= 1;
			rl->region = realloc(rl->region, rl->size * sizeof(struct region_t));
		}
		region_clone(&rl->region[rl->count], r);
		rl->count++;
	}
}

void region_list_clear(struct region_list_t * rl)
{
	if(rl)
		rl->count = 0;
}
