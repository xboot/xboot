#ifndef __XUI_RADIALBAR_H__
#define __XUI_RADIALBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_RADIALBAR_PRIMARY	= (0x0 << 8),
	XUI_RADIALBAR_SECONDARY	= (0x1 << 8),
	XUI_RADIALBAR_SUCCESS	= (0x2 << 8),
	XUI_RADIALBAR_INFO		= (0x3 << 8),
	XUI_RADIALBAR_WARNING	= (0x4 << 8),
	XUI_RADIALBAR_DANGER	= (0x5 << 8),
	XUI_RADIALBAR_LIGHT		= (0x6 << 8),
	XUI_RADIALBAR_DARK		= (0x7 << 8),
};

void xui_radialbar_ex(struct xui_context_t * ctx, int percent, int opt);

static inline void xui_radialbar(struct xui_context_t * ctx, int percent)
{
	xui_radialbar_ex(ctx, percent, XUI_RADIALBAR_PRIMARY);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_RADIALBAR_H__ */
