#ifndef __G2D_H__
#define __G2D_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct g2d_t
{
	char * name;

	bool_t (*blit)(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o);
	bool_t (*fill)(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c);
	void * priv;
};

struct g2d_t * search_g2d(const char * name);
struct g2d_t * search_first_g2d(void);
struct device_t * register_g2d(struct g2d_t * g2d, struct driver_t * drv);
void unregister_g2d(struct g2d_t * g2d);

#ifdef __cplusplus
}
#endif

#endif /* __G2D_H__ */
