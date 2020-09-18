#ifndef __XUI_TEXTEDIT_H__
#define __XUI_TEXTEDIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_TEXTEDIT_PRIMARY	= (0x0 << 8),
	XUI_TEXTEDIT_SECONDARY	= (0x1 << 8),
	XUI_TEXTEDIT_SUCCESS	= (0x2 << 8),
	XUI_TEXTEDIT_INFO		= (0x3 << 8),
	XUI_TEXTEDIT_WARNING	= (0x4 << 8),
	XUI_TEXTEDIT_DANGER		= (0x5 << 8),
	XUI_TEXTEDIT_LIGHT		= (0x6 << 8),
	XUI_TEXTEDIT_DARK		= (0x7 << 8),

	XUI_TEXTEDIT_ROUNDED	= (0x1 << 12),
	XUI_TEXTEDIT_OUTLINE	= (0x1 << 13),
};

int xui_textedit_ex(struct xui_context_t * ctx, char * buf, int size, int opt);

static inline int xui_textedit(struct xui_context_t * ctx, char * buf, int size)
{
	return xui_textedit_ex(ctx, buf, size, XUI_TEXTEDIT_PRIMARY | XUI_TEXTEDIT_OUTLINE | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TEXTEDIT_H__ */
