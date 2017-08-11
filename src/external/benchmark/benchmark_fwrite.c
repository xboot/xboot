#include <benchmark.h>

#define BENCHMARK_FWRITE_FILE_PATH		"/home/jjj/__benchmark_fwrite_file.dat"

struct fwrite_data_t {
	FILE * fp;
	char * buf;
	size_t size;
};

static void * fwrite_int(void)
{
	struct fwrite_data_t * d;
	unsigned int i;

	d = malloc(sizeof(struct fwrite_data_t));
	if(!d)
		return 0;

	d->fp = fopen(BENCHMARK_FWRITE_FILE_PATH, "w");
	if(!d->fp)
	{
		free(d);
		return 0;
	}

	d->size = 1024 * 1024 * 1;
	d->buf = malloc(d->size);
	if(!d->buf)
	{
		fclose(d->fp);
		free(d);
		return 0;
	}

	for(i = 0; i < d->size; i++)
	{
		d->buf[i] = i & 0xff;
	}

	return d;
}

static void fwrite_exit(void * data)
{
	struct fwrite_data_t * d = (struct fwrite_data_t *)data;

	if(!d)
		return;

	if(d->buf)
		free(d->buf);
	fclose(d->fp);
	free(d);
}

static void fwrite_routine(void * data)
{
	struct fwrite_data_t * d = (struct fwrite_data_t *)data;
	size_t w;

	do {
		fseek(d->fp, 0L, SEEK_SET);
		w = fwrite(d->buf, d->size, 1, d->fp);
		fflush(d->fp);
	} while(w != 1);
}

struct benchmark_t benchmark_fwrite = {
	.name				= "fwrite",
	.desc				= "C library fwrite",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= fwrite_int,
	.benchmark_exit		= fwrite_exit,
	.benchmark_routine	= fwrite_routine,
};
