#include <benchmark.h>

struct memcpy_data_t {
	char * dst;
	char * src;
	size_t size;
};

static void * memcpy_int(void)
{
	struct memcpy_data_t * d;

	d = malloc(sizeof(struct memcpy_data_t));
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

	return d;
}

static void memcpy_exit(void * data)
{
	struct memcpy_data_t * d = (struct memcpy_data_t *)data;

	if(!d)
		return;

	if(d->dst)
		free(d->dst);

	if(d->src)
		free(d->src);

	free(d);
}

static void memcpy_routine(void * data)
{
	struct memcpy_data_t * d = (struct memcpy_data_t *)data;
	memcpy(d->dst, d->src, d->size);
}

struct benchmark_t benchmark_memcpy = {
	.name				= "memcpy",
	.desc				= "C library memcpy",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= memcpy_int,
	.benchmark_exit		= memcpy_exit,
	.benchmark_routine	= memcpy_routine,
};
