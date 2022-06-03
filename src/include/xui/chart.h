#ifndef __XUI_CHART_H__
#define __XUI_CHART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_CHART_FILL		= (0x1 << 12),
};

void xui_chart_ex(struct xui_context_t * ctx, double * values, int len, double ymin, double ymax, struct color_t * c, int opt);
void xui_chart_float_ex(struct xui_context_t * ctx, float * values, int len, float ymin, float ymax, struct color_t * c, int opt);
void xui_chart_int_ex(struct xui_context_t * ctx, int * values, int len, int ymin, int ymax, struct color_t * c, int opt);

static inline void xui_chart(struct xui_context_t * ctx, double * values, int len, double ymin, double ymax, struct color_t * c)
{
	return xui_chart_ex(ctx, values, len, ymin, ymax, c, 0);
}

static inline void xui_chart_float(struct xui_context_t * ctx, float * values, int len, float ymin, float ymax, struct color_t * c)
{
	return xui_chart_float_ex(ctx, values, len, ymin, ymax, c, 0);
}

static inline void xui_chart_int(struct xui_context_t * ctx, int * values, int len, int ymin, int ymax, struct color_t * c)
{
	return xui_chart_int_ex(ctx, values, len, ymin, ymax, c, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_CHART_H__ */
