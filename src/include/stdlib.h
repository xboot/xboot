#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <types.h>
#include <stddef.h>

#define RAND_MAX (0x7fffffff)
int rand(void);
void srand(unsigned int seed);

int abs(int n);
long labs(long n);
long long llabs(long long n);

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

void * bsearch(const void *key, const void *base, size_t nmemb, size_t size,
		int(*compar)(const void *, const void *));

/*
 * environment variable
 */
struct environ_t {
	char * content;
	struct environ_t * prev;
	struct environ_t * next;
};

char * getenv(const char * name);
int putenv(const char * str);
int setenv(const char * name, const char * val, int overwrite);
int unsetenv(const char * name);
int clearenv(void);

bool_t loadenv(char * file);
bool_t saveenv(char * file);

#endif /* __STDLIB_H__ */
