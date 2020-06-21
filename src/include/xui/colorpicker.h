#ifndef __XUI_COLORPICKER_H__
#define __XUI_COLORPICKER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

int xui_colorpicker_ex(struct xui_context_t * ctx, struct color_t * c, int opt);

static inline int xui_colorpicker(struct xui_context_t * ctx, struct color_t * c)
{
	return xui_colorpicker_ex(ctx, c, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_COLORPICKER_H__ */
