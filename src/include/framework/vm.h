#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>
#include <xboot/window.h>

struct vmctx_t
{
	struct xfs_context_t * xfs;
	struct window_t * w;
};

int vmexec(const char * path, const char * fb, const char * input);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
