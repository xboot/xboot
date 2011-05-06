#ifndef __STDLIB_H__
#define __STDLIB_H__


int abs(int n);
long labs(long n);
long long llabs(long long n);

void srand(int seed);
int rand(void);

int atoi(const char * nptr);
long atol(const char * nptr);
long long atoll(const char * nptr);
double atof(const char * nptr);

long strtol(const char * nptr, char ** endptr, int base);
long long strtoll(const char * nptr, char ** endptr, int base);
unsigned long strtoul(const char * nptr, char ** endptr, int base);
unsigned long long strtoull(const char * nptr, char ** endptr, int base);
double strtod(const char * nptr, char ** endptr);

#endif /* __STDLIB_H__ */
