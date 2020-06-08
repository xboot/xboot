#ifndef __XUI_RADIO_H__
#define __XUI_RADIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

int xui_radio_ex(struct xui_context_t * ctx, const char * label, int active, int opt);

static inline int xui_radio(struct xui_context_t * ctx, const char * label, int active)
{
	return xui_radio_ex(ctx, label, active, XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_RADIO_H__ */
