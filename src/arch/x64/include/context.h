#ifndef __X64_CONTEXT_H__
#define __X64_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

typedef void * fcontext_t;

struct transfer_t
{
	fcontext_t context;
	void * priv;
};

fcontext_t make_fcontext(void * sp, size_t size, void (*func)(struct transfer_t));
struct transfer_t jump_fcontext(fcontext_t context, void * priv);

#ifdef __cplusplus
}
#endif

#endif /* __X64_CONTEXT_H__ */
