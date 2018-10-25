#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct transfer_t
{
	void * ctx;
	void * priv;
};

void * make_context(void * stack, size_t size, void (*func)(struct transfer_t));
struct transfer_t jump_context(void * ctx, void * priv);

#ifdef __cplusplus
}
#endif

#endif /* __CONTEXT_H__ */
