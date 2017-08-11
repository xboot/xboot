#include <benchmark.h>

struct memmove_data_t {
	char * dst;
	char * src;
	size_t size;
};

static void * memmove_int(void)
{
	struct memmove_data_t * d;

	d = malloc(sizeof(struct memmove_data_t));
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

static void memmove_exit(void * data)
{
	struct memmove_data_t * d = (struct memmove_data_t *)data;

	if(!d)
		return;

	if(d->dst)
		free(d->dst);

	if(d->src)
		free(d->src);

	free(d);
}

static void memmove_routine(void * data)
{
	struct memmove_data_t * d = (struct memmove_data_t *)data;
	memmove(d->dst, d->src, d->size);
}

struct benchmark_t benchmark_memmove = {
	.name				= "memmove",
	.desc				= "C library memmove",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= memmove_int,
	.benchmark_exit		= memmove_exit,
	.benchmark_routine	= memmove_routine,
};
