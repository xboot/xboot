/*
 * libc/stdlib/bsearch.c
 */

#include <types.h>
#include <stddef.h>
#include <stdlib.h>

/*
 * Perform a binary search
 */
void * bsearch(const void * key, const void * base, size_t nmemb, size_t size,
		int (*compar)(const void *, const void *))
{
	size_t l, u, idx;
	const void *p;
	int comparison;

	l = 0;
	u = nmemb;
	while (l < u)
	{
		idx = (l + u) / 2;
		p = (void *) (((const char *) base) + (idx * size));
		comparison = (*compar)(key, p);
		if (comparison < 0)
			u = idx;
		else if (comparison > 0)
			l = idx + 1;
		else
			return (void *) p;
	}

	return NULL;
}
EXPORT_SYMBOL(bsearch);
