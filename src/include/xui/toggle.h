#ifndef __XUI_TOGGLE_H__
#define __XUI_TOGGLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

int xui_toggle_ex(struct xui_context_t * ctx, int * state, int opt);

static inline int xui_toggle(struct xui_context_t * ctx, int * state)
{
	return xui_toggle_ex(ctx, state, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TOGGLE_H__ */
