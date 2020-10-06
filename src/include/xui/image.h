#ifndef __XUI_IMAGE_H__
#define __XUI_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_IMAGE_NONE		= (0x0 << 12),
	XUI_IMAGE_CONTAIN	= (0x1 << 12),
	XUI_IMAGE_COVER		= (0x2 << 12),
	XUI_IMAGE_FILL		= (0x3 << 12),

	XUI_IMAGE_REFRESH	= (0x1 << 14),
};

int xui_image_ex(struct xui_context_t * ctx, struct surface_t * s, double angle, int opt);

static inline int xui_image(struct xui_context_t * ctx, struct surface_t * s)
{
	return xui_image_ex(ctx, s, 0, XUI_IMAGE_NONE);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_IMAGE_H__ */
