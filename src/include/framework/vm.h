#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>
#include <graphic/font.h>
#include <xboot/window.h>

struct vmctx_t
{
	struct xfs_context_t * xfs;
	struct font_context_t * f;
	struct window_t * w;
	void * priv;
};

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
