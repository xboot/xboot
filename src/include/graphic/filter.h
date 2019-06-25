#ifndef __GRAPHIC_FILTER_H__
#define __GRAPHIC_FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/surface.h>

void filter_soft_grayscale(struct surface_t * s);
void filter_soft_sepia(struct surface_t * s);
void filter_soft_invert(struct surface_t * s);
void filter_soft_threshold(struct surface_t * s, const char * type, int threshold, int value);
void filter_soft_colorize(struct surface_t * s, const char * type);
void filter_soft_gamma(struct surface_t * s, double gamma);
void filter_soft_hue(struct surface_t * s, int angle);
void filter_soft_saturate(struct surface_t * s, int saturate);
void filter_soft_brightness(struct surface_t * s, int brightness);
void filter_soft_contrast(struct surface_t * s, int contrast);
void filter_soft_blur(struct surface_t * s, int radius);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FILTER_H__ */
