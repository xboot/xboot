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

	/* xfs context */
	struct xfs_context_t * __xfs_ctx;

	/* module list */
	struct module_list * __module_list;
};

struct runtime_t * __get_runtime(void);
void __set_runtime(struct runtime_t * r);
struct runtime_t * runtime_alloc(void);
void runtime_free(struct runtime_t * r);

#endif /* __RUNTIME_H__ */
