#ifndef __XUI_TREE_H__
#define __XUI_TREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

int xui_begin_tree_ex(struct xui_context_t * ctx, const char * label, int opt);
int xui_begin_tree(struct xui_context_t * ctx, const char * label);
void xui_end_tree(struct xui_context_t * ctx);
int xui_header_ex(struct xui_context_t * ctx, const char * label, int opt);
int xui_header(struct xui_context_t * ctx, const char * label);

#ifdef __cplusplus
}
#endif

#endif /* __XUI_TREE_H__ */
