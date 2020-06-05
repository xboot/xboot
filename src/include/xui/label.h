#ifndef __XUI_LABEL_H__
#define __XUI_LABEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

void xui_label_ex(struct xui_context_t * ctx, const char * utf8, int opt);

static inline void xui_label(struct xui_context_t * ctx, const char * utf8)
{
	xui_label_ex(ctx, utf8, XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_LABEL_H__ */
