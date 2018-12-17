#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>
#include <cairo-xboot.h>

struct vmctx_t
{
	struct xfs_context_t * xfs;
	struct framebuffer_t * fb;
	cairo_surface_t * cs;
	cairo_t * cr;
};

int vmexec(const char * path, const char * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
