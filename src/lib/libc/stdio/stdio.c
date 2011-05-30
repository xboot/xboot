/*
 * libc/stdio/stdio.c
 */

#include <stdio.h>

static struct list_head streams = { &(streams), &(streams) };

/*
 * stdin
 */
static unsigned char __stdin_buf[BUFSIZ];
static FILE __stdin = {
	.fd			= 0,
	.eof		= 0,
	.error		= 0,
	.ofs		= 0,
	.in = {
		.buf	= __stdin_buf,
		.limit	= 0,
		.pos	= 0,
		.dirty	= 0
	},
	.out = {
		.buf	= NULL,
		.limit	= 0,
		.pos	= 0,
		.dirty	= 0
	}
};
FILE * stdin = &__stdin;

/*
 * stdout
 */
static unsigned char __stdout_buf[BUFSIZ];
static FILE __stdout = {
	.fd			= 1,
	.eof		= 0,
	.error		= 0,
	.ofs		= 0,
	.in = {
		.buf	= NULL,
		.limit	= 0,
		.pos	= 0,
		.dirty	= 0
	},
	.out = {
		.buf	= __stdout_buf,
		.limit	= BUFSIZ,
		.pos	= 0,
		.dirty	= 0
	}
};
FILE * stdout = &__stdout;

/*
 * stderr
 */
static unsigned char __stderr_buf[BUFSIZ];
static FILE __stderr = {
	.fd			= 2,
	.eof		= 0,
	.error		= 0,
	.ofs		= 0,
	.in = {
		.buf	= NULL,
		.limit	= 0,
		.pos	= 0,
		.dirty	= 0
	},
	.out = {
		.buf	= __stderr_buf,
		.limit	= BUFSIZ,
		.pos	= 0,
		.dirty	= 0
	}
};
FILE * stderr = &__stderr;


FILE * __create_stdio(void)
{
	FILE * f;

	f = malloc(sizeof(FILE));
	if (f == NULL)
		return NULL;

	f->fd = -1;
	f->eof = f->error = 0;
	f->ofs = 0;

	f->in.buf = NULL;
	f->in.limit = f->in.pos = 0;
	f->in.dirty = 0;

	f->out.buf = NULL;
	f->out.limit = f->out.pos = 0;
	f->out.dirty = 0;

	init_list_head(&(f->node));
	list_add_tail(&streams, &(f->node));

	return f;
}

void __destroy_stdio(FILE * f)
{
	if (f == NULL)
		return;

	if ((f == stdin) || (f == stdout) || (f == stderr))
	{
		f->eof = f->error = 0;
		f->ofs = 0;
		if (f->in.buf != NULL)
			memset(f->in.buf, 0, BUFSIZ);
		f->in.limit = f->in.pos = 0;
		f->in.dirty = 0;
		if (f->out.buf != NULL)
			memset(f->out.buf, 0, BUFSIZ);
		f->out.limit = f->out.pos = 0;
		f->out.dirty = 0;
		return;
	}

	if (f->in.buf != NULL)
		free(f->in.buf);
	if (f->out.buf != NULL)
		free(f->out.buf);

	list_del(&(f->node));

	memset(f, 0, sizeof(FILE));
	free(f);
}

int __fill_stdio(FILE * f)
{
	int err;

	if (f == NULL)
		return 0;

	if (f->in.buf == NULL)
	{
		f->out.limit = BUFSIZ;
		return 0;
	}

	memset(f->in.buf, 0, BUFSIZ);
	f->in.pos = 0;
	f->in.dirty = 0;

	err = 0;
	f->error = 0;
	f->eof = 0;

	f->in.limit = read(f->fd, f->in.buf, BUFSIZ);

	if ((ssize_t) f->in.limit > 0)
	{
		if (f->out.buf != NULL)
		{
			memcpy(f->out.buf, f->in.buf, BUFSIZ);
			f->out.limit = f->in.limit;
			f->out.pos = f->in.pos;
			f->out.dirty = 0;
		}
	}

	if (f->in.limit == 0)
	{
		f->eof = 1;
		f->out.limit = f->in.limit;
		f->out.pos = f->in.pos;
		f->out.dirty = 0;
	}

	if ((ssize_t) f->in.limit < 0)
	{
		err = errno;
		f->error = 1;
		f->in.limit = f->out.limit = 0;
	}

	errno = err;
	return (err == 0) ? 0 : -1;
}

int __flush_stdio(void)
{
	struct list_head * pos;
	FILE * f;
	int err = 0;

	for(pos = (&streams)->next; pos != &streams; pos = pos->next)
	{
		f = list_entry(pos, FILE, node);
		if (fflush(f) == EOF)
			err = errno;
	}

	fflush(stdin);
	fflush(stdout);
	fflush(stderr);

	return (err == 0) ? 0 : EOF;
}
