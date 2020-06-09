#ifndef __XUI_TOGGLE_H__
#define __XUI_TOGGLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_TOGGLE_PRIMARY		= (0x0 << 8),
	XUI_TOGGLE_SECONDARY	= (0x1 << 8),
	XUI_TOGGLE_SUCCESS		= (0x2 << 8),
	XUI_TOGGLE_INFO			= (0x3 << 8),
	XUI_TOGGLE_WARNING		= (0x4 << 8),
	XUI_TOGGLE_DANGER		= (0x5 << 8),
	XUI_TOGGLE_LIGHT		= (0x6 << 8),
	XUI_TOGGLE_DARK			= (0x7 << 8),
};

int xui_toggle_ex(struct xui_context_t * ctx, int * state, int opt);

static inline int xui_toggle(struct xui_context_t * ctx, int * state)
{
	return xui_toggle_ex(ctx, state, XUI_TOGGLE_PRIMARY);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TOGGLE_H__ */
