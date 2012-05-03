#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include <xboot.h>

struct runtime_t {
	/* memory pool */
	void * __pool;

	/* error number */
	int __errno;

	/* environ */
	struct environ_t __environ;

	/* pseudo random seed - 48bit */
	unsigned short __seed[3];

	/* standard input, output and error */
	FILE *__stdin, *__stdout, *__stderr;
};


void strap_runtime_init(void);
void strap_runtime_exit();

struct runtime_t * runtime_alloc(void);
void runtime_free(struct runtime_t * r);

void __set_runtime(struct runtime_t * r);
struct runtime_t * __get_runtime(void);

#endif /* __RUNTIME_H__ */
