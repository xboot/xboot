#ifndef __LOGO_H__
#define __LOGO_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

/*
 * the logo, gimp's C source format.
 */
struct logo_image {
	x_u32 width;
	x_u32 height;
	x_u32 bytes_per_pixel;
	x_u8  * pixel_data;
};

x_bool display_logo(struct fb * fb);
x_bool register_logo(const struct logo_image * logo);

#endif /* __LOGO_H__ */
