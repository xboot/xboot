#ifndef __XUI_CHECKBOX_H__
#define __XUI_CHECKBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_CHECKBOX_PRIMARY	= (0x0 << 8),
	XUI_CHECKBOX_SECONDARY	= (0x1 << 8),
	XUI_CHECKBOX_SUCCESS	= (0x2 << 8),
	XUI_CHECKBOX_INFO		= (0x3 << 8),
	XUI_CHECKBOX_WARNING	= (0x4 << 8),
	XUI_CHECKBOX_DANGER		= (0x5 << 8),
	XUI_CHECKBOX_LIGHT		= (0x6 << 8),
	XUI_CHECKBOX_DARK		= (0x7 << 8),
};

int xui_checkbox_ex(struct xui_context_t * ctx, const char * label, int * state, int opt);

static inline int xui_checkbox(struct xui_context_t * ctx, const char * label, int * state)
{
	return xui_checkbox_ex(ctx, label, state, XUI_CHECKBOX_PRIMARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_CHECKBOX_H__ */
