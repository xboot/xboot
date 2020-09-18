#ifndef __XUI_SPLIT_H__
#define __XUI_SPLIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_SPLIT_PRIMARY		= (0x0 << 8),
	XUI_SPLIT_SECONDARY		= (0x1 << 8),
	XUI_SPLIT_SUCCESS		= (0x2 << 8),
	XUI_SPLIT_INFO			= (0x3 << 8),
	XUI_SPLIT_WARNING		= (0x4 << 8),
	XUI_SPLIT_DANGER		= (0x5 << 8),
	XUI_SPLIT_LIGHT			= (0x6 << 8),
	XUI_SPLIT_DARK			= (0x7 << 8),

	XUI_SPLIT_HORIZONTAL	= (0x0 << 12),
	XUI_SPLIT_VERTICAL		= (0x1 << 12),
};

void xui_split_ex(struct xui_context_t * ctx, int opt);

static inline void xui_split(struct xui_context_t * ctx)
{
	xui_split_ex(ctx, XUI_SPLIT_LIGHT | XUI_SPLIT_HORIZONTAL);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_SPLIT_H__ */
