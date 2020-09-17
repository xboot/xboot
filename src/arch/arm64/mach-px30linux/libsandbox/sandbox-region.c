#include <x.h>
#include <sandbox.h>

struct sandbox_region_list_t * sandbox_region_list_alloc(unsigned int size)
{
	struct sandbox_region_list_t * rl;
	struct sandbox_region_t * r;

	if(size < 16)
		size = 16;

	r = malloc(size * sizeof(struct sandbox_region_t));
	if(!r)
		return NULL;

	rl = malloc(sizeof(struct sandbox_region_list_t));
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

void sandbox_region_list_free(struct sandbox_region_list_t * rl)
{
	if(rl)
	{
		free(rl->region);
		free(rl);
	}
}

static inline void sandbox_region_list_resize(struct sandbox_region_list_t * rl, unsigned int size)
{
	if(rl && (rl->size != size))
	{
		if(size < 16)
			size = 16;
		rl->size = size;
		rl->region = realloc(rl->region, rl->size * sizeof(struct sandbox_region_t));
	}
}

void sandbox_region_list_clone(struct sandbox_region_list_t * rl, struct sandbox_region_list_t * o)
{
	int count;

	if(rl)
	{
		if(!o)
			rl->count = 0;
		else
		{
			if(rl->size < o->size)
				sandbox_region_list_resize(rl, o->size);
			if((count = o->count) > 0)
				memcpy(rl->region, o->region, sizeof(struct sandbox_region_t) * count);
			rl->count = count;
		}
	}
}

void sandbox_region_list_merge(struct sandbox_region_list_t * rl, struct sandbox_region_list_t * o)
{
	int count;
	int i;

	if(rl && o && ((count = o->count) > 0))
	{
		for(i = 0; i < count; i++)
			sandbox_region_list_add(rl, &o->region[i]);
	}
}

void sandbox_region_list_add(struct sandbox_region_list_t * rl, struct sandbox_region_t * r)
{
	struct sandbox_region_t * p;
	int index = -1;
	int i;

	if(!rl || !r)
		return;

	for(i = 0; i < rl->count; i++)
	{
		if(sandbox_region_overlap(&rl->region[i], r))
		{
			index = i;
			break;
		}
	}

	if(index >= 0)
	{
		p = &rl->region[index];
		sandbox_region_union(p, p, r);
	}
	else
	{
		if(rl->size <= rl->count)
			sandbox_region_list_resize(rl, rl->size << 1);
		sandbox_region_clone(&rl->region[rl->count], r);
		rl->count++;
	}
}

void sandbox_region_list_clear(struct sandbox_region_list_t * rl)
{
	if(rl)
		rl->count = 0;
}
