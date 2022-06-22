#ifndef __EASING_H__
#define __EASING_H__

#ifdef __cplusplus
extern "C" {
#endif

struct bezier_t {
	double ax, bx, cx;
	double ay, by, cy;
};

void bezier_init(struct bezier_t * b, double x1, double y1, double x2, double y2);
double bezier_calc(struct bezier_t * b, double t);

struct easing_t {
	struct bezier_t bezier;
	double start;
	double stop;
	double duration;
	double acc;
};

void easing_init(struct easing_t * e, double start, double stop, double duration, double x1, double y1, double x2, double y2);
double easing_calc(struct easing_t * e, double t);
double easing_step(struct easing_t * e, double dt);
int easing_finished(struct easing_t * e);

#ifdef __cplusplus
}
#endif

#endif /* __EASING_H__ */
