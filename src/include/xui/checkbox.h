#ifndef __XUI_CHECKBOX_H__
#define __XUI_CHECKBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

int xui_checkbox_ex(struct xui_context_t * ctx, const char * label, int * state, int opt);

static inline int xui_checkbox(struct xui_context_t * ctx, const char * label, int * state)
{
	return xui_checkbox_ex(ctx, label, state, XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_CHECKBOX_H__ */
