#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct benchmark_t {
	const char * name;
	const char * desc;

	const char * fmt;
	double unit;

	void *(*benchmark_init)(void);
	void (*benchmark_exit)(void * data);
	void (*benchmark_routine)(void * data);
};

struct measurement_t {
	unsigned int calls;
	double t1, t2;
};

extern struct benchmark_t benchmark_memcpy;
extern struct benchmark_t benchmark_memset;
extern struct benchmark_t benchmark_memmove;
extern struct benchmark_t benchmark_memcmp;
extern struct benchmark_t benchmark_fwrite;
extern struct benchmark_t benchmark_fread;

void benchmark(const char * name);

#endif /* __BENCHMARK_H__ */
