/*
 * libx/matrix.c
 */

#include <math.h>
#include <string.h>
#include <matrix.h>

void matrix_init(struct matrix_t * m,
		double a, double b,
		double c, double d,
		double tx, double ty)
{
	m->a = a; m->b = b;
	m->c = c; m->d = d;
	m->tx = tx; m->ty = ty;
}

void matrix_init_identity(struct matrix_t * m)
{
	m->a = 1; m->b = 0;
	m->c = 0; m->d = 1;
	m->tx = 0; m->ty = 0;
}

void matrix_init_translate(struct matrix_t * m,
		double tx, double ty)
{
	m->a = 1;  m->b = 0;
	m->c = 0;  m->d = 1;
	m->tx = tx; m->ty = ty;
}

void matrix_init_scale(struct matrix_t * m,
		double sx, double sy)
{
	m->a = sx; m->b = 0;
	m->c = 0;  m->d = sy;
	m->tx = 0;  m->ty = 0;
}

void matrix_init_rotate(struct matrix_t * m, double r)
{
	double s = sin(r);
	double c = cos(r);

	m->a = c;  m->b = s;
	m->c = -s; m->d = c;
	m->tx = 0;  m->ty = 0;
}

/*
 * | [m->a]  [m->b]  [0] |   | [m1->a]  [m1->b]  [0] |   | [m2->a]  [m2->b]  [0] |
 * | [m->c]  [m->d]  [0] | = | [m1->c]  [m1->d]  [0] | x | [m2->c]  [m2->d]  [0] |
 * | [m->tx] [m->ty] [1] |   | [m1->tx] [m1->ty] [1] |   | [m2->tx] [m2->ty] [1] |
 */
void matrix_multiply(struct matrix_t * m,
		struct matrix_t * m1,
		struct matrix_t * m2)
{
	struct matrix_t t;

	t.a = m1->a * m2->a;
	t.b = 0.0;
	t.c = 0.0;
	t.d = m1->d * m2->d;
	t.tx = m1->tx * m2->a + m2->tx;
	t.ty = m1->ty * m2->d + m2->ty;

	if(m1->b != 0.0 || m1->c != 0.0 || m2->b != 0.0 || m2->c != 0.0)
	{
		t.a += m1->b * m2->c;
		t.b += m1->a * m2->b + m1->b * m2->d;
		t.c += m1->c * m2->a + m1->d * m2->c;
		t.d += m1->c * m2->b;
		t.tx += m1->ty * m2->c;
		t.ty += m1->tx * m2->b;
	}
	memcpy(m, &t, sizeof(struct matrix_t));
}

void matrix_invert(struct matrix_t * m)
{
	double a, b, c, d, tx, ty;
	double det;

	if((m->c == 0.0) && (m->b == 0.0))
	{
		m->tx = -m->tx;
		m->ty = -m->ty;
		if(m->a != 1.0)
		{
			if(m->a == 0.0)
				return;
			m->a = 1.0 / m->a;
			m->tx *= m->a;
		}
		if(m->d != 1.0)
		{
			if(m->d == 0.0)
				return;
			m->d = 1.0 / m->d;
			m->ty *= m->d;
		}
	}
	else
	{
		det = m->a * m->d - m->b * m->c;
		if(det != 0.0)
		{
			a  = m->a;
			b  = m->b;
			c  = m->c;
			d  = m->d;
			tx = m->tx;
			ty = m->ty;
			m->a = d / det;
			m->b = -b / det;
			m->c = -c / det;
			m->d = a / det;
			m->tx = (c * ty - d * tx) / det;
			m->ty = (b * tx - a * ty) / det;
		}
	}
}

void matrix_translate(struct matrix_t * m, double tx, double ty)
{
	m->tx += m->a * tx + m->c * ty;
	m->ty += m->b * tx + m->d * ty;
}

void matrix_scale(struct matrix_t * m, double sx, double sy)
{
	m->a *= sx;
	m->b *= sx;
	m->c *= sy;
	m->d *= sy;
}

void matrix_rotate(struct matrix_t * m, double r)
{
	double s = sin(r);
	double c = cos(r);
	double ca = c * m->a;
	double cb = c * m->b;
	double cc = c * m->c;
	double cd = c * m->d;
	double sa = s * m->a;
	double sb = s * m->b;
	double sc = s * m->c;
	double sd = s * m->d;

	m->a = ca + sc;
	m->b = cb + sd;
	m->c = cc - sa;
	m->d = cd - sb;
}

void matrix_transform_distance(struct matrix_t * m, double * dx, double * dy)
{
	double nx = m->a * (*dx) + m->c * (*dy);
	double ny = m->b * (*dx) + m->d * (*dy);
	*dx = nx;
	*dy = ny;
}

void matrix_transform_point(struct matrix_t * m, double * x, double * y)
{
	double nx = m->a * (*x) + m->c * (*y) + m->tx;
	double ny = m->b * (*x) + m->d * (*y) + m->ty;
	*x = nx;
	*y = ny;
}

void matrix_transform_bounds(struct matrix_t * m, double * x1, double * y1, double * x2, double * y2)
{
	double qx[4], qy[4];
	double minx, maxx;
	double miny, maxy;
	int i;

	if((m->c == 0.0) && (m->b == 0.0))
	{
		if(m->a != 1.0)
		{
			qx[0] = (*x1) * m->a;
			qx[1] = (*x2) * m->a;
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
		if(m->tx != 0.0)
		{
			*x1 += m->tx;
			*x2 += m->tx;
		}
		if(m->d != 1.0)
		{
			qy[0] = (*y1) * m->d;
			qy[1] = (*y2) * m->d;
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
		if(m->ty != 0.0)
		{
			*y1 += m->ty;
			*y2 += m->ty;
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
