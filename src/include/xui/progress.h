#ifndef __XUI_PROGRESS_H__
#define __XUI_PROGRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_PROGRESS_PRIMARY	= (0x0 << 8),
	XUI_PROGRESS_SECONDARY	= (0x1 << 8),
	XUI_PROGRESS_SUCCESS	= (0x2 << 8),
	XUI_PROGRESS_INFO		= (0x3 << 8),
	XUI_PROGRESS_WARNING	= (0x4 << 8),
	XUI_PROGRESS_DANGER		= (0x5 << 8),
	XUI_PROGRESS_LIGHT		= (0x6 << 8),
	XUI_PROGRESS_DARK		= (0x7 << 8),

	XUI_PROGRESS_HORIZONTAL	= (0x0 << 12),
	XUI_PROGRESS_VERTICAL	= (0x1 << 12),
};

void xui_progress_ex(struct xui_context_t * ctx, int percent, int opt);

static inline void xui_progress(struct xui_context_t * ctx, int percent)
{
	xui_progress_ex(ctx, percent, XUI_PROGRESS_PRIMARY | XUI_PROGRESS_HORIZONTAL);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_PROGRESS_H__ */
