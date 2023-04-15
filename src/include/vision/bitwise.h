#ifndef __VISION_BITWISE_H__
#define __VISION_BITWISE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vision/vision.h>

void vision_bitwise_and(struct vision_t * v, struct vision_t * o);
void vision_bitwise_or(struct vision_t * v, struct vision_t * o);
void vision_bitwise_xor(struct vision_t * v, struct vision_t * o);
void vision_bitwise_not(struct vision_t * v);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_BITWISE_H__ */
