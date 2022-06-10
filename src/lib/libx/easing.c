/*
 * libx/easing.c
 *
 * https://cubic-bezier.com
 * https://easings.co
 */

#include <math.h>
#include <easing.h>

void cubic_bezier_init(struct cubic_bezier_t * b, double x1, double y1, double x2, double y2)
{
	b->cx = 3.0 * x1;
	b->bx = 3.0 * (x2 - x1) - b->cx;
	b->ax = 1.0 - b->cx - b->bx;
	b->cy = 3.0 * y1;
	b->by = 3.0 * (y2 - y1) - b->cy;
	b->ay = 1.0 - b->cy - b->by;
}

static inline double sample_curve_x(struct cubic_bezier_t * b, double t)
{
	return ((b->ax * t + b->bx) * t + b->cx) * t;
}

static inline double sample_curve_y(struct cubic_bezier_t * b, double t)
{
	return ((b->ay * t + b->by) * t + b->cy) * t;
}

static inline double sample_curve_derivative(struct cubic_bezier_t * b, double t)
{
	return (3 * b->ax * t + 2 * b->bx) * t + b->cx;
}

static inline double solve_curve_x(struct cubic_bezier_t * b, double t)
{
	double t1 = t;
	for(int i = 0; i < 8; i++)
	{
		double err = sample_curve_x(b, t1) - t;
		if(fabs(err) < 1e-6)
			return t1;
		double dx = sample_curve_derivative(b, t1);
		if(fabs(dx) < 1e-6)
			break;
		t1 -= err / dx;
	}
	double lower = 0.0;
	double upper = 1.0;
	double t2 = t;
	while(lower < upper)
	{
		double v = sample_curve_x(b, t2);
		if(fabs(v - t) < 1e-6)
			return t2;
		if(t > v)
			lower = t2;
		else
			upper = t2;
		t2 = (lower + upper) / 2;
	}
	return t2;
}

double cubic_bezier_calc(struct cubic_bezier_t * b, double t)
{
	if(t <= 0.0)
		return 0.0;
	else if(t >= 1.0)
		return 1.0;
	else
		return sample_curve_y(b, solve_curve_x(b, t));
}

void easing_init(struct easing_t * e, double start, double stop, double duration, double x1, double y1, double x2, double y2)
{
	cubic_bezier_init(&e->bezier, x1, y1, x2, y2);
	e->start = start;
	e->stop = stop;
	e->duration = duration;
	e->acc = 0.0;
}

double easing_calc(struct easing_t * e, double t)
{
	return (e->stop - e->start) * cubic_bezier_calc(&e->bezier, t / e->duration) + e->start;
}

double easing_step(struct easing_t * e, double dt)
{
	if(e->acc < e->duration)
		e->acc += dt;
	return easing_calc(e, e->acc);
}

int easing_finished(struct easing_t * e)
{
	return (e->acc < e->duration) ? 0 : 1;
}
