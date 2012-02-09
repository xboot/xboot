/*
 * libc/stdio/fwrite.c
 */

#include <stdio.h>

size_t __fwritex(const unsigned char * s, size_t l, FILE * f)
{
	size_t i=0;

	if (!f->wend && __towrite(f))
		return 0;

	if (l > f->wend - f->wpos)
		return f->write(f, s, l);

	if (f->lbf >= 0)
	{
		/* Match /^(.*\n|)/ */
		for (i=l; i && s[i-1] != '\n'; i--);
		if (i)
		{
			if (f->write(f, s, i) < i)
				return i;
			s += i;
			l -= i;
		}
	}

	memcpy(f->wpos, s, l);
	f->wpos += l;

	return l+i;
}

size_t fwrite(const void * src, size_t size, size_t nmemb, FILE * f)
{
	size_t k, l = size*nmemb;

	if (!l)
		return l;

	FLOCK(f);
	k = __fwritex(src, l, f);
	FUNLOCK(f);

	return k==l ? nmemb : l/size;
}
