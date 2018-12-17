#ifndef __PROFILER_H__
#define __PROFILER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <list.h>

struct profiler_t
{
	struct hlist_node node;
	char * name;
	int event;
	int data;
	uint64_t begin;
	uint64_t end;
	uint64_t count;
};

struct profiler_t * profiler_search(const char * name);
void profiler_snap(const char * name, int event, int data);
void profiler_dump(void);
void profiler_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __PROFILER_H__ */
