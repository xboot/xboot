/*
 * libx/matrix.c
 */

#include <math.h>
#include <string.h>
#include <matrix.h>

void matrix_init(struct matrix_t * m,
		double xx, double yx,
		double xy, double yy,
		double x0, double y0)
{
	m->xx = xx; m->yx = yx;
	m->xy = xy; m->yy = yy;
	m->x0 = x0; m->y0 = y0;
}

void matrix_init_identity(struct matrix_t * m)
{
	m->xx = 1; m->yx = 0;
	m->xy = 0; m->yy = 1;
	m->x0 = 0; m->y0 = 0;
}

void matrix_init_translate(struct matrix_t * m,
		double tx, double ty)
{
	m->xx = 1;  m->yx = 0;
	m->xy = 0;  m->yy = 1;
	m->x0 = tx; m->y0 = ty;
}

void matrix_init_scale(struct matrix_t * m,
		double sx, double sy)
{
	m->xx = sx; m->yx = 0;
	m->xy = 0;  m->yy = sy;
	m->x0 = 0;  m->y0 = 0;
}

void matrix_init_rotate(struct matrix_t * m, double r)
{
	double s = sin(r);
	double c = cos(r);

	m->xx = c;  m->yx = s;
	m->xy = -s; m->yy = c;
	m->x0 = 0;  m->y0 = 0;
}

/*
 * | [m->xx] [m->yx] [0] |   | [m1->xx] [m1->yx] [0] |   | [m2->xx] [m2->yx] [0] |
 * | [m->xy] [m->yy] [0] | = | [m1->xy] [m1->yy] [0] | x | [m2->xy] [m2->yy] [0] |
 * | [m->x0] [m->y0] [1] |   | [m1->x0] [m1->y0] [1] |   | [m2->x0] [m2->y0] [1] |
 */
void matrix_multiply(struct matrix_t * m,
		const struct matrix_t * m1,
		const struct matrix_t * m2)
{
	struct matrix_t t;

	t.xx = m1->xx * m2->xx + m1->yx * m2->xy;
	t.yx = m1->xx * m2->yx + m1->yx * m2->yy;

	t.xy = m1->xy * m2->xx + m1->yy * m2->xy;
	t.yy = m1->xy * m2->yx + m1->yy * m2->yy;

	t.x0 = m1->x0 * m2->xx + m1->y0 * m2->xy + m2->x0;
	t.y0 = m1->x0 * m2->yx + m1->y0 * m2->yy + m2->y0;

	memcpy(m, &t, sizeof(struct matrix_t));
}

void matrix_invert(struct matrix_t * m)
{
	double a, b, c, d, tx, ty;
	double det;

	if((m->xy == 0.0) && (m->yx == 0.0))
	{
		m->x0 = -m->x0;
		m->y0 = -m->y0;
		if(m->xx != 1.0)
		{
			if(m->xx == 0.0)
				return;
			m->xx = 1.0 / m->xx;
			m->x0 *= m->xx;
		}
		if(m->yy != 1.0)
		{
			if(m->yy == 0.0)
				return;
			m->yy = 1.0 / m->yy;
			m->y0 *= m->yy;
		}
	}
	else
	{
		det = m->xx * m->yy - m->yx * m->xy;
		if(det != 0.0)
		{
			a  = m->xx;
			b  = m->yx;
			c  = m->xy;
			d  = m->yy;
			tx = m->x0;
			ty = m->y0;
			m->xx = d / det;
			m->yx = -b / det;
			m->xy = -c / det;
			m->yy = a / det;
			m->x0 = (c * ty - d * tx) / det;
			m->y0 = (b * tx - a * ty) / det;
		}
	}
}

void matrix_translate(struct matrix_t * m, double tx, double ty)
{
	struct matrix_t t;

	matrix_init_translate(&t, tx, ty);
	matrix_multiply(m, &t, m);
}

void matrix_scale(struct matrix_t * m, double sx, double sy)
{
	struct matrix_t t;

	matrix_init_scale(&t, sx, sy);
	matrix_multiply(m, &t, m);
}

void matrix_rotate(struct matrix_t * m, double r)
{
	struct matrix_t t;

	matrix_init_rotate(&t, r);
	matrix_multiply(m, &t, m);
}

void matrix_transform_distance(const struct matrix_t * m, double * dx, double * dy)
{
	double nx, ny;

	nx = (m->xx * (*dx) + m->xy * (*dy));
	ny = (m->yx * (*dx) + m->yy * (*dy));
	*dx = nx;
	*dy = ny;
}

void matrix_transform_point(const struct matrix_t * m, double * x, double * y)
{
	matrix_transform_distance(m, x, y);
	*x += m->x0;
	*y += m->y0;
}

void matrix_transform_bounds(const struct matrix_t * m, double * x1, double * y1, double * x2, double * y2)
{
	double qx[4], qy[4];
	double minx, maxx;
	double miny, maxy;
	int i;

	if((m->xy == 0.0) && (m->yx == 0.0))
	{
		if(m->xx != 1.0)
		{
			qx[0] = (*x1) * m->xx;
			qx[1] = (*x2) * m->xx;
			if(qx[0] < qx[1])
			{
				*x1 = qx[0];
				*x2 = qx[1];
			}
			else
			{
				*x1 = qx[1];
				*x2 = qx[0];
			}
		}
		if(m->x0 != 0.0)
		{
			*x1 += m->x0;
			*x2 += m->x0;
		}
		if(m->yy != 1.0)
		{
			qy[0] = (*y1) * m->yy;
			qy[1] = (*y2) * m->yy;
			if(qy[0] < qy[1])
			{
				*y1 = qy[0];
				*y2 = qy[1];
			}
			else
			{
				*y1 = qy[1];
				*y2 = qy[0];
			}
		}
		if(m->y0 != 0.0)
		{
			*y1 += m->y0;
			*y2 += m->y0;
		}
		return;
	}
	else
	{
		qx[0] = *x1;
		qy[0] = *y1;
		matrix_transform_point(m, &qx[0], &qy[0]);

		qx[1] = *x2;
		qy[1] = *y1;
		matrix_transform_point(m, &qx[1], &qy[1]);

		qx[2] = *x1;
		qy[2] = *y2;
		matrix_transform_point(m, &qx[2], &qy[2]);

		qx[3] = *x2;
		qy[3] = *y2;
		matrix_transform_point(m, &qx[3], &qy[3]);

		minx = maxx = qx[0];
		miny = maxy = qy[0];

		for(i = 1; i < 4; i++)
		{
			if(qx[i] < minx)
				minx = qx[i];
			if(qx[i] > maxx)
				maxx = qx[i];
			if(qy[i] < miny)
				miny = qy[i];
			if(qy[i] > maxy)
				maxy = qy[i];
		}
		*x1 = minx;
		*y1 = miny;
		*x2 = maxx;
		*y2 = maxy;
	}
}
