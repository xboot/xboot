#ifndef __VISION_INRANGE_H__
#define __VISION_INRANGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vision/vision.h>

struct vision_t * vision_inrange_gray(struct vision_t * v, unsigned char l, unsigned char h);
struct vision_t * vision_inrange_rgb(struct vision_t * v, unsigned char * lrgb, unsigned char * hrgb);
struct vision_t * vision_inrange_hsv(struct vision_t * v, float * lhsv, float * hhsv);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_INRANGE_H__ */
