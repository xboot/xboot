#ifndef __XUI_SPLIT_H__
#define __XUI_SPLIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_SPLIT_HORIZONTAL	= (0x0 << 8),
	XUI_SPLIT_VERTICAL		= (0x1 << 8),
};

void xui_split_ex(struct xui_context_t * ctx, int opt);

static inline void xui_split(struct xui_context_t * ctx)
{
	xui_split_ex(ctx, XUI_SPLIT_HORIZONTAL);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_SPLIT_H__ */
