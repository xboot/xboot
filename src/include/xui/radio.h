#ifndef __XUI_RADIO_H__
#define __XUI_RADIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_RADIO_PRIMARY	= (0x0 << 8),
	XUI_RADIO_SECONDARY	= (0x1 << 8),
	XUI_RADIO_SUCCESS	= (0x2 << 8),
	XUI_RADIO_INFO		= (0x3 << 8),
	XUI_RADIO_WARNING	= (0x4 << 8),
	XUI_RADIO_DANGER	= (0x5 << 8),
	XUI_RADIO_LIGHT		= (0x6 << 8),
	XUI_RADIO_DARK		= (0x7 << 8),
};

int xui_radio_ex(struct xui_context_t * ctx, const char * label, int state, int opt);

static inline int xui_radio(struct xui_context_t * ctx, const char * label, int state)
{
	return xui_radio_ex(ctx, label, state, XUI_RADIO_PRIMARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_RADIO_H__ */
