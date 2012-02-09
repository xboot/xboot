#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include <xboot.h>

struct runtime_t {
	/* error number */
	int __errno;

	/* environ */
	struct environ_t __environ;

	/* pseudo random seed - 48bit */
	unsigned short __seed[3];

	/* standard input, output and error */
	FILE *__stdin, *__stdout, *__stderr;

	FILE * ofl_head;
	int ofl_lock;
};


struct runtime_t * __get_runtime(void);

struct runtime_t * runtime_alloc(void);
void runtime_free(struct runtime_t * runtime);

#endif /* __RUNTIME_H__ */
