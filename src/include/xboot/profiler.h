#ifndef __PROFILER_H__
#define __PROFILER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <hmap.h>
#include <xboot/ktime.h>
#include <clocksource/clocksource.h>

struct profiler_t
{
	uint64_t begin;
	uint64_t end;
	uint64_t elapsed;
	uint64_t count;
};

static inline void profiler_begin(struct profiler_t * p)
{
	if(p)
		p->begin = ktime_to_ns(ktime_get());
}

static inline void profiler_end(struct profiler_t * p)
{
	if(p)
	{
		p->end = ktime_to_ns(ktime_get());
		p->elapsed += p->end - p->begin;
		p->count++;
	}
}

struct profiler_t * profiler_search(struct hmap_t * m, const char * name);
struct hmap_t * profiler_alloc(int size);
void profiler_free(struct hmap_t * m);
void profiler_dump(struct hmap_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __PROFILER_H__ */
