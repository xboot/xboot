#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>
#include <xboot/display.h>

struct vmctx_t
{
	struct xfs_context_t * xfs;
	struct display_t * d;
};

int vmexec(const char * path, const char * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
