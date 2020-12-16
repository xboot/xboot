#include <x.h>
#include <sandbox.h>

void * sandbox_malloc(size_t size)
{
	return malloc(size);
}

void * sandbox_memalign(size_t align, size_t size)
{
	return memalign(align, size);
}

void * sandbox_realloc(void * ptr, size_t size)
{
	return realloc(ptr, size);
}

void * sandbox_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

void sandbox_free(void * ptr)
{
	free(ptr);
}

void sandbox_meminfo(size_t * mused, size_t * mfree)
{
	struct mallinfo mi = mallinfo();

	if(mused)
		*mused = mi.uordblks;
	if(mfree)
		*mfree = mi.fordblks;
}
