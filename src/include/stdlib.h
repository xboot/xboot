#ifndef __STDLIB_H__
#define __STDLIB_H__


int abs(int n);
long labs(long n);
long long llabs(long long n);

void srand(int seed);
int rand(void);

long strtol(const char * nptr, char ** endptr, int base);
long long strtoll(const char * nptr, char ** endptr, int base);
unsigned long strtoul(const char * nptr, char ** endptr, int base);
unsigned long long strtoull(const char * nptr, char ** endptr, int base);

#endif /* __STDLIB_H__ */
