#include <benchmark.h>

#if 1
#include <xboot.h>
#define DPRINT(x...)	printf(x)
static double get_current_time(void)
{
	return (double)ktime_to_ns(ktime_get()) / 1000000000.0f;
}
#else
#include <sys/time.h>
#define DPRINT(x...)	printf(x)
static double get_current_time(void)
{
	struct timeval time;

	gettimeofday(&time, 0);
	return ((double)time.tv_sec + (double)time.tv_usec / 1000000.0);
}
#endif

static struct benchmark_t * benchmarks[] =
{
	&benchmark_memcpy,
	&benchmark_memset,
	&benchmark_memmove,
	&benchmark_memcmp,
	&benchmark_fwrite,
	&benchmark_fread,
	0,
};

static int benchmark_runner(struct benchmark_t * b, struct measurement_t * m)
{
	void * d;

	if(!b || !m)
		return -1;

	d = b->benchmark_init();
	if(!d)
		return -1;

	m->calls = 0;
	m->t2 = m->t1 = get_current_time();
	do {
		m->calls++;
		b->benchmark_routine(d);
		m->t2 = get_current_time();
	} while(m->t2 < m->t1 + 1);

	b->benchmark_exit(d);

	return 0;
}

void benchmark(const char * name)
{
	struct benchmark_t ** b;
	struct measurement_t m;

	for(b = &benchmarks[0]; b; b++)
	{
		if(strcmp((*b)->name, name) == 0)
		{
			DPRINT("%s - %s\r\n", (*b)->name, (*b)->desc);

			if(benchmark_runner(*b, &m) != 0)
			{
				DPRINT("Fail to run '%s' benchmark\r\n", name);
			}
			else
			{
				DPRINT((*b)->fmt, (double)((*b)->unit * m.calls) / (m.t2 - m.t1));
				DPRINT("\r\n");
			}
			return;
		}
	}

	DPRINT("No '%s' benchmark\r\n", name);
}
