/*
 * kernel/core/profiler.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <xboot/profiler.h>

struct hmap_t * profiler_alloc(int size)
{
	return hmap_alloc(size);
}

static void hmap_entry_callback(struct hmap_entry_t * e)
{
	if(e && e->value)
		free(e->value);
}

void profiler_free(struct hmap_t * m)
{
	hmap_free(m, hmap_entry_callback);
}

struct profiler_t * profiler_search(struct hmap_t * m, const char * name)
{
	struct profiler_t * p = NULL;

	if(m && name)
	{
		p = hmap_search(m, name);
		if(!p)
		{
			p = malloc(sizeof(struct profiler_t));
			if(p)
			{
				p->begin = 0;
				p->end = 0;
				p->elapsed = 0;
				p->count = 0;
				hmap_add(m, name, p);
			}
		}
	}
	return p;
}

void profiler_dump(struct hmap_t * m)
{
	struct hmap_entry_t * e;
	struct profiler_t * p;

	if(m)
	{
		printf("Profiler analysis:\r\n");
		hmap_sort(m);
		hmap_for_each_entry(e, m)
		{
			p = (struct profiler_t *)e->value;
		    printf("%-32s %ld %12.3f(us)\r\n", e->key, p->count, (p->count > 0) ? ((double)p->elapsed / 1000.0f) / (double)p->count : 0);
		}
	}
}
