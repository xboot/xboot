#ifndef __STDLIB_H__
#define __STDLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <environ.h>

#define RAND_MAX (0x7fffffff)

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

int rand(void);
void srand(unsigned int seed);

int abs(int n);
long labs(long n);
long long llabs(long long n);

div_t div(int num, int den);
ldiv_t ldiv(long num, long den);
lldiv_t lldiv(long long num, long long den);

int atoi(const char * nptr);
long atol(const char * nptr);
long long atoll(const char * nptr);
double atof(const char * nptr);

long strtol(const char * nptr, char ** endptr, int base);
long long strtoll(const char * nptr, char ** endptr, int base);
unsigned long strtoul(const char * nptr, char ** endptr, int base);
unsigned long long strtoull(const char * nptr, char ** endptr, int base);
double strtod(const char * nptr, char ** endptr);

intmax_t strtoimax(const char * nptr, char ** endptr, int base);
uintmax_t strtoumax(const char * nptr, char ** endptr, int base);
intmax_t strntoimax(const char * nptr, char ** endptr, int base, size_t n);
uintmax_t strntoumax(const char * nptr, char ** endptr, int base, size_t n);

void * bsearch(const void * key, const void * base, size_t nmemb, size_t size,
		int (*compar)(const void *, const void *));
void qsort(void * aa, size_t n, size_t es, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif /* __STDLIB_H__ */
