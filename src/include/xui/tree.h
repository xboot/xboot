#ifndef __XUI_TREE_H__
#define __XUI_TREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_TREE_PRIMARY	= (0x0 << 8),
	XUI_TREE_SECONDARY	= (0x1 << 8),
	XUI_TREE_SUCCESS	= (0x2 << 8),
	XUI_TREE_INFO		= (0x3 << 8),
	XUI_TREE_WARNING	= (0x4 << 8),
	XUI_TREE_DANGER		= (0x5 << 8),
	XUI_TREE_LIGHT		= (0x6 << 8),
	XUI_TREE_DARK		= (0x7 << 8),

	XUI_TREE_EXPANDED	= (0x1 << 12),
};

int xui_begin_tree_ex(struct xui_context_t * ctx, const char * label, int opt);
void xui_end_tree(struct xui_context_t * ctx);

static inline int xui_begin_tree(struct xui_context_t * ctx, const char * label)
{
	return xui_begin_tree_ex(ctx, label, XUI_TREE_SECONDARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TREE_H__ */
