#ifndef __XUI_TABBAR_H__
#define __XUI_TABBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_TABBAR_PRIMARY		= (0x0 << 8),
	XUI_TABBAR_SECONDARY	= (0x1 << 8),
	XUI_TABBAR_SUCCESS		= (0x2 << 8),
	XUI_TABBAR_INFO			= (0x3 << 8),
	XUI_TABBAR_WARNING		= (0x4 << 8),
	XUI_TABBAR_DANGER		= (0x5 << 8),
	XUI_TABBAR_LIGHT		= (0x6 << 8),
	XUI_TABBAR_DARK			= (0x7 << 8),
};

int xui_tabbar_ex(struct xui_context_t * ctx, int icon, const char * label, int state, int opt);

static inline int xui_tabbar(struct xui_context_t * ctx, int icon, const char * label, int state)
{
	return xui_tabbar_ex(ctx, icon, label, state, XUI_TABBAR_PRIMARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TABBAR_H__ */
