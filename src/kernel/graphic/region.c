/*
 * kernel/graphic/region.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <graphic/region.h>

struct region_list_t * region_list_alloc(unsigned int size)
{
	struct region_list_t * rl;
	struct region_t * r;

	if(size < 16)
		size = 16;

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
		rl->size = size;
		rl->region = realloc(rl->region, rl->size * sizeof(struct region_t));
	}
}

void region_list_clone(struct region_list_t * rl, struct region_list_t * o)
{
	int count;

	if(rl)
	{
		if(!o)
			rl->count = 0;
		else
		{
			if(rl->size < o->size)
				region_list_resize(rl, o->size);
			if((count = o->count) > 0)
				memcpy(rl->region, o->region, sizeof(struct region_t) * count);
			rl->count = count;
		}
	}
}

void region_list_merge(struct region_list_t * rl, struct region_list_t * o)
{
	int count;
	int i;

	if(rl && o && ((count = o->count) > 0))
	{
		for(i = 0; i < count; i++)
			region_list_add(rl, &o->region[i]);
	}
}

void region_list_add(struct region_list_t * rl, struct region_t * r)
{
	struct region_t * p;
	int index = -1;
	int i;

	if(!rl || !r)
		return;

	for(i = 0; i < rl->count; i++)
	{
		if(region_overlap(&rl->region[i], r))
		{
			index = i;
			break;
		}
	}

	if(index >= 0)
	{
		p = &rl->region[index];
		region_union(p, p, r);
	}
	else
	{
		if(rl->size <= rl->count)
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
