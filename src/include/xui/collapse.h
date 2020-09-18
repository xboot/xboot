#ifndef __XUI_COLLAPSE_H__
#define __XUI_COLLAPSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_COLLAPSE_PRIMARY	= (0x0 << 8),
	XUI_COLLAPSE_SECONDARY	= (0x1 << 8),
	XUI_COLLAPSE_SUCCESS	= (0x2 << 8),
	XUI_COLLAPSE_INFO		= (0x3 << 8),
	XUI_COLLAPSE_WARNING	= (0x4 << 8),
	XUI_COLLAPSE_DANGER		= (0x5 << 8),
	XUI_COLLAPSE_LIGHT		= (0x6 << 8),
	XUI_COLLAPSE_DARK		= (0x7 << 8),

	XUI_COLLAPSE_EXPANDED	= (0x1 << 12),
};

int xui_collapse_ex(struct xui_context_t * ctx, int icon, const char * label, int opt);

static inline int xui_collapse(struct xui_context_t * ctx, int icon, const char * label)
{
	return xui_collapse_ex(ctx, icon, label, XUI_COLLAPSE_PRIMARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_COLLAPSE_H__ */
