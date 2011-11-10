#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include <xboot.h>

struct runtime_t {
	/* error number */
	int __errno;

	/* environ */
	struct environ_t __environ;
};


struct runtime_t * __get_runtime(void);

struct runtime_t * runtime_alloc(void);
void runtime_free(struct runtime_t * runtime);

#endif /* __RUNTIME_H__ */
