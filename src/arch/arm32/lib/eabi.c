/*
 * eabi.c
 */
#include <types.h>

int raise(int signal)
{
	return 0;
}

void __aeabi_unwind_cpp_pr0(void)
{
}

void __aeabi_unwind_cpp_pr1(void)
{
}

void __aeabi_memcpy(void * dest, const void * src, size_t n)
{
	(void)memcpy(dest, src, n);
}

void __aeabi_memset(void * dest, size_t n, int c)
{
	(void)memset(dest, c, n);
}
