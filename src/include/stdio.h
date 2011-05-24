#ifndef __STDIO_H__
#define __STDIO_H__

#include <types.h>
#include <stdarg.h>

#ifndef EOF
#define EOF		(-1)
#endif

typedef struct {
	int fd;
} FILE;

enum {
	SEEK_SET	= 0,
	SEEK_CUR	= 1,
	SEEK_END	= 2,
};

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

int fileno(FILE * fp);
int fflush(FILE * fp);
int ferror(FILE * fp);
void clearerr(FILE * fp);
FILE * fopen(const char * file, const char * mode);
int fclose(FILE * fp);
int fseek(FILE * fp, loff_t offset, int whence);
loff_t ftell(FILE * fp);
size_t fread(void * buf, size_t size, size_t count, FILE * fp);
size_t fwrite(const void * buf, size_t size, size_t count, FILE * fp);

int fputc(int c, FILE * fp);
int fputs(const char * s, FILE * fp);
int fprintf(FILE * fp, const char * fmt, ...);
int fgetc(FILE * fp);
char * fgets(char * buf, int n, FILE * fp);
int fscanf(FILE * fp, const char * fmt, ...);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

#endif /* __STDIO_H__ */
