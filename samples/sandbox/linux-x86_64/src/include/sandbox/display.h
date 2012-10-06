#ifndef __SANDBOX_DISPLAY_H__
#define __SANDBOX_DISPLAY_H__

struct display_t {
	int width;
	int height;
	char bits_per_pixel;
	char bytes_per_pixel;
	char red_mask_size;
	char red_field_pos;
	char green_mask_size;
	char green_field_pos;
	char blue_mask_size;
	char blue_field_pos;
	char alpha_mask_size;
	char alpha_field_pos;
	void * pixels;

	void (*lock)(struct display_t * display);
	void (*unlock)(struct display_t * display);
	void (*flip)(struct display_t * display);
	
	void * data;
};

struct display_t * display_alloc(void);
void display_free(struct display_t * display);

#endif /* __SANDBOX_DISPLAY_H__ */
