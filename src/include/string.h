#ifndef __STRING_H__
#define __STRING_H__

#include <types.h>

size_t strlen(const char * s);
size_t strnlen(const char * s, size_t n);

char * strcat(char * dest, const char * src);
char * strncat(char * dest, const char * src, size_t n);
size_t strlcat(char * dest, const char * src, size_t n);

char * strcpy(char * dest, const char * src);
char * strncpy(char * dest, const char * src, size_t n);
size_t strlcpy(char * dest, const char * src, size_t n);

int strcmp(const char * s1, const char * s2);
int strncmp(const char * s1, const char * s2, size_t n);
int strnicmp(const char * s1, const char * s2, size_t n);
int strcasecmp(const char * s1, const char * s2);
int strncasecmp(const char * s1, const char * s2, size_t n);

char * strchr(const char * s, int c);
char * strrchr(const char * s, int c);
char * strnchr(const char * s, size_t n, int c);

size_t strspn(const char * s, const char * accept);
size_t strcspn(const char * s, const char * reject);

char * strdup(const char * s);
char * strpbrk(const char * s1, const char * s2);
char * strsep(char ** s, const char * ct);
char * strstr(const char * s1, const char * s2);
char * strnstr(const char * s1, const char * s2, size_t n);

void * memscan(void * addr, int c, size_t size);
void * memchr(const void * s, int c, size_t n);
void * memcpy(void * dest, const void * src, size_t len);
void * memset(void * s, int c, size_t n);
void * memmove(void * dest, const void * src, size_t n);
int memcmp(const void * s1, const void * s2, size_t n);

#endif /* __STRING_H__ */
