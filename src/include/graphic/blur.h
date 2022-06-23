#ifndef __GRAPHIC_BLUR_H__
#define __GRAPHIC_BLUR_H__

#ifdef __cplusplus
extern "C" {
#endif

void blur(unsigned char * pixel, int width, int height, int x, int y, int w, int h, int radius);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_BLUR_H__ */
