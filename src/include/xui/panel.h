#ifndef __XUI_PANEL_H__
#define __XUI_PANEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_PANEL_TRANSPARENT	= (0x1 << 12),
};

void xui_begin_panel_ex(struct xui_context_t * ctx, const char * name, int opt);
void xui_end_panel(struct xui_context_t * ctx);

static inline void xui_begin_panel(struct xui_context_t * ctx, const char * name)
{
	xui_begin_panel_ex(ctx, name, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_PANEL_H__ */
