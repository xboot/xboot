#ifndef __XUI_BADGE_H__
#define __XUI_BADGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_BADGE_PRIMARY	= (0x0 << 8),
	XUI_BADGE_SECONDARY	= (0x1 << 8),
	XUI_BADGE_SUCCESS	= (0x2 << 8),
	XUI_BADGE_INFO		= (0x3 << 8),
	XUI_BADGE_WARNING	= (0x4 << 8),
	XUI_BADGE_DANGER	= (0x5 << 8),
	XUI_BADGE_LIGHT		= (0x6 << 8),
	XUI_BADGE_DARK		= (0x7 << 8),

	XUI_BADGE_ROUNDED	= (0x1 << 12),
	XUI_BADGE_OUTLINE	= (0x1 << 13),
};

void xui_badge_ex(struct xui_context_t * ctx, const char * label, int opt);

static inline void xui_badge(struct xui_context_t * ctx, const char * label)
{
	xui_badge_ex(ctx, label, XUI_BADGE_PRIMARY);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_BADGE_H__ */
