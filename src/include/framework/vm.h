#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>
#include <xboot/display.h>
#include <xboot/event.h>

struct vmctx_t
{
	struct xfs_context_t * xfs;
	struct display_t * disp;
	struct event_context_t * ectx;
};

int vmexec(const char * path, const char * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
