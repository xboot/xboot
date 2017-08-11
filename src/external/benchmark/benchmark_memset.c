#include <benchmark.h>

struct memset_data_t {
	char * src;
	size_t size;
};

static void * memset_int(void)
{
	struct memset_data_t * d;

	d = malloc(sizeof(struct memset_data_t));
	if(!d)
		return 0;

	d->size = 1024 * 1024 * 1;
	d->src = malloc(d->size);
	if(!d->src)
	{
		free(d);
		return 0;
	}

	return d;
}

static void memset_exit(void * data)
{
	struct memset_data_t * d = (struct memset_data_t *)data;

	if(!d)
		return;

	if(d->src)
		free(d->src);

	free(d);
}

static void memset_routine(void * data)
{
	struct memset_data_t * d = (struct memset_data_t *)data;
	memset(d->src, 0, d->size);
}

struct benchmark_t benchmark_memset = {
	.name				= "memset",
	.desc				= "C library memset",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= memset_int,
	.benchmark_exit		= memset_exit,
	.benchmark_routine	= memset_routine,
};
