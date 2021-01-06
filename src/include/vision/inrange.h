#ifndef __VISION_INRANGE_H__
#define __VISION_INRANGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vision/vision.h>

struct vision_t * vision_inrange(struct vision_t * v, float * l, float * h);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_INRANGE_H__ */
