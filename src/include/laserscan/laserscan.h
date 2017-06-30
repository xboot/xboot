#ifndef __LASERSCAN_H__
#define __LASERSCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct laserscan_t
{
	char * name;

	void (*perspective)(struct laserscan_t * l, float x, float y);
	void (*translate)(struct laserscan_t * l, float x, float y);
	void (*scale)(struct laserscan_t * l, float x, float y);
	void (*shear)(struct laserscan_t * l, float x, float y);

	void (*set_color)(struct laserscan_t * l, u8_t r, u8_t g, u8_t b, u8_t a);
	void (*get_color)(struct laserscan_t * l, u8_t * r, u8_t * g, u8_t * b, u8_t * a);
	void (*move_to)(struct laserscan_t * l, float x, float y);
	void (*rel_move_to)(struct laserscan_t * l, float dx, float dy);
	void (*line_to)(struct laserscan_t * l, float x, float y);
	void (*rel_line_to)(struct laserscan_t * l, float dx, float dy);
	void (*curve_to)(struct laserscan_t * l, float x1, float y1, float x2, float y2, float x3, float y3);
	void (*rel_curve_to)(struct laserscan_t * l, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3);
	void (*arc)(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2);
	void (*arc_negative)(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2);
	void (*clear)(struct laserscan_t * l);

	void * priv;
};

struct laserscan_t * search_laserscan(const char * name);
struct laserscan_t * search_first_laserscan(void);
bool_t register_laserscan(struct device_t ** device,struct laserscan_t * l);
bool_t unregister_laserscan(struct laserscan_t * l);

void laserscan_perspective(struct laserscan_t * l, float x, float y);
void laserscan_translate(struct laserscan_t * l, float x, float y);
void laserscan_scale(struct laserscan_t * l, float x, float y);
void laserscan_shear(struct laserscan_t * l, float x, float y);

void laserscan_set_color(struct laserscan_t * l, u8_t r, u8_t g, u8_t b, u8_t a);
void laserscan_get_color(struct laserscan_t * l, u8_t * r, u8_t * g, u8_t * b, u8_t * a);
void laserscan_move_to(struct laserscan_t * l, float x, float y);
void laserscan_rel_move_to(struct laserscan_t * l, float dx, float dy);
void laserscan_line_to(struct laserscan_t * l, float x, float y);
void laserscan_rel_line_to(struct laserscan_t * l, float dx, float dy);
void laserscan_curve_to(struct laserscan_t * l, float x1, float y1, float x2, float y2, float x3, float y3);
void laserscan_rel_curve_to(struct laserscan_t * l, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3);
void laserscan_arc(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2);
void laserscan_arc_negative(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2);
void laserscan_rectangle(struct laserscan_t * l, float x, float y, float w, float h);
void laserscan_seekbar(struct laserscan_t * l, float x, float y, float w, float h, float v);

void laserscan_clear(struct laserscan_t * l);
void laserscan_load_ilda(struct laserscan_t * l, const char * file);

#ifdef __cplusplus
}
#endif

#endif /* __LASERSCAN_H__ */
