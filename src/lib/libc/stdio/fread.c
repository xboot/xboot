/*
 * libc/stdio/fread.c
 */

#include <stdio.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

size_t fread(void * destv, size_t size, size_t nmemb, FILE * f)
{
	unsigned char * dest = destv;
	size_t len = size * nmemb, l = len, k;

	/*
	 * Never touch the file if length is zero..
	 */
	if(!l)
		return 0;

	FLOCK(f);

	if(f->rend - f->rpos > 0)
	{
		/*
		 * First exhaust the buffer.
		 */
		k = MIN(f->rend - f->rpos, l);
		memcpy(dest, f->rpos, k);
		f->rpos += k;
		dest += k;
		l -= k;
	}

	/*
	 * Read the remainder directly
	 */
	for(; l; l -= k, dest += k)
	{
		k = __toread(f) ? 0 : f->read(f, dest, l);
		if(k + 1 <= 1)
		{
			FUNLOCK(f);
			return (len - l) / size;
		}
	}

	FUNLOCK(f);
	return nmemb;
}
