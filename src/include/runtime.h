#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct runtime_t {
	/* memory pool */
	void * __pool;

	/* error number */
	int __errno;

	/* pseudo random seed - 48bit */
	unsigned short __seed[3];

	/* environ */
	struct environ_t __environ;

	/* standard input, output and error */
	FILE *__stdin, *__stdout, *__stderr;

	/* module list */
	struct module_list * __module_list;

	/* event base */
	struct event_base_t * __event_base;

	/* xfs context */
	struct xfs_context_t * __xfs_ctx;
};

struct runtime_t * runtime_get(void);
bool_t runtime_alloc_save(struct runtime_t ** rt);
bool_t runtime_free_restore(struct runtime_t * rt);

#ifdef __cplusplus
}
#endif

#endif /* __RUNTIME_H__ */
