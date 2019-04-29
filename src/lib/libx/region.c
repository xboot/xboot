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

	if(size < 16)
		size = 16;
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

static inline void region_list_resize(struct region_list_t * rl, unsigned int size)
{
	if(rl && (rl->size != size))
	{
		if(size < 16)
			size = 16;
		if(size & (size - 1))
			size = roundup_pow_of_two(size);
		rl->size = size;
		rl->region = realloc(rl->region, rl->size * sizeof(struct region_t));
	}
}

void region_list_clone(struct region_list_t * rl, struct region_list_t * a)
{
	int count;

	if(rl)
	{
		if(!a)
			rl->count = 0;
		else
		{
			if(rl->size < a->size)
				region_list_resize(rl, a->size);
			if((count = a->count) > 0)
				memcpy(rl->region, a->region, sizeof(struct region_t) * count);
			rl->count = count;
		}
	}
}

void region_list_merge(struct region_list_t * rl, struct region_list_t * a)
{
	int count;
	int i;

	if(rl && a && ((count = a->count) > 0))
	{
		if(rl->size < a->size)
			region_list_resize(rl, a->size);
		for(i = 0; i < count; i++)
			region_list_add(rl, &a->region[i]);
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

	if(rl->count < 8)
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
		if(rl->size < rl->count)
			region_list_resize(rl, rl->size << 1);
		region_clone(&rl->region[rl->count], r);
		rl->count++;
	}
}

void region_list_clear(struct region_list_t * rl)
{
	if(rl)
		rl->count = 0;
}
