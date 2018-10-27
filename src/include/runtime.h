#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct runtime_t {
	/* error number */
	int __errno;

	/* standard input, output and error */
	FILE *__stdin, *__stdout, *__stderr;

	/* event base */
	struct event_base_t * __event_base;

	/* xfs context */
	void * __xfs_ctx;
};

struct runtime_t * runtime_get(void);
void runtime_create_save(struct runtime_t * rt, const char * path, struct runtime_t ** r);
void runtime_destroy_restore(struct runtime_t * rt, struct runtime_t * r);

#ifdef __cplusplus
}
#endif

#endif /* __RUNTIME_H__ */
