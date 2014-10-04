/*
 * libc/stdio/fclose.c
 */

#include <malloc.h>
#include <stdio.h>

int fclose(FILE * f)
{
	int err;

	if(!f)
		return EINVAL;

	if(!f->close)
		return EINVAL;

	if((err = f->rwflush(f)))
		return err;

	if((err = f->close(f)))
		return err;

	if(f->fifo_read)
		fifo_free(f->fifo_read);

	if(f->fifo_write)
		fifo_free(f->fifo_write);

	if(f->buf)
		free(f->buf);

	if(f)
		free(f);

	return err;
}
EXPORT_SYMBOL(fclose);
