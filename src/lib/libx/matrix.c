/*
 * libx/matrix.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot/module.h>
#include <matrix.h>

void matrix_transform_init(struct matrix_transform_t * m,
		float a, float b, float x,
		float c, float d, float y)
{
	m->a = a;	m->b = b;	m->x = x;
	m->a = c;	m->b = d;	m->x = y;
}
EXPORT_SYMBOL(matrix_transform_init);

void matrix_transform_init_identity(struct matrix_transform_t * m)
{
	m->a = 1;	m->b = 0;	m->x = 0;
	m->a = 0;	m->b = 1;	m->x = 0;
}
EXPORT_SYMBOL(matrix_transform_init_identity);

void matrix_transform_init_translate(struct matrix_transform_t * m,
		float tx, float ty)
{
	m->a = 1;	m->b = 0;	m->x = tx;
	m->a = 0;	m->b = 1;	m->x = ty;
}
EXPORT_SYMBOL(matrix_transform_init_translate);

void matrix_transform_init_scale(struct matrix_transform_t * m,
		float sx, float sy)
{
	m->a = sx;	m->b = 0;	m->x = 0;
	m->a = 0;	m->b = sy;	m->x = 0;
}
EXPORT_SYMBOL(matrix_transform_init_scale);

void matrix_transform_init_rotate(struct matrix_transform_t * m,
		float c, float s)
{
	m->a = c;	m->b = -s;	m->x = 0;
	m->a = s;	m->b = c;	m->x = 0;
}
EXPORT_SYMBOL(matrix_transform_init_rotate);

void matrix_transform_init_shear(struct matrix_transform_t * m,
		float x, float y)
{
	m->a = 1;	m->b = x;	m->x = 0;
	m->a = y;	m->b = 1;	m->x = 0;
}
EXPORT_SYMBOL(matrix_transform_init_shear);

/*
 * | [m->a] [m->b] [m->x] |   | [m1->a] [m1->b] [m1->x] |   | [m2->a] [m2->b] [m2->x] |
 * | [m->c] [m->d] [m->y] | = | [m1->c] [m1->d] [m1->y] | x | [m2->c] [m2->d] [m2->y] |
 * | [0]    [0]    [1]    |   | [0]     [0]     [1]     |   | [0]     [0]     [1]     |
 */
void matrix_transform_multiply(struct matrix_transform_t * m,
		const struct matrix_transform_t * m1,
		const struct matrix_transform_t * m2)
{
	struct matrix_transform_t t;

	t.a = m1->a * m2->a + m1->b * m2->c;
	t.b = m1->a * m2->b + m1->b * m2->d;
	t.x = m1->a * m2->x + m1->b * m2->y + m1->x;

	t.c = m1->c * m2->a + m1->d * m2->c;
	t.d = m1->c * m2->b + m1->d * m2->d;
	t.y = m1->c * m2->x + m1->d * m2->y + m1->y;

	memcpy(m, &t, sizeof(struct matrix_transform_t));
}
EXPORT_SYMBOL(matrix_transform_multiply);

void matrix_transform_invert(struct matrix_transform_t * m, const struct matrix_transform_t * i)
{
	struct matrix_transform_t t;
	float det;

	det = i->a * i->d - i->b * i->c;

	if(det == 0.0)
	{
		matrix_transform_init_identity(m);
		return;
	}

	t.a = i->d / det;
	t.b = -i->b / det;
	t.c = -i->c / det;
	t.d = i->a / det;
	t.x = (i->c * i->y - i->d * i->x) / det;
	t.y = (i->b * i->x - i->a * i->y) / det;

	memcpy(m, &t, sizeof(struct matrix_transform_t));
}
EXPORT_SYMBOL(matrix_transform_invert);

void matrix_transform_translate(struct matrix_transform_t * m, float tx, float ty)
{
	struct matrix_transform_t t;

	matrix_transform_init_translate(&t, tx, ty);
	matrix_transform_multiply(m, &t, m);
}
EXPORT_SYMBOL(matrix_transform_translate);

void matrix_transform_scale(struct matrix_transform_t * m, float sx, float sy)
{
	struct matrix_transform_t t;

	matrix_transform_init_scale(&t, sx, sy);
	matrix_transform_multiply(m, &t, m);
}
EXPORT_SYMBOL(matrix_transform_scale);

void matrix_transform_rotate(struct matrix_transform_t * m, float c, float s)
{
	struct matrix_transform_t t;

	matrix_transform_init_rotate(&t, c, s);
	matrix_transform_multiply(m, &t, m);
}
EXPORT_SYMBOL(matrix_transform_rotate);

void matrix_transform_shear(struct matrix_transform_t * m, float x, float y)
{
	struct matrix_transform_t t;

	matrix_transform_init_shear(&t, x, y);
	matrix_transform_multiply(m, &t, m);
}
EXPORT_SYMBOL(matrix_transform_shear);

void matrix_transform_distance(const struct matrix_transform_t * m, float * dx, float * dy)
{
	float nx, ny;

	nx = m->a * (*dx) + m->b * (*dy);
	ny = m->c * (*dx) + m->d * (*dy);

	*dx = nx;
	*dy = ny;
}
EXPORT_SYMBOL(matrix_transform_distance);

void matrix_transform_point(const struct matrix_transform_t * m, float * x, float * y)
{
	matrix_transform_distance(m, x, y);

	*x += m->x;
	*y += m->y;
}
EXPORT_SYMBOL(matrix_transform_point);
