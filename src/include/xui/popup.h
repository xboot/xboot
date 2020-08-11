#ifndef __XUI_POPUP_H__
#define __XUI_POPUP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

void xui_open_popup(struct xui_context_t * ctx, const char * name);

static inline int xui_begin_popup(struct xui_context_t * ctx, const char * name)
{
	return xui_begin_window_ex(ctx, name, NULL, XUI_WINDOW_NOTITLE | XUI_WINDOW_NORESIZE | XUI_OPT_NOSCROLL | XUI_WINDOW_POPUP | XUI_OPT_CLOSED);
}

static inline void xui_end_popup(struct xui_context_t * ctx)
{
	xui_end_window(ctx);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_POPUP_H__ */
