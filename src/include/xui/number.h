#ifndef __XUI_NUMBER_H__
#define __XUI_NUMBER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xui/xui.h>

enum {
	XUI_NUMBER_PRIMARY		= (0x0 << 8),
	XUI_NUMBER_SECONDARY	= (0x1 << 8),
	XUI_NUMBER_SUCCESS		= (0x2 << 8),
	XUI_NUMBER_INFO			= (0x3 << 8),
	XUI_NUMBER_WARNING		= (0x4 << 8),
	XUI_NUMBER_DANGER		= (0x5 << 8),
	XUI_NUMBER_LIGHT		= (0x6 << 8),
	XUI_NUMBER_DARK			= (0x7 << 8),

	XUI_NUMBER_ROUNDED		= (0x1 << 12),
	XUI_NUMBER_OUTLINE		= (0x1 << 13),
};

int xui_number_ex(struct xui_context_t * ctx, double * value, double low, double high, double step, const char * fmt, int opt);

static inline int xui_number_float_ex(struct xui_context_t * ctx, float * value, float low, float high, float step, const char * fmt, int opt)
{
	double v;
	int change;

	xui_push_id(ctx, &value, sizeof(float *));
	v = *value;
	change = xui_number_ex(ctx, &v, low, high, step, fmt, opt);
	*value = v;
	xui_pop_id(ctx);
	return change;
}

static inline int xui_number_int_ex(struct xui_context_t * ctx, int * value, int low, int high, int step, const char * fmt, int opt)
{
	double v;
	int change;

	xui_push_id(ctx, &value, sizeof(int *));
	v = *value;
	change = xui_number_ex(ctx, &v, low, high, step, fmt, opt);
	*value = v;
	xui_pop_id(ctx);
	return change;
}

static inline int xui_number_uint_ex(struct xui_context_t * ctx, unsigned int * value, unsigned int low, unsigned int high, unsigned int step, const char * fmt, int opt)
{
	double v;
	int change;

	xui_push_id(ctx, &value, sizeof(unsigned int *));
	v = *value;
	change = xui_number_ex(ctx, &v, low, high, step, fmt, opt);
	*value = v;
	xui_pop_id(ctx);
	return change;
}

static inline int xui_number_char_ex(struct xui_context_t * ctx, char * value, char low, char high, char step, const char * fmt, int opt)
{
	double v;
	int change;

	xui_push_id(ctx, &value, sizeof(char *));
	v = *value;
	change = xui_number_ex(ctx, &v, low, high, step, fmt, opt);
	*value = v;
	xui_pop_id(ctx);
	return change;
}

static inline int xui_number_uchar_ex(struct xui_context_t * ctx, unsigned char * value, unsigned char low, unsigned char high, unsigned char step, const char * fmt, int opt)
{
	double v;
	int change;

	xui_push_id(ctx, &value, sizeof(unsigned char *));
	v = *value;
	change = xui_number_ex(ctx, &v, low, high, step, fmt, opt);
	*value = v;
	xui_pop_id(ctx);
	return change;
}

static inline int xui_number(struct xui_context_t * ctx, double * value, double low, double high, double step)
{
	return xui_number_ex(ctx, value, low, high, step, "%.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

static inline int xui_number_float(struct xui_context_t * ctx, float * value, float low, float high, float step)
{
	return xui_number_float_ex(ctx, value, low, high, step, "%.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

static inline int xui_number_int(struct xui_context_t * ctx, int * value, int low, int high, int step)
{
	return xui_number_int_ex(ctx, value, low, high, step, "%.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

static inline int xui_number_uint(struct xui_context_t * ctx, unsigned int * value, unsigned int low, unsigned int high, unsigned int step)
{
	return xui_number_uint_ex(ctx, value, low, high, step, "%.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

static inline int xui_number_char(struct xui_context_t * ctx, char * value, char low, char high, char step)
{
	return xui_number_char_ex(ctx, value, low, high, step, "%.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

static inline int xui_number_uchar(struct xui_context_t * ctx, unsigned char * value, unsigned char low, unsigned char high, unsigned char step)
{
	return xui_number_uchar_ex(ctx, value, low, high, step, "%.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
}

#ifdef __cplusplus
}
#endif

#endif /* __XUI_NUMBER_H__ */
