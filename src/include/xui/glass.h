#ifndef __XUI_GLASS_H__
#define __XUI_GLASS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_GLASS_REFRESH	= (0x1 << 12),
};

void xui_glass_ex(struct xui_context_t * ctx, int radius, int opt);

static inline void xui_glass(struct xui_context_t * ctx, int radius)
{
	xui_glass_ex(ctx, radius, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_GLASS_H__ */
