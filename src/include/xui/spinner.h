#ifndef __XUI_SPINNER_H__
#define __XUI_SPINNER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_SPINNER_PRIMARY		= (0x0 << 8),
	XUI_SPINNER_SECONDARY	= (0x1 << 8),
	XUI_SPINNER_SUCCESS		= (0x2 << 8),
	XUI_SPINNER_INFO		= (0x3 << 8),
	XUI_SPINNER_WARNING		= (0x4 << 8),
	XUI_SPINNER_DANGER		= (0x5 << 8),
	XUI_SPINNER_LIGHT		= (0x6 << 8),
	XUI_SPINNER_DARK		= (0x7 << 8),
};

void xui_spinner_ex(struct xui_context_t * ctx, int opt);

static inline void xui_spinner(struct xui_context_t * ctx)
{
	xui_spinner_ex(ctx, XUI_SPINNER_PRIMARY);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_SPINNER_H__ */
