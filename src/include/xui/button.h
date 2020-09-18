#ifndef __XUI_BUTTON_H__
#define __XUI_BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_BUTTON_PRIMARY		= (0x0 << 8),
	XUI_BUTTON_SECONDARY	= (0x1 << 8),
	XUI_BUTTON_SUCCESS		= (0x2 << 8),
	XUI_BUTTON_INFO			= (0x3 << 8),
	XUI_BUTTON_WARNING		= (0x4 << 8),
	XUI_BUTTON_DANGER		= (0x5 << 8),
	XUI_BUTTON_LIGHT		= (0x6 << 8),
	XUI_BUTTON_DARK			= (0x7 << 8),

	XUI_BUTTON_ROUNDED		= (0x1 << 12),
	XUI_BUTTON_OUTLINE		= (0x1 << 13),
};

int xui_button_ex(struct xui_context_t * ctx, int icon, const char * label, int opt);

static inline int xui_button(struct xui_context_t * ctx, const char * label)
{
	return xui_button_ex(ctx, 0, label, XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_CENTER);
}

static inline int xui_button_icon(struct xui_context_t * ctx, int icon)
{
	return xui_button_ex(ctx, icon, NULL, XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_CENTER);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_BUTTON_H__ */
