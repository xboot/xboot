#ifndef __XUI_WINDOW_H__
#define __XUI_WINDOW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_WINDOW_FULLSCREEN	= (0x1 << 12),
	XUI_WINDOW_NOTITLE		= (0x1 << 13),
	XUI_WINDOW_NOCLOSE		= (0x1 << 14),
	XUI_WINDOW_NORESIZE		= (0x1 << 15),
	XUI_WINDOW_POPUP		= (0x1 << 16),
};

int xui_begin_window_ex(struct xui_context_t * ctx, const char * title, struct region_t * r, int opt);
void xui_end_window(struct xui_context_t * ctx);

static inline int xui_begin_window(struct xui_context_t * ctx, const char * title, struct region_t * r)
{
	return xui_begin_window_ex(ctx, title, r, XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_WINDOW_H__ */
