#ifndef __VISION_THRESHOLD_H__
#define __VISION_THRESHOLD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vision/vision.h>

void vision_threshold(struct vision_t * v, int threshold, const char * type);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_THRESHOLD_H__ */
