#include <benchmark.h>

struct memcmp_data_t {
	char * dst;
	char * src;
	size_t size;
};

static void * memcmp_int(void)
{
	struct memcmp_data_t * d;

	d = malloc(sizeof(struct memcmp_data_t));
	if(!d)
		return 0;

	d->size = 1024 * 1024 * 1;
	d->dst = malloc(d->size);
	d->src = malloc(d->size);

	if(!d->dst || !d->src)
	{
		if(!d->dst)
			free(d->dst);
		if(!d->src)
			free(d->src);
		if(!d)
			free(d);
		return 0;
	}

	memset(d->dst, 0, d->size);
	memset(d->src, 0, d->size);

	return d;
}

static void memcmp_exit(void * data)
{
	struct memcmp_data_t * d = (struct memcmp_data_t *)data;

	if(!d)
		return;

	if(d->dst)
		free(d->dst);

	if(d->src)
		free(d->src);

	free(d);
}

static void memcmp_routine(void * data)
{
	struct memcmp_data_t * d = (struct memcmp_data_t *)data;

	if(memcmp(d->dst, d->src, d->size) == 0)
	{
	}
}

struct benchmark_t benchmark_memcmp = {
	.name				= "memcmp",
	.desc				= "C library memcmp",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= memcmp_int,
	.benchmark_exit		= memcmp_exit,
	.benchmark_routine	= memcmp_routine,
};
