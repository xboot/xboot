#include <benchmark.h>

#define BENCHMARK_FREAD_FILE_PATH		"/home/jjj/__benchmark_fread_file.dat"

struct fread_data_t {
	FILE * fp;
	char * buf;
	size_t size;
};

static void * fread_int(void)
{
	struct fread_data_t * d;
	unsigned int i;

	d = malloc(sizeof(struct fread_data_t));
	if(!d)
		return 0;

	d->size = 1024 * 1024 * 1;
	d->buf = malloc(d->size);
	if(!d->buf)
	{
		free(d);
		return 0;
	}

	for(i = 0; i < d->size; i++)
	{
		d->buf[i] = i & 0xff;
	}

	d->fp = fopen(BENCHMARK_FREAD_FILE_PATH, "w");
	if(!d->fp)
	{
		free(d->buf);
		free(d);
	}
	fseek(d->fp, 0L, SEEK_SET);
	fwrite(d->buf, d->size, 1, d->fp);
	fflush(d->fp);
	fclose(d->fp);

	d->fp = fopen(BENCHMARK_FREAD_FILE_PATH, "r");
	if(!d->fp)
	{
		free(d->buf);
		free(d);
		return 0;
	}

	return d;
}

static void fread_exit(void * data)
{
	struct fread_data_t * d = (struct fread_data_t *)data;

	if(!d)
		return;

	if(d->buf)
		free(d->buf);
	fclose(d->fp);
	free(d);
}

static void fread_routine(void * data)
{
	struct fread_data_t * d = (struct fread_data_t *)data;
	size_t r;

	do {
		fseek(d->fp, 0L, SEEK_SET);
		r = fread(d->buf, d->size, 1, d->fp);
	} while(r != 1);
}

struct benchmark_t benchmark_fread = {
	.name				= "fread",
	.desc				= "C library fread",

	.fmt				= "Bandwidth: %fMB/s",
	.unit				= 1,

	.benchmark_init		= fread_int,
	.benchmark_exit		= fread_exit,
	.benchmark_routine	= fread_routine,
};
