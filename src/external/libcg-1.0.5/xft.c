/*
 * xft.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xft.h>

/*
 * type
 */
typedef int64_t			XCG_FT_Int64;
typedef uint64_t		XCG_FT_UInt64;
typedef int32_t			XCG_FT_Int32;
typedef uint32_t		XCG_FT_UInt32;

#define XCG_FT_BOOL(x)	((XCG_FT_Bool)(x))
#ifndef TRUE
#define TRUE			1
#endif
#ifndef FALSE
#define FALSE			0
#endif

/*
 * math
 */
#define XCG_FT_MIN(a, b)		((a) < (b) ? (a) : (b))
#define XCG_FT_MAX(a, b)		((a) > (b) ? (a) : (b))
#define XCG_FT_ABS(a)			((a) < 0 ? -(a) : (a))
#define XCG_FT_HYPOT(x, y)		(x = XCG_FT_ABS(x), y = XCG_FT_ABS(y), x > y ? x + (3 * y >> 3) : y + (3 * x >> 3))
#define XCG_FT_ANGLE_PI			(180L << 16)
#define XCG_FT_ANGLE_2PI		(XCG_FT_ANGLE_PI * 2)
#define XCG_FT_ANGLE_PI2		(XCG_FT_ANGLE_PI / 2)
#define XCG_FT_ANGLE_PI4		(XCG_FT_ANGLE_PI / 4)

#define XCG_FT_MSB(x)			(31 - __builtin_clz(x))
#define XCG_FT_PAD_FLOOR(x, n)	((x) & ~((n)-1))
#define XCG_FT_PAD_ROUND(x, n)	XCG_FT_PAD_FLOOR((x) + ((n) / 2), n)
#define XCG_FT_PAD_CEIL(x, n)	XCG_FT_PAD_FLOOR((x) + ((n) - 1), n)

#define XCG_FT_MOVE_SIGN(x, s) \
	do { \
	if(x < 0) { \
			x = -x; \
			s = -s; \
		} \
	} while(0)

XCG_FT_Long XCG_FT_MulFix(XCG_FT_Long a, XCG_FT_Long b)
{
	XCG_FT_Int s = 1;
	XCG_FT_Long c;

	XCG_FT_MOVE_SIGN(a, s);
	XCG_FT_MOVE_SIGN(b, s);
	c = (XCG_FT_Long)(((XCG_FT_Int64)a * b + 0x8000L) >> 16);

	return (s > 0) ? c : -c;
}

XCG_FT_Long XCG_FT_MulDiv(XCG_FT_Long a, XCG_FT_Long b, XCG_FT_Long c)
{
	XCG_FT_Int s = 1;
	XCG_FT_Long d;

	XCG_FT_MOVE_SIGN(a, s);
	XCG_FT_MOVE_SIGN(b, s);
	XCG_FT_MOVE_SIGN(c, s);
	d = (XCG_FT_Long)(c > 0 ? ((XCG_FT_Int64)a * b + (c >> 1)) / c : 0x7FFFFFFFL);
	return (s > 0) ? d : -d;
}

XCG_FT_Long XCG_FT_DivFix(XCG_FT_Long a, XCG_FT_Long b)
{
	XCG_FT_Int s = 1;
	XCG_FT_Long q;

	XCG_FT_MOVE_SIGN(a, s);
	XCG_FT_MOVE_SIGN(b, s);
	q = (XCG_FT_Long)(b > 0 ? (((XCG_FT_UInt64)a << 16) + (b >> 1)) / b : 0x7FFFFFFFL);
	return (s < 0 ? -q : q);
}

#define XCG_FT_TRIG_SCALE 		(0xDBD95B16UL)
#define XCG_FT_TRIG_SAFE_MSB	(29)
#define XCG_FT_TRIG_MAX_ITERS	(23)

static const XCG_FT_Fixed ft_trig_arctan_table[] = {
	1740967L, 919879L, 466945L, 234379L, 117304L, 58666L, 29335L, 14668L,
	7334L,    3667L,   1833L,   917L,    458L,    229L,   115L,   57L,
	29L,      14L,     7L,      4L,      2L,      1L
};

static XCG_FT_Fixed ft_trig_downscale(XCG_FT_Fixed val)
{
	XCG_FT_Fixed s;
	XCG_FT_Int64 v;

	s = val;
	val = XCG_FT_ABS(val);
	v = (val * (XCG_FT_Int64)XCG_FT_TRIG_SCALE) + 0x100000000UL;
	val = (XCG_FT_Fixed)(v >> 32);
	return (s >= 0) ? val : -val;
}

static XCG_FT_Int ft_trig_prenorm(XCG_FT_Vector *vec)
{
	XCG_FT_Pos x, y;
	XCG_FT_Int shift;

	x = vec->x;
	y = vec->y;

	shift = XCG_FT_MSB(XCG_FT_ABS(x) | XCG_FT_ABS(y));

	if(shift <= XCG_FT_TRIG_SAFE_MSB)
	{
		shift = XCG_FT_TRIG_SAFE_MSB - shift;
		vec->x = (XCG_FT_Pos)((XCG_FT_ULong)x << shift);
		vec->y = (XCG_FT_Pos)((XCG_FT_ULong)y << shift);
	}
	else
	{
		shift -= XCG_FT_TRIG_SAFE_MSB;
		vec->x = x >> shift;
		vec->y = y >> shift;
		shift = -shift;
	}
	return shift;
}

static void ft_trig_pseudo_rotate(XCG_FT_Vector *vec, XCG_FT_Angle theta)
{
	XCG_FT_Int i;
	XCG_FT_Fixed x, y, xtemp, b;
	const XCG_FT_Fixed *arctanptr;

	x = vec->x;
	y = vec->y;
	while(theta < -XCG_FT_ANGLE_PI4)
	{
		xtemp = y;
		y = -x;
		x = xtemp;
		theta += XCG_FT_ANGLE_PI2;
	}
	while(theta > XCG_FT_ANGLE_PI4)
	{
		xtemp = -y;
		y = x;
		x = xtemp;
		theta -= XCG_FT_ANGLE_PI2;
	}
	arctanptr = ft_trig_arctan_table;
	for(i = 1, b = 1; i < XCG_FT_TRIG_MAX_ITERS; b <<= 1, i++)
	{
		XCG_FT_Fixed v1 = ((y + b) >> i);
		XCG_FT_Fixed v2 = ((x + b) >> i);
		if(theta < 0)
		{
			xtemp = x + v1;
			y = y - v2;
			x = xtemp;
			theta += *arctanptr++;
		}
		else
		{
			xtemp = x - v1;
			y = y + v2;
			x = xtemp;
			theta -= *arctanptr++;
		}
	}
	vec->x = x;
	vec->y = y;
}

static void ft_trig_pseudo_polarize(XCG_FT_Vector *vec)
{
	XCG_FT_Angle theta;
	XCG_FT_Int i;
	XCG_FT_Fixed x, y, xtemp, b;
	const XCG_FT_Fixed *arctanptr;

	x = vec->x;
	y = vec->y;
	if(y > x)
	{
		if(y > -x)
		{
			theta = XCG_FT_ANGLE_PI2;
			xtemp = y;
			y = -x;
			x = xtemp;
		}
		else
		{
			theta = y > 0 ? XCG_FT_ANGLE_PI : -XCG_FT_ANGLE_PI;
			x = -x;
			y = -y;
		}
	}
	else
	{
		if(y < -x)
		{
			theta = -XCG_FT_ANGLE_PI2;
			xtemp = -y;
			y = x;
			x = xtemp;
		}
		else
		{
			theta = 0;
		}
	}
	arctanptr = ft_trig_arctan_table;
	for(i = 1, b = 1; i < XCG_FT_TRIG_MAX_ITERS; b <<= 1, i++)
	{
		XCG_FT_Fixed v1 = ((y + b) >> i);
		XCG_FT_Fixed v2 = ((x + b) >> i);
		if(y > 0)
		{
			xtemp = x + v1;
			y = y - v2;
			x = xtemp;
			theta += *arctanptr++;
		}
		else
		{
			xtemp = x - v1;
			y = y + v2;
			x = xtemp;
			theta -= *arctanptr++;
		}
	}
	if(theta >= 0)
		theta = XCG_FT_PAD_ROUND(theta, 32);
	else
		theta = -XCG_FT_PAD_ROUND(-theta, 32);
	vec->x = x;
	vec->y = theta;
}

XCG_FT_Fixed XCG_FT_Cos(XCG_FT_Angle angle)
{
	XCG_FT_Vector v;

	v.x = XCG_FT_TRIG_SCALE >> 8;
	v.y = 0;
	ft_trig_pseudo_rotate(&v, angle);
	return (v.x + 0x80L) >> 8;
}

XCG_FT_Fixed XCG_FT_Sin(XCG_FT_Angle angle)
{
	return XCG_FT_Cos(XCG_FT_ANGLE_PI2 - angle);
}

XCG_FT_Fixed XCG_FT_Tan(XCG_FT_Angle angle)
{
	XCG_FT_Vector v;

	v.x = XCG_FT_TRIG_SCALE >> 8;
	v.y = 0;
	ft_trig_pseudo_rotate(&v, angle);
	return XCG_FT_DivFix(v.y, v.x);
}

XCG_FT_Angle XCG_FT_Atan2(XCG_FT_Fixed dx, XCG_FT_Fixed dy)
{
	XCG_FT_Vector v;

	if(dx == 0 && dy == 0)
		return 0;
	v.x = dx;
	v.y = dy;
	ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	return v.y;
}

void XCG_FT_Vector_Unit(XCG_FT_Vector *vec, XCG_FT_Angle angle)
{
	vec->x = XCG_FT_TRIG_SCALE >> 8;
	vec->y = 0;
	ft_trig_pseudo_rotate(vec, angle);
	vec->x = (vec->x + 0x80L) >> 8;
	vec->y = (vec->y + 0x80L) >> 8;
}

void XCG_FT_Vector_Rotate(XCG_FT_Vector *vec, XCG_FT_Angle angle)
{
	XCG_FT_Int shift;
	XCG_FT_Vector v = *vec;

	if(v.x == 0 && v.y == 0)
		return;
	shift = ft_trig_prenorm(&v);
	ft_trig_pseudo_rotate(&v, angle);
	v.x = ft_trig_downscale(v.x);
	v.y = ft_trig_downscale(v.y);

	if(shift > 0)
	{
		XCG_FT_Int32 half = (XCG_FT_Int32)1L << (shift - 1);

		vec->x = (v.x + half - (v.x < 0)) >> shift;
		vec->y = (v.y + half - (v.y < 0)) >> shift;
	}
	else
	{
		shift = -shift;
		vec->x = (XCG_FT_Pos)((XCG_FT_ULong)v.x << shift);
		vec->y = (XCG_FT_Pos)((XCG_FT_ULong)v.y << shift);
	}
}

XCG_FT_Fixed XCG_FT_Vector_Length(XCG_FT_Vector *vec)
{
	XCG_FT_Int shift;
	XCG_FT_Vector v;

	v = *vec;
	if(v.x == 0)
	{
		return XCG_FT_ABS(v.y);
	}
	else if(v.y == 0)
	{
		return XCG_FT_ABS(v.x);
	}

	shift = ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	v.x = ft_trig_downscale(v.x);
	if(shift > 0)
		return (v.x + (1 << (shift - 1))) >> shift;

	return (XCG_FT_Fixed)((XCG_FT_UInt32)v.x << -shift);
}

void XCG_FT_Vector_Polarize(XCG_FT_Vector *vec, XCG_FT_Fixed *length, XCG_FT_Angle *angle)
{
	XCG_FT_Int shift;
	XCG_FT_Vector v;

	v = *vec;
	if(v.x == 0 && v.y == 0)
		return;
	shift = ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	v.x = ft_trig_downscale(v.x);

	*length = (shift >= 0) ? (v.x >> shift) : (XCG_FT_Fixed)((XCG_FT_UInt32)v.x << -shift);
	*angle = v.y;
}

void XCG_FT_Vector_From_Polar(XCG_FT_Vector *vec, XCG_FT_Fixed length, XCG_FT_Angle angle)
{
	vec->x = length;
	vec->y = 0;

	XCG_FT_Vector_Rotate(vec, angle);
}

XCG_FT_Angle XCG_FT_Angle_Diff(XCG_FT_Angle angle1, XCG_FT_Angle angle2)
{
	XCG_FT_Angle delta = angle2 - angle1;

	while(delta <= -XCG_FT_ANGLE_PI)
		delta += XCG_FT_ANGLE_2PI;

	while(delta > XCG_FT_ANGLE_PI)
		delta -= XCG_FT_ANGLE_2PI;

	return delta;
}

/*
 * raster
 */
typedef long			TCoord;
typedef long			TPos;
typedef long			TArea;
typedef ptrdiff_t		XCG_FT_PtrDist;

#define xcg_ft_setjmp	setjmp
#define xcg_ft_longjmp 	longjmp
#define xcg_ft_jmp_buf	jmp_buf

#define ErrRaster_Invalid_Mode      -2
#define ErrRaster_Invalid_Outline   -1
#define ErrRaster_Invalid_Argument  -3
#define ErrRaster_Memory_Overflow   -4
#define ErrRaster_OutOfMemory       -6

#define XCG_FT_MINIMUM_POOL_SIZE 	8192
#define XCG_FT_MAX_GRAY_SPANS		256

#define RAS_ARG   					PWorker worker
#define RAS_ARG_					PWorker worker,
#define RAS_VAR 					worker
#define RAS_VAR_					worker,
#define ras							(*worker)

#define PIXEL_BITS		8
#define ONE_PIXEL		(1L << PIXEL_BITS)
#define TRUNC(x)		(TCoord)((x) >> PIXEL_BITS)
#define FRACT(x)		(TCoord)((x) & (ONE_PIXEL - 1))
#if PIXEL_BITS >= 6
#define UPSCALE(x)		((x) * (ONE_PIXEL >> 6))
#define DOWNSCALE(x)	((x) >> (PIXEL_BITS - 6))
#else
#define UPSCALE(x)		((x) >> (6 - PIXEL_BITS))
#define DOWNSCALE(x)	((x) * (64 >> PIXEL_BITS))
#endif

#define XCG_FT_DIV_MOD(type, dividend, divisor, quotient, remainder) \
	do { \
		(quotient) = (type)((dividend) / (divisor)); \
		(remainder) = (type)((dividend) % (divisor)); \
		if((remainder) < 0) \
		{ \
			(quotient)--; \
			(remainder) += (type)(divisor); \
		} \
	} while(0)

typedef struct TCell_ * PCell;
typedef struct TCell_ {
	int x;
	int cover;
	TArea area;
	PCell next;
} TCell;

typedef struct TWorker_ {
	TCoord ex, ey;
	TPos min_ex, max_ex;
	TPos min_ey, max_ey;
	TPos count_ex, count_ey;
	TArea area;
	int cover;
	int invalid;
	PCell cells;
	XCG_FT_PtrDist max_cells;
	XCG_FT_PtrDist num_cells;
	TPos x, y;
	XCG_FT_Outline outline;
	XCG_FT_BBox clip_box;
	XCG_FT_Span gray_spans[XCG_FT_MAX_GRAY_SPANS];
	int num_gray_spans;
	int skip_spans;
	XCG_FT_Raster_Span_Func render_span;
	void *render_span_data;
	int band_size;
	int band_shoot;
	xcg_ft_jmp_buf jump_buffer;
	void *buffer;
	long buffer_size;
	PCell *ycells;
	TPos ycount;
} TWorker, *PWorker;

static void gray_init_cells(RAS_ARG_ void* buffer, long byte_size)
{
	ras.buffer = buffer;
	ras.buffer_size = byte_size;
	ras.ycells = (PCell*)buffer;
	ras.cells = NULL;
	ras.max_cells = 0;
	ras.num_cells = 0;
	ras.area = 0;
	ras.cover = 0;
	ras.invalid = 1;
}

static void gray_compute_cbox( RAS_ARG)
{
	XCG_FT_Outline *outline = &ras.outline;
	XCG_FT_Vector *vec = outline->points;
	XCG_FT_Vector *limit = vec + outline->n_points;

	if(outline->n_points <= 0)
	{
		ras.min_ex = ras.max_ex = 0;
		ras.min_ey = ras.max_ey = 0;
		return;
	}
	ras.min_ex = ras.max_ex = vec->x;
	ras.min_ey = ras.max_ey = vec->y;
	vec++;
	for(; vec < limit; vec++)
	{
		TPos x = vec->x;
		TPos y = vec->y;
		if(x < ras.min_ex)
			ras.min_ex = x;
		if(x > ras.max_ex)
			ras.max_ex = x;
		if(y < ras.min_ey)
			ras.min_ey = y;
		if(y > ras.max_ey)
			ras.max_ey = y;
	}
	ras.min_ex = ras.min_ex >> 6;
	ras.min_ey = ras.min_ey >> 6;
	ras.max_ex = (ras.max_ex + 63) >> 6;
	ras.max_ey = (ras.max_ey + 63) >> 6;
}

static PCell gray_find_cell(RAS_ARG)
{
	PCell *pcell, cell;
	TPos x = ras.ex;

	if(x > ras.count_ex)
		x = ras.count_ex;
	pcell = &ras.ycells[ras.ey];
	for(;;)
	{
		cell = *pcell;
		if(cell == NULL || cell->x > x)
			break;
		if(cell->x == x)
			goto Exit;
		pcell = &cell->next;
	}
	if(ras.num_cells >= ras.max_cells)
		xcg_ft_longjmp(ras.jump_buffer, 1);
	cell = ras.cells + ras.num_cells++;
	cell->x = x;
	cell->area = 0;
	cell->cover = 0;
	cell->next = *pcell;
	*pcell = cell;
Exit:
	return cell;
}

static void gray_record_cell( RAS_ARG)
{
	if( ras.area | ras.cover)
	{
		PCell cell = gray_find_cell( RAS_VAR);
		cell->area += ras.area;
		cell->cover += ras.cover;
	}
}

static void gray_set_cell( RAS_ARG_ TCoord ex, TCoord ey)
{
	ey -= ras.min_ey;
	if(ex > ras.max_ex)
		ex = ras.max_ex;
	ex -= ras.min_ex;
	if(ex < 0)
		ex = -1;
	if(ex != ras.ex || ey != ras.ey)
	{
		if(!ras.invalid)
			gray_record_cell( RAS_VAR);
		ras.area = 0;
		ras.cover = 0;
		ras.ex = ex;
		ras.ey = ey;
	}
	ras.invalid = ((unsigned int)ey >= (unsigned int)ras.count_ey || ex >= ras.count_ex);
}

static void gray_start_cell( RAS_ARG_ TCoord ex, TCoord ey)
{
	if(ex > ras.max_ex)
		ex = (TCoord)( ras.max_ex);
	if(ex < ras.min_ex)
		ex = (TCoord)( ras.min_ex - 1);
	ras.area = 0;
	ras.cover = 0;
	ras.ex = ex - ras.min_ex;
	ras.ey = ey - ras.min_ey;
	ras.invalid = 0;
	gray_set_cell( RAS_VAR_ ex, ey);
}

static void gray_render_scanline( RAS_ARG_ TCoord ey, TPos x1, TCoord y1, TPos x2, TCoord y2)
{
	TCoord ex1, ex2, fx1, fx2, first, dy, delta, mod;
	TPos p, dx;
	int incr;

	ex1 = TRUNC(x1);
	ex2 = TRUNC(x2);
	if(y1 == y2)
	{
		gray_set_cell( RAS_VAR_ ex2, ey);
		return;
	}
	fx1 = FRACT(x1);
	fx2 = FRACT(x2);
	if(ex1 == ex2)
		goto End;
	dx = x2 - x1;
	dy = y2 - y1;
	if(dx > 0)
	{
		p = ( ONE_PIXEL - fx1) * dy;
		first = ONE_PIXEL;
		incr = 1;
	}
	else
	{
		p = fx1 * dy;
		first = 0;
		incr = -1;
		dx = -dx;
	}
	XCG_FT_DIV_MOD(TCoord, p, dx, delta, mod);
	ras.area += (TArea)(fx1 + first) * delta;
	ras.cover += delta;
	y1 += delta;
	ex1 += incr;
	gray_set_cell( RAS_VAR_ ex1, ey);
	if(ex1 != ex2)
	{
		TCoord lift, rem;
		p = ONE_PIXEL * dy;
		XCG_FT_DIV_MOD(TCoord, p, dx, lift, rem);
		do {
			delta = lift;
			mod += rem;
			if(mod >= (TCoord)dx)
			{
				mod -= (TCoord)dx;
				delta++;
			}
			ras.area += (TArea)( ONE_PIXEL * delta);
			ras.cover += delta;
			y1 += delta;
			ex1 += incr;
			gray_set_cell( RAS_VAR_ ex1, ey);
		} while(ex1 != ex2);
	}
	fx1 = ONE_PIXEL - first;
End:
	dy = y2 - y1;
	ras.area += (TArea)((fx1 + fx2) * dy);
	ras.cover += dy;
}

static void gray_render_line( RAS_ARG_ TPos to_x, TPos to_y)
{
	TCoord ey1, ey2, fy1, fy2, first, delta, mod;
	TPos p, dx, dy, x, x2;
	int incr;

	ey1 = TRUNC(ras.y);
	ey2 = TRUNC(to_y);
	if((ey1 >= ras.max_ey && ey2 >= ras.max_ey) || (ey1 < ras.min_ey && ey2 < ras.min_ey))
		goto End;
	fy1 = FRACT(ras.y);
	fy2 = FRACT(to_y);
	if(ey1 == ey2)
	{
		gray_render_scanline( RAS_VAR_ ey1, ras.x, fy1, to_x, fy2);
		goto End;
	}
	dx = to_x - ras.x;
	dy = to_y - ras.y;
	if(dx == 0)
	{
		TCoord ex = TRUNC(ras.x);
		TCoord two_fx = FRACT( ras.x ) << 1;
		TPos area, max_ey1;

		if(dy > 0)
		{
			first = ONE_PIXEL;
		}
		else
		{
			first = 0;
		}
		delta = first - fy1;
		ras.area += (TArea)two_fx * delta;
		ras.cover += delta;
		delta = first + first - ONE_PIXEL;
		area = (TArea)two_fx * delta;
		max_ey1 = ras.count_ey + ras.min_ey;
		if(dy < 0)
		{
			if(ey1 > max_ey1)
			{
				ey1 = (max_ey1 > ey2) ? max_ey1 : ey2;
				gray_set_cell(&ras, ex, ey1);
			}
			else
			{
				ey1--;
				gray_set_cell(&ras, ex, ey1);
			}
			while(ey1 > ey2 && ey1 >= ras.min_ey)
			{
				ras.area += area;
				ras.cover += delta;
				ey1--;

				gray_set_cell(&ras, ex, ey1);
			}
			if(ey1 != ey2)
			{
				ey1 = ey2;
				gray_set_cell(&ras, ex, ey1);
			}
		}
		else
		{
			if(ey1 < ras.min_ey)
			{
				ey1 = (ras.min_ey < ey2) ? ras.min_ey : ey2;
				gray_set_cell(&ras, ex, ey1);
			}
			else
			{
				ey1++;
				gray_set_cell(&ras, ex, ey1);
			}
			while(ey1 < ey2 && ey1 < max_ey1)
			{
				ras.area += area;
				ras.cover += delta;
				ey1++;

				gray_set_cell(&ras, ex, ey1);
			}
			if(ey1 != ey2)
			{
				ey1 = ey2;
				gray_set_cell(&ras, ex, ey1);
			}
		}
		delta = (int)(fy2 - ONE_PIXEL + first);
		ras.area += (TArea)two_fx * delta;
		ras.cover += delta;
		goto End;
	}
	if(dy > 0)
	{
		p = ( ONE_PIXEL - fy1) * dx;
		first = ONE_PIXEL;
		incr = 1;
	}
	else
	{
		p = fy1 * dx;
		first = 0;
		incr = -1;
		dy = -dy;
	}
	XCG_FT_DIV_MOD(TCoord, p, dy, delta, mod);
	x = ras.x + delta;
	gray_render_scanline( RAS_VAR_ ey1, ras.x, fy1, x, (TCoord)first);
	ey1 += incr;
	gray_set_cell( RAS_VAR_ TRUNC( x ), ey1);
	if(ey1 != ey2)
	{
		TCoord lift, rem;
		p = ONE_PIXEL * dx;
		XCG_FT_DIV_MOD(TCoord, p, dy, lift, rem);
		do {
			delta = lift;
			mod += rem;
			if(mod >= (TCoord)dy)
			{
				mod -= (TCoord)dy;
				delta++;
			}
			x2 = x + delta;
			gray_render_scanline( RAS_VAR_ ey1, x, ONE_PIXEL - first, x2, first);
			x = x2;
			ey1 += incr;
			gray_set_cell( RAS_VAR_ TRUNC( x ), ey1);
		} while(ey1 != ey2);
	}
	gray_render_scanline( RAS_VAR_ ey1, x, ONE_PIXEL - first, to_x, fy2);
End:
	ras.x = to_x;
	ras.y = to_y;
}

static void gray_split_conic(XCG_FT_Vector * base)
{
	TPos a, b;

	base[4].x = base[2].x;
	b = base[1].x;
	a = base[3].x = (base[2].x + b) / 2;
	b = base[1].x = (base[0].x + b) / 2;
	base[2].x = (a + b) / 2;

	base[4].y = base[2].y;
	b = base[1].y;
	a = base[3].y = (base[2].y + b) / 2;
	b = base[1].y = (base[0].y + b) / 2;
	base[2].y = (a + b) / 2;
}

static void gray_render_conic( RAS_ARG_ const XCG_FT_Vector * control, const XCG_FT_Vector * to)
{
	XCG_FT_Vector bez_stack[16 * 2 + 1];
	XCG_FT_Vector *arc = bez_stack;
	TPos dx, dy;
	int draw, split;

	arc[0].x = UPSCALE(to->x);
	arc[0].y = UPSCALE(to->y);
	arc[1].x = UPSCALE(control->x);
	arc[1].y = UPSCALE(control->y);
	arc[2].x = ras.x;
	arc[2].y = ras.y;

	if(( TRUNC( arc[0].y ) >= ras.max_ey &&
		TRUNC( arc[1].y ) >= ras.max_ey &&
		TRUNC( arc[2].y ) >= ras.max_ey) || ( TRUNC( arc[0].y ) < ras.min_ey &&
		TRUNC( arc[1].y ) < ras.min_ey &&
		TRUNC( arc[2].y ) < ras.min_ey))
	{
		ras.x = arc[0].x;
		ras.y = arc[0].y;
		return;
	}
	dx = XCG_FT_ABS(arc[2].x + arc[0].x - 2 * arc[1].x);
	dy = XCG_FT_ABS(arc[2].y + arc[0].y - 2 * arc[1].y);
	if(dx < dy)
		dx = dy;
	draw = 1;
	while(dx > ONE_PIXEL / 4)
	{
		dx >>= 2;
		draw <<= 1;
	}
	do {
		split = 1;
		while((draw & split) == 0)
		{
			gray_split_conic(arc);
			arc += 2;
			split <<= 1;
		}
		gray_render_line( RAS_VAR_ arc[0].x, arc[0].y);
		arc -= 2;
	} while(--draw);
}

static void gray_split_cubic(XCG_FT_Vector * base)
{
	TPos a, b, c, d;

	base[6].x = base[3].x;
	c = base[1].x;
	d = base[2].x;
	base[1].x = a = (base[0].x + c) / 2;
	base[5].x = b = (base[3].x + d) / 2;
	c = (c + d) / 2;
	base[2].x = a = (a + c) / 2;
	base[4].x = b = (b + c) / 2;
	base[3].x = (a + b) / 2;

	base[6].y = base[3].y;
	c = base[1].y;
	d = base[2].y;
	base[1].y = a = (base[0].y + c) / 2;
	base[5].y = b = (base[3].y + d) / 2;
	c = (c + d) / 2;
	base[2].y = a = (a + c) / 2;
	base[4].y = b = (b + c) / 2;
	base[3].y = (a + b) / 2;
}

static void gray_render_cubic(RAS_ARG_ const XCG_FT_Vector * control1, const XCG_FT_Vector * control2, const XCG_FT_Vector * to)
{
	XCG_FT_Vector bez_stack[16 * 3 + 1];
	XCG_FT_Vector * arc = bez_stack;
	TPos dx, dy, dx_, dy_;
	TPos dx1, dy1, dx2, dy2;
	TPos L, s, s_limit;

	arc[0].x = UPSCALE(to->x);
	arc[0].y = UPSCALE(to->y);
	arc[1].x = UPSCALE(control2->x);
	arc[1].y = UPSCALE(control2->y);
	arc[2].x = UPSCALE(control1->x);
	arc[2].y = UPSCALE(control1->y);
	arc[3].x = ras.x;
	arc[3].y = ras.y;

	if(( TRUNC( arc[0].y ) >= ras.max_ey &&
		TRUNC( arc[1].y ) >= ras.max_ey &&
		TRUNC( arc[2].y ) >= ras.max_ey &&
		TRUNC( arc[3].y ) >= ras.max_ey) || ( TRUNC( arc[0].y ) < ras.min_ey &&
		TRUNC( arc[1].y ) < ras.min_ey &&
		TRUNC( arc[2].y ) < ras.min_ey &&
		TRUNC( arc[3].y ) < ras.min_ey))
	{
		ras.x = arc[0].x;
		ras.y = arc[0].y;
		return;
	}
	for(;;)
	{
		dx = dx_ = arc[3].x - arc[0].x;
		dy = dy_ = arc[3].y - arc[0].y;
		L = XCG_FT_HYPOT(dx_, dy_);
		if(L >= (1 << 23))
			goto Split;
		s_limit = L * (TPos)( ONE_PIXEL / 6);
		dx1 = arc[1].x - arc[0].x;
		dy1 = arc[1].y - arc[0].y;
		s = XCG_FT_ABS(dy * dx1 - dx * dy1);
		if(s > s_limit)
			goto Split;
		dx2 = arc[2].x - arc[0].x;
		dy2 = arc[2].y - arc[0].y;
		s = XCG_FT_ABS(dy * dx2 - dx * dy2);
		if(s > s_limit)
			goto Split;
		if(dx1 * (dx1 - dx) + dy1 * (dy1 - dy) > 0 || dx2 * (dx2 - dx) + dy2 * (dy2 - dy) > 0)
			goto Split;
		gray_render_line( RAS_VAR_ arc[0].x, arc[0].y);
		if(arc == bez_stack)
			return;
		arc -= 3;
		continue;
Split:
		gray_split_cubic(arc);
		arc += 3;
	}
}

static int gray_move_to(const XCG_FT_Vector *to, PWorker worker)
{
	TPos x, y;

	if(!ras.invalid)
		gray_record_cell(worker);
	x = UPSCALE(to->x);
	y = UPSCALE(to->y);
	gray_start_cell(worker, TRUNC(x), TRUNC(y));
	ras.x = x;
	ras.y = y;
	return 0;
}

static void gray_hline( RAS_ARG_ TCoord x, TCoord y, TPos area, int acount)
{
	int coverage;

	coverage = (int)(area >> ( PIXEL_BITS * 2 + 1 - 8));
	if(coverage < 0)
		coverage = -coverage;
	if( ras.outline.flags & XCG_FT_OUTLINE_EVEN_ODD_FILL)
	{
		coverage &= 511;
		if(coverage > 256)
			coverage = 512 - coverage;
		else if(coverage == 256)
			coverage = 255;
	}
	else
	{
		if(coverage >= 256)
			coverage = 255;
	}
	y += (TCoord)ras.min_ey;
	x += (TCoord)ras.min_ex;
	if(x >= (1 << 23))
		x = (1 << 23) - 1;
	if(y >= (1 << 23))
		y = (1 << 23) - 1;
	if(coverage)
	{
		XCG_FT_Span *span;
		int count;
		int skip;
		count = ras.num_gray_spans;
		span = ras.gray_spans + count - 1;
		if(count > 0 && span->y == y && span->x + span->len == x && span->coverage == coverage)
		{
			span->len = span->len + acount;
			return;
		}
		if(count >= XCG_FT_MAX_GRAY_SPANS)
		{
			if( ras.render_span && count > ras.skip_spans)
			{
				skip = ras.skip_spans > 0 ? ras.skip_spans : 0;
				ras.render_span( ras.num_gray_spans - skip,
				ras.gray_spans + skip,
				ras.render_span_data);
			}
			ras.skip_spans -= ras.num_gray_spans;
			ras.num_gray_spans = 0;
			span = ras.gray_spans;
		}
		else
			span++;
		span->x = x;
		span->len = acount;
		span->y = y;
		span->coverage = (unsigned char)coverage;
		ras.num_gray_spans++;
	}
}

static void gray_sweep(RAS_ARG)
{
	int yindex;

	if( ras.num_cells == 0)
		return;
	for(yindex = 0; yindex < ras.ycount; yindex++)
	{
		PCell cell = ras.ycells[yindex];
		TCoord cover = 0;
		TCoord x = 0;

		for(; cell != NULL; cell = cell->next)
		{
			TArea area;
			if(cell->x > x && cover != 0)
				gray_hline( RAS_VAR_ x, yindex, cover * ( ONE_PIXEL * 2), cell->x - x);
			cover += cell->cover;
			area = cover * ( ONE_PIXEL * 2) - cell->area;
			if(area != 0 && cell->x >= 0)
				gray_hline( RAS_VAR_ cell->x, yindex, area, 1);
			x = cell->x + 1;
		}
		if( ras.count_ex > x && cover != 0)
			gray_hline( RAS_VAR_ x, yindex, cover * ( ONE_PIXEL * 2), ras.count_ex - x);
	}
}

static int XCG_FT_Outline_Decompose(const XCG_FT_Outline * outline, void * user)
{
#undef SCALED
#define SCALED( x )  (x)
	XCG_FT_Vector v_last;
	XCG_FT_Vector v_control;
	XCG_FT_Vector v_start;
	XCG_FT_Vector * point;
	XCG_FT_Vector * limit;
	char * tags;
	int n;
	int first;
	int error;
	char tag;

	if(!outline)
		return ErrRaster_Invalid_Outline;
	first = 0;
	for(n = 0; n < outline->n_contours; n++)
	{
		int last;
		last = outline->contours[n];
		if(last < 0)
			goto Invalid_Outline;
		limit = outline->points + last;
		v_start = outline->points[first];
		v_start.x = SCALED(v_start.x);
		v_start.y = SCALED(v_start.y);
		v_last = outline->points[last];
		v_last.x = SCALED(v_last.x);
		v_last.y = SCALED(v_last.y);
		v_control = v_start;
		point = outline->points + first;
		tags = outline->tags + first;
		tag = XCG_FT_CURVE_TAG(tags[0]);
		if(tag == XCG_FT_CURVE_TAG_CUBIC)
			goto Invalid_Outline;
		if(tag == XCG_FT_CURVE_TAG_CONIC)
		{
			if( XCG_FT_CURVE_TAG( outline->tags[last] ) == XCG_FT_CURVE_TAG_ON)
			{
				v_start = v_last;
				limit--;
			}
			else
			{
				v_start.x = (v_start.x + v_last.x) / 2;
				v_start.y = (v_start.y + v_last.y) / 2;
				v_last = v_start;
			}
			point--;
			tags--;
		}
		error = gray_move_to(&v_start, user);
		if(error)
			goto Exit;
		while(point < limit)
		{
			point++;
			tags++;
			tag = XCG_FT_CURVE_TAG(tags[0]);
			switch(tag)
			{
			case XCG_FT_CURVE_TAG_ON:
			{
				XCG_FT_Vector vec;
				vec.x = SCALED(point->x);
				vec.y = SCALED(point->y);
				gray_render_line(user, UPSCALE(vec.x), UPSCALE(vec.y));
				continue;
			}
			case XCG_FT_CURVE_TAG_CONIC:
			{
				v_control.x = SCALED(point->x);
				v_control.y = SCALED(point->y);
Do_Conic:
				if(point < limit)
				{
					XCG_FT_Vector vec;
					XCG_FT_Vector v_middle;
					point++;
					tags++;
					tag = XCG_FT_CURVE_TAG(tags[0]);
					vec.x = SCALED(point->x);
					vec.y = SCALED(point->y);
					if(tag == XCG_FT_CURVE_TAG_ON)
					{
						gray_render_conic(user, &v_control, &vec);
						continue;
					}
					if(tag != XCG_FT_CURVE_TAG_CONIC)
						goto Invalid_Outline;
					v_middle.x = (v_control.x + vec.x) / 2;
					v_middle.y = (v_control.y + vec.y) / 2;
					gray_render_conic(user, &v_control, &v_middle);
					v_control = vec;
					goto Do_Conic;
				}
				gray_render_conic(user, &v_control, &v_start);
				goto Close;
			}
			default:
			{
				XCG_FT_Vector vec1, vec2;
				if(point + 1 > limit ||
				XCG_FT_CURVE_TAG( tags[1] ) != XCG_FT_CURVE_TAG_CUBIC)
					goto Invalid_Outline;
				point += 2;
				tags += 2;
				vec1.x = SCALED(point[-2].x);
				vec1.y = SCALED(point[-2].y);
				vec2.x = SCALED(point[-1].x);
				vec2.y = SCALED(point[-1].y);
				if(point <= limit)
				{
					XCG_FT_Vector vec;
					vec.x = SCALED(point->x);
					vec.y = SCALED(point->y);
					gray_render_cubic(user, &vec1, &vec2, &vec);
					continue;
				}
				gray_render_cubic(user, &vec1, &vec2, &v_start);
				goto Close;
			}
			}
		}
		gray_render_line(user, UPSCALE(v_start.x), UPSCALE(v_start.y));
Close:
		first = last + 1;
	}
	return 0;
Exit:
	return error;
Invalid_Outline:
	return ErrRaster_Invalid_Outline;
}

typedef struct TBand_ {
	TPos min, max;
} TBand;

static int gray_convert_glyph_inner(RAS_ARG)
{
	volatile int error = 0;

	if( xcg_ft_setjmp( ras.jump_buffer) == 0)
	{
		error = XCG_FT_Outline_Decompose(&ras.outline, &ras);
		if(!ras.invalid)
			gray_record_cell( RAS_VAR);
	}
	else
	{
		error = ErrRaster_Memory_Overflow;
	}
	return error;
}

static int gray_convert_glyph(RAS_ARG)
{
	TBand bands[40];
	TBand *volatile band;
	int volatile n, num_bands;
	TPos volatile min, max, max_y;
	XCG_FT_BBox *clip;
	int skip;

	ras.num_gray_spans = 0;
	gray_compute_cbox( RAS_VAR);
	clip = &ras.clip_box;
	if( ras.max_ex <= clip->xMin || ras.min_ex >= clip->xMax || ras.max_ey <= clip->yMin || ras.min_ey >= clip->yMax)
		return 0;
	if( ras.min_ex < clip->xMin)
		ras.min_ex = clip->xMin;
	if( ras.min_ey < clip->yMin)
		ras.min_ey = clip->yMin;
	if( ras.max_ex > clip->xMax)
		ras.max_ex = clip->xMax;
	if( ras.max_ey > clip->yMax)
		ras.max_ey = clip->yMax;
	ras.count_ex = ras.max_ex - ras.min_ex;
	ras.count_ey = ras.max_ey - ras.min_ey;
	num_bands = (int)(( ras.max_ey - ras.min_ey) / ras.band_size);
	if(num_bands == 0)
		num_bands = 1;
	if(num_bands >= 39)
		num_bands = 39;
	ras.band_shoot = 0;
	min = ras.min_ey;
	max_y = ras.max_ey;
	for(n = 0; n < num_bands; n++, min = max)
	{
		max = min + ras.band_size;
		if(n == num_bands - 1 || max > max_y)
			max = max_y;
		bands[0].min = min;
		bands[0].max = max;
		band = bands;
		while(band >= bands)
		{
			TPos bottom, top, middle;
			int error;
			{
				PCell cells_max;
				int yindex;
				int cell_start, cell_end, cell_mod;
				ras.ycells = (PCell*)ras.buffer;
				ras.ycount = band->max - band->min;
				cell_start = sizeof(PCell) * ras.ycount;
				cell_mod = cell_start % sizeof(TCell);
				if(cell_mod > 0)
					cell_start += sizeof(TCell) - cell_mod;
				cell_end = ras.buffer_size;
				cell_end -= cell_end % sizeof(TCell);
				cells_max = (PCell)((char*)ras.buffer + cell_end);
				ras.cells = (PCell)((char*)ras.buffer + cell_start);
				if( ras.cells >= cells_max)
					goto ReduceBands;
				ras.max_cells = (int)(cells_max - ras.cells);
				if( ras.max_cells < 2)
					goto ReduceBands;
				for(yindex = 0; yindex < ras.ycount; yindex++)
					ras.ycells[yindex] = NULL;
			}
			ras.num_cells = 0;
			ras.invalid = 1;
			ras.min_ey = band->min;
			ras.max_ey = band->max;
			ras.count_ey = band->max - band->min;
			error = gray_convert_glyph_inner( RAS_VAR);
			if(!error)
			{
				gray_sweep( RAS_VAR);
				band--;
				continue;
			}
			else if(error != ErrRaster_Memory_Overflow)
				return 1;
ReduceBands:
			bottom = band->min;
			top = band->max;
			middle = bottom + ((top - bottom) >> 1);
			if(middle == bottom)
			{
				return ErrRaster_OutOfMemory;
			}
			if(bottom - top >= ras.band_size)
				ras.band_shoot++;
			band[1].min = bottom;
			band[1].max = middle;
			band[0].min = middle;
			band[0].max = top;
			band++;
		}
	}
	if( ras.render_span && ras.num_gray_spans > ras.skip_spans)
	{
		skip = ras.skip_spans > 0 ? ras.skip_spans : 0;
		ras.render_span( ras.num_gray_spans - skip,
		ras.gray_spans + skip,
		ras.render_span_data);
	}
	ras.skip_spans -= ras.num_gray_spans;
	if( ras.band_shoot > 8 && ras.band_size > 16)
		ras.band_size = ras.band_size / 2;
	return 0;
}

static int gray_raster_render(RAS_ARG_ void * buffer, long buffer_size, const XCG_FT_Raster_Params * params)
{
	const XCG_FT_Outline *outline = (const XCG_FT_Outline*)params->source;
	if(outline == NULL)
		return ErrRaster_Invalid_Outline;
	if(outline->n_points == 0 || outline->n_contours <= 0)
		return 0;
	if(!outline->contours || !outline->points)
		return ErrRaster_Invalid_Outline;
	if(outline->n_points != outline->contours[outline->n_contours - 1] + 1)
		return ErrRaster_Invalid_Outline;
	if(!(params->flags & XCG_FT_RASTER_FLAG_AA))
		return ErrRaster_Invalid_Mode;
	if(!(params->flags & XCG_FT_RASTER_FLAG_DIRECT))
		return ErrRaster_Invalid_Mode;
	if(params->flags & XCG_FT_RASTER_FLAG_CLIP)
	{
		ras.clip_box = params->clip_box;
	}
	else
	{
		ras.clip_box.xMin = -(1 << 23);
		ras.clip_box.yMin = -(1 << 23);
		ras.clip_box.xMax = (1 << 23) - 1;
		ras.clip_box.yMax = (1 << 23) - 1;
	}
	gray_init_cells( RAS_VAR_ buffer, buffer_size);
	ras.outline = *outline;
	ras.num_cells = 0;
	ras.invalid = 1;
	ras.band_size = (int)(buffer_size / (long)(sizeof(TCell) * 8));
	ras.render_span = (XCG_FT_Raster_Span_Func)params->gray_spans;
	ras.render_span_data = params->user;
	return gray_convert_glyph( RAS_VAR);
}

void XCG_FT_Raster_Render(const XCG_FT_Raster_Params * params)
{
	char stack[XCG_FT_MINIMUM_POOL_SIZE];
	size_t length = XCG_FT_MINIMUM_POOL_SIZE;

	TWorker worker;
	worker.skip_spans = 0;
	int rendered_spans = 0;
	int error = gray_raster_render(&worker, stack, length, params);
	while(error == ErrRaster_OutOfMemory)
	{
		if(worker.skip_spans < 0)
			rendered_spans += -worker.skip_spans;
		worker.skip_spans = rendered_spans;
		length *= 2;
		void *heap = malloc(length);
		error = gray_raster_render(&worker, heap, length, params);
		free(heap);
	}
}

/*
 * stroker
 */
#define XCG_FT_SMALL_CONIC_THRESHOLD	(XCG_FT_ANGLE_PI / 6)
#define XCG_FT_SMALL_CUBIC_THRESHOLD	(XCG_FT_ANGLE_PI / 8)
#define XCG_FT_IS_SMALL(x)				((x) > -2 && (x) < 2)

static XCG_FT_Pos ft_pos_abs(XCG_FT_Pos x)
{
	return x >= 0 ? x : -x;
}

static void ft_conic_split(XCG_FT_Vector *base)
{
	XCG_FT_Pos a, b;

	base[4].x = base[2].x;
	a = base[0].x + base[1].x;
	b = base[1].x + base[2].x;
	base[3].x = b >> 1;
	base[2].x = (a + b) >> 2;
	base[1].x = a >> 1;

	base[4].y = base[2].y;
	a = base[0].y + base[1].y;
	b = base[1].y + base[2].y;
	base[3].y = b >> 1;
	base[2].y = (a + b) >> 2;
	base[1].y = a >> 1;
}

static XCG_FT_Bool ft_conic_is_small_enough(XCG_FT_Vector *base, XCG_FT_Angle *angle_in, XCG_FT_Angle *angle_out)
{
	XCG_FT_Vector d1, d2;
	XCG_FT_Angle theta;
	XCG_FT_Int close1, close2;

	d1.x = base[1].x - base[2].x;
	d1.y = base[1].y - base[2].y;
	d2.x = base[0].x - base[1].x;
	d2.y = base[0].y - base[1].y;
	close1 = XCG_FT_IS_SMALL(d1.x) && XCG_FT_IS_SMALL(d1.y);
	close2 = XCG_FT_IS_SMALL(d2.x) && XCG_FT_IS_SMALL(d2.y);
	if(close1)
	{
		if(close2)
		{
		}
		else
		{
			*angle_in = *angle_out = XCG_FT_Atan2(d2.x, d2.y);
		}
	}
	else
	{
		if(close2)
		{
			*angle_in = *angle_out = XCG_FT_Atan2(d1.x, d1.y);
		}
		else
		{
			*angle_in = XCG_FT_Atan2(d1.x, d1.y);
			*angle_out = XCG_FT_Atan2(d2.x, d2.y);
		}
	}
	theta = ft_pos_abs(XCG_FT_Angle_Diff(*angle_in, *angle_out));
	return XCG_FT_BOOL(theta < XCG_FT_SMALL_CONIC_THRESHOLD);
}

static void ft_cubic_split(XCG_FT_Vector * base)
{
	XCG_FT_Pos a, b, c;

	base[6].x = base[3].x;
	a = base[0].x + base[1].x;
	b = base[1].x + base[2].x;
	c = base[2].x + base[3].x;
	base[5].x = c >> 1;
	c += b;
	base[4].x = c >> 2;
	base[1].x = a >> 1;
	a += b;
	base[2].x = a >> 2;
	base[3].x = (a + c) >> 3;

	base[6].y = base[3].y;
	a = base[0].y + base[1].y;
	b = base[1].y + base[2].y;
	c = base[2].y + base[3].y;
	base[5].y = c >> 1;
	c += b;
	base[4].y = c >> 2;
	base[1].y = a >> 1;
	a += b;
	base[2].y = a >> 2;
	base[3].y = (a + c) >> 3;
}

static XCG_FT_Angle ft_angle_mean(XCG_FT_Angle angle1, XCG_FT_Angle angle2)
{
	return angle1 + XCG_FT_Angle_Diff(angle1, angle2) / 2;
}

static XCG_FT_Bool ft_cubic_is_small_enough(XCG_FT_Vector *base, XCG_FT_Angle *angle_in, XCG_FT_Angle *angle_mid, XCG_FT_Angle *angle_out)
{
	XCG_FT_Vector d1, d2, d3;
	XCG_FT_Angle theta1, theta2;
	XCG_FT_Int close1, close2, close3;

	d1.x = base[2].x - base[3].x;
	d1.y = base[2].y - base[3].y;
	d2.x = base[1].x - base[2].x;
	d2.y = base[1].y - base[2].y;
	d3.x = base[0].x - base[1].x;
	d3.y = base[0].y - base[1].y;
	close1 = XCG_FT_IS_SMALL(d1.x) && XCG_FT_IS_SMALL(d1.y);
	close2 = XCG_FT_IS_SMALL(d2.x) && XCG_FT_IS_SMALL(d2.y);
	close3 = XCG_FT_IS_SMALL(d3.x) && XCG_FT_IS_SMALL(d3.y);

	if(close1)
	{
		if(close2)
		{
			if(close3)
			{
			}
			else
			{
				*angle_in = *angle_mid = *angle_out = XCG_FT_Atan2(d3.x, d3.y);
			}
		}
		else
		{
			if(close3)
			{
				*angle_in = *angle_mid = *angle_out = XCG_FT_Atan2(d2.x, d2.y);
			}
			else
			{
				*angle_in = *angle_mid = XCG_FT_Atan2(d2.x, d2.y);
				*angle_out = XCG_FT_Atan2(d3.x, d3.y);
			}
		}
	}
	else
	{
		if(close2)
		{
			if(close3)
			{
				*angle_in = *angle_mid = *angle_out = XCG_FT_Atan2(d1.x, d1.y);
			}
			else
			{
				*angle_in = XCG_FT_Atan2(d1.x, d1.y);
				*angle_out = XCG_FT_Atan2(d3.x, d3.y);
				*angle_mid = ft_angle_mean(*angle_in, *angle_out);
			}
		}
		else
		{
			if(close3)
			{
				*angle_in = XCG_FT_Atan2(d1.x, d1.y);
				*angle_mid = *angle_out = XCG_FT_Atan2(d2.x, d2.y);
			}
			else
			{
				*angle_in = XCG_FT_Atan2(d1.x, d1.y);
				*angle_mid = XCG_FT_Atan2(d2.x, d2.y);
				*angle_out = XCG_FT_Atan2(d3.x, d3.y);
			}
		}
	}
	theta1 = ft_pos_abs(XCG_FT_Angle_Diff(*angle_in, *angle_mid));
	theta2 = ft_pos_abs(XCG_FT_Angle_Diff(*angle_mid, *angle_out));
	return XCG_FT_BOOL(theta1 < XCG_FT_SMALL_CUBIC_THRESHOLD && theta2 < XCG_FT_SMALL_CUBIC_THRESHOLD);
}

typedef enum XCG_FT_StrokeTags_ {
	XCG_FT_STROKE_TAG_ON = 1,
	XCG_FT_STROKE_TAG_CUBIC = 2,
	XCG_FT_STROKE_TAG_BEGIN = 4,
	XCG_FT_STROKE_TAG_END = 8,
} XCG_FT_StrokeTags;

#define XCG_FT_STROKE_TAG_BEGIN_END		(XCG_FT_STROKE_TAG_BEGIN | XCG_FT_STROKE_TAG_END)

typedef struct XCG_FT_StrokeBorderRec_ {
	XCG_FT_UInt num_points;
	XCG_FT_UInt max_points;
	XCG_FT_Vector * points;
	XCG_FT_Byte * tags;
	XCG_FT_Bool movable;
	XCG_FT_Int start;
	XCG_FT_Bool valid;
} XCG_FT_StrokeBorderRec, *XCG_FT_StrokeBorder;

XCG_FT_Error XCG_FT_Outline_Check(XCG_FT_Outline * outline)
{
	if(outline)
	{
		XCG_FT_Int n_points = outline->n_points;
		XCG_FT_Int n_contours = outline->n_contours;
		XCG_FT_Int end0, end;
		XCG_FT_Int n;

		if(n_points == 0 && n_contours == 0)
			return 0;
		if(n_points <= 0 || n_contours <= 0)
			goto Bad;
		end0 = end = -1;
		for(n = 0; n < n_contours; n++)
		{
			end = outline->contours[n];
			if(end <= end0 || end >= n_points)
				goto Bad;
			end0 = end;
		}
		if(end != n_points - 1)
			goto Bad;
		return 0;
	}
Bad:
	return -1;
}

void XCG_FT_Outline_Get_CBox(const XCG_FT_Outline * outline, XCG_FT_BBox * acbox)
{
	XCG_FT_Pos xMin, yMin, xMax, yMax;

	if(outline && acbox)
	{
		if(outline->n_points == 0)
		{
			xMin = 0;
			yMin = 0;
			xMax = 0;
			yMax = 0;
		}
		else
		{
			XCG_FT_Vector *vec = outline->points;
			XCG_FT_Vector *limit = vec + outline->n_points;
			xMin = xMax = vec->x;
			yMin = yMax = vec->y;
			vec++;
			for(; vec < limit; vec++)
			{
				XCG_FT_Pos x, y;
				x = vec->x;
				if(x < xMin)
					xMin = x;
				if(x > xMax)
					xMax = x;
				y = vec->y;
				if(y < yMin)
					yMin = y;
				if(y > yMax)
					yMax = y;
			}
		}
		acbox->xMin = xMin;
		acbox->xMax = xMax;
		acbox->yMin = yMin;
		acbox->yMax = yMax;
	}
}

static XCG_FT_Error ft_stroke_border_grow(XCG_FT_StrokeBorder border, XCG_FT_UInt new_points)
{
	XCG_FT_UInt old_max = border->max_points;
	XCG_FT_UInt new_max = border->num_points + new_points;
	XCG_FT_Error error = 0;

	if(new_max > old_max)
	{
		XCG_FT_UInt cur_max = old_max;
		while(cur_max < new_max)
			cur_max += (cur_max >> 1) + 16;
		border->points = (XCG_FT_Vector*)realloc(border->points, cur_max * sizeof(XCG_FT_Vector));
		border->tags = (XCG_FT_Byte*)realloc(border->tags, cur_max * sizeof(XCG_FT_Byte));
		if(!border->points || !border->tags)
			goto Exit;
		border->max_points = cur_max;
	}
Exit:
	return error;
}

static void ft_stroke_border_close(XCG_FT_StrokeBorder border, XCG_FT_Bool reverse)
{
	XCG_FT_UInt start = border->start;
	XCG_FT_UInt count = border->num_points;

	if(count <= start + 1U)
		border->num_points = start;
	else
	{
		border->num_points = --count;
		border->points[start] = border->points[count];
		border->tags[start] = border->tags[count];
		if(reverse)
		{
			{
				XCG_FT_Vector *vec1 = border->points + start + 1;
				XCG_FT_Vector *vec2 = border->points + count - 1;
				for(; vec1 < vec2; vec1++, vec2--)
				{
					XCG_FT_Vector tmp;
					tmp = *vec1;
					*vec1 = *vec2;
					*vec2 = tmp;
				}
			}
			{
				XCG_FT_Byte *tag1 = border->tags + start + 1;
				XCG_FT_Byte *tag2 = border->tags + count - 1;

				for(; tag1 < tag2; tag1++, tag2--)
				{
					XCG_FT_Byte tmp;
					tmp = *tag1;
					*tag1 = *tag2;
					*tag2 = tmp;
				}
			}
		}
		border->tags[start] |= XCG_FT_STROKE_TAG_BEGIN;
		border->tags[count - 1] |= XCG_FT_STROKE_TAG_END;
	}
	border->start = -1;
	border->movable = FALSE;
}

static XCG_FT_Error ft_stroke_border_lineto(XCG_FT_StrokeBorder border, XCG_FT_Vector * to, XCG_FT_Bool movable)
{
	XCG_FT_Error error = 0;

	if(border->movable)
	{
		border->points[border->num_points - 1] = *to;
	}
	else
	{
		if(border->num_points > 0&&
		XCG_FT_IS_SMALL(border->points[border->num_points - 1].x - to->x) &&
		XCG_FT_IS_SMALL(border->points[border->num_points - 1].y - to->y))
			return error;
		error = ft_stroke_border_grow(border, 1);
		if(!error)
		{
			XCG_FT_Vector *vec = border->points + border->num_points;
			XCG_FT_Byte *tag = border->tags + border->num_points;
			vec[0] = *to;
			tag[0] = XCG_FT_STROKE_TAG_ON;
			border->num_points += 1;
		}
	}
	border->movable = movable;
	return error;
}

static XCG_FT_Error ft_stroke_border_conicto(XCG_FT_StrokeBorder border, XCG_FT_Vector * control, XCG_FT_Vector * to)
{
	XCG_FT_Error error;

	error = ft_stroke_border_grow(border, 2);
	if(!error)
	{
		XCG_FT_Vector *vec = border->points + border->num_points;
		XCG_FT_Byte *tag = border->tags + border->num_points;

		vec[0] = *control;
		vec[1] = *to;
		tag[0] = 0;
		tag[1] = XCG_FT_STROKE_TAG_ON;
		border->num_points += 2;
	}
	border->movable = FALSE;
	return error;
}

static XCG_FT_Error ft_stroke_border_cubicto(XCG_FT_StrokeBorder border, XCG_FT_Vector * control1, XCG_FT_Vector * control2, XCG_FT_Vector * to)
{
	XCG_FT_Error error;

	error = ft_stroke_border_grow(border, 3);
	if(!error)
	{
		XCG_FT_Vector *vec = border->points + border->num_points;
		XCG_FT_Byte *tag = border->tags + border->num_points;

		vec[0] = *control1;
		vec[1] = *control2;
		vec[2] = *to;
		tag[0] = XCG_FT_STROKE_TAG_CUBIC;
		tag[1] = XCG_FT_STROKE_TAG_CUBIC;
		tag[2] = XCG_FT_STROKE_TAG_ON;
		border->num_points += 3;
	}
	border->movable = FALSE;
	return error;
}

#define XCG_FT_ARC_CUBIC_ANGLE		(XCG_FT_ANGLE_PI / 2)

static XCG_FT_Error ft_stroke_border_arcto(XCG_FT_StrokeBorder border, XCG_FT_Vector *center, XCG_FT_Fixed radius, XCG_FT_Angle angle_start, XCG_FT_Angle angle_diff)
{
	XCG_FT_Fixed coef;
	XCG_FT_Vector a0, a1, a2, a3;
	XCG_FT_Int i, arcs = 1;
	XCG_FT_Error error = 0;

	while(angle_diff > XCG_FT_ARC_CUBIC_ANGLE * arcs || -angle_diff > XCG_FT_ARC_CUBIC_ANGLE * arcs)
		arcs++;
	coef = XCG_FT_Tan(angle_diff / (4 * arcs));
	coef += coef / 3;
	XCG_FT_Vector_From_Polar(&a0, radius, angle_start);
	a1.x = XCG_FT_MulFix(-a0.y, coef);
	a1.y = XCG_FT_MulFix(a0.x, coef);
	a0.x += center->x;
	a0.y += center->y;
	a1.x += a0.x;
	a1.y += a0.y;
	for(i = 1; i <= arcs; i++)
	{
		XCG_FT_Vector_From_Polar(&a3, radius, angle_start + i * angle_diff / arcs);
		a2.x = XCG_FT_MulFix(a3.y, coef);
		a2.y = XCG_FT_MulFix(-a3.x, coef);
		a3.x += center->x;
		a3.y += center->y;
		a2.x += a3.x;
		a2.y += a3.y;
		error = ft_stroke_border_cubicto(border, &a1, &a2, &a3);
		if(error)
			break;
		a1.x = a3.x - a2.x + a3.x;
		a1.y = a3.y - a2.y + a3.y;
	}
	return error;
}

static XCG_FT_Error ft_stroke_border_moveto(XCG_FT_StrokeBorder border, XCG_FT_Vector * to)
{
	if(border->start >= 0)
		ft_stroke_border_close(border, FALSE);
	border->start = border->num_points;
	border->movable = FALSE;
	return ft_stroke_border_lineto(border, to, FALSE);
}

static void ft_stroke_border_init(XCG_FT_StrokeBorder border)
{
	border->points = NULL;
	border->tags = NULL;
	border->num_points = 0;
	border->max_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static void ft_stroke_border_reset(XCG_FT_StrokeBorder border)
{
	border->num_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static void ft_stroke_border_done(XCG_FT_StrokeBorder border)
{
	free(border->points);
	free(border->tags);

	border->num_points = 0;
	border->max_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static XCG_FT_Error ft_stroke_border_get_counts(XCG_FT_StrokeBorder border, XCG_FT_UInt * anum_points, XCG_FT_UInt * anum_contours)
{
	XCG_FT_Error error = 0;
	XCG_FT_UInt num_points = 0;
	XCG_FT_UInt num_contours = 0;
	XCG_FT_UInt count = border->num_points;
	XCG_FT_Vector *point = border->points;
	XCG_FT_Byte *tags = border->tags;
	XCG_FT_Int in_contour = 0;

	for(; count > 0; count--, num_points++, point++, tags++)
	{
		if(tags[0] & XCG_FT_STROKE_TAG_BEGIN)
		{
			if(in_contour != 0)
				goto Fail;
			in_contour = 1;
		}
		else if(in_contour == 0)
			goto Fail;
		if(tags[0] & XCG_FT_STROKE_TAG_END)
		{
			in_contour = 0;
			num_contours++;
		}
	}
	if(in_contour != 0)
		goto Fail;
	border->valid = TRUE;
Exit:
	*anum_points = num_points;
	*anum_contours = num_contours;
	return error;
Fail:
	num_points = 0;
	num_contours = 0;
	goto Exit;
}

static void ft_stroke_border_export(XCG_FT_StrokeBorder border, XCG_FT_Outline * outline)
{
	if(outline->points != NULL && border->points != NULL)
		memcpy(outline->points + outline->n_points, border->points, border->num_points * sizeof(XCG_FT_Vector));
	if(outline->tags)
	{
		XCG_FT_UInt count = border->num_points;
		XCG_FT_Byte *read = border->tags;
		XCG_FT_Byte *write = (XCG_FT_Byte*)outline->tags + outline->n_points;
		for(; count > 0; count--, read++, write++)
		{
			if(*read & XCG_FT_STROKE_TAG_ON)
				*write = XCG_FT_CURVE_TAG_ON;
			else if(*read & XCG_FT_STROKE_TAG_CUBIC)
				*write = XCG_FT_CURVE_TAG_CUBIC;
			else
				*write = XCG_FT_CURVE_TAG_CONIC;
		}
	}
	if(outline->contours)
	{
		XCG_FT_UInt count = border->num_points;
		XCG_FT_Byte *tags = border->tags;
		XCG_FT_Int *write = outline->contours + outline->n_contours;
		XCG_FT_Int idx = (XCG_FT_Int)outline->n_points;
		for(; count > 0; count--, tags++, idx++)
		{
			if(*tags & XCG_FT_STROKE_TAG_END)
			{
				*write++ = idx;
				outline->n_contours++;
			}
		}
	}
	outline->n_points = (int)(outline->n_points + border->num_points);
	XCG_FT_Outline_Check(outline);
}

#define XCG_FT_SIDE_TO_ROTATE(s)	(XCG_FT_ANGLE_PI2 - (s) * XCG_FT_ANGLE_PI)

typedef struct XCG_FT_StrokerRec_ {
	XCG_FT_Angle angle_in;
	XCG_FT_Angle angle_out;
	XCG_FT_Vector center;
	XCG_FT_Fixed line_length;
	XCG_FT_Bool first_point;
	XCG_FT_Bool subpath_open;
	XCG_FT_Angle subpath_angle;
	XCG_FT_Vector subpath_start;
	XCG_FT_Fixed subpath_line_length;
	XCG_FT_Bool handle_wide_strokes;
	XCG_FT_Stroker_LineCap line_cap;
	XCG_FT_Stroker_LineJoin line_join;
	XCG_FT_Stroker_LineJoin line_join_saved;
	XCG_FT_Fixed miter_limit;
	XCG_FT_Fixed radius;
	XCG_FT_StrokeBorderRec borders[2];
} XCG_FT_StrokerRec;

XCG_FT_Error XCG_FT_Stroker_New(XCG_FT_Stroker * astroker)
{
	XCG_FT_Error error = 0;
	XCG_FT_Stroker stroker = NULL;
	stroker = (XCG_FT_StrokerRec*)calloc(1, sizeof(XCG_FT_StrokerRec));
	if(stroker)
	{
		ft_stroke_border_init(&stroker->borders[0]);
		ft_stroke_border_init(&stroker->borders[1]);
	}
	*astroker = stroker;
	return error;
}

void XCG_FT_Stroker_Rewind(XCG_FT_Stroker stroker)
{
	if(stroker)
	{
		ft_stroke_border_reset(&stroker->borders[0]);
		ft_stroke_border_reset(&stroker->borders[1]);
	}
}

void XCG_FT_Stroker_Set(XCG_FT_Stroker stroker, XCG_FT_Fixed radius, XCG_FT_Stroker_LineCap line_cap, XCG_FT_Stroker_LineJoin line_join, XCG_FT_Fixed miter_limit)
{
	stroker->radius = radius;
	stroker->line_cap = line_cap;
	stroker->line_join = line_join;
	stroker->miter_limit = miter_limit;
	if(stroker->miter_limit < 0x10000)
		stroker->miter_limit = 0x10000;
	stroker->line_join_saved = line_join;
	XCG_FT_Stroker_Rewind(stroker);
}

void XCG_FT_Stroker_Done(XCG_FT_Stroker stroker)
{
	if(stroker)
	{
		ft_stroke_border_done(&stroker->borders[0]);
		ft_stroke_border_done(&stroker->borders[1]);
		free(stroker);
	}
}

static XCG_FT_Error ft_stroker_arcto(XCG_FT_Stroker stroker, XCG_FT_Int side)
{
	XCG_FT_Angle total, rotate;
	XCG_FT_Fixed radius = stroker->radius;
	XCG_FT_Error error = 0;
	XCG_FT_StrokeBorder border = stroker->borders + side;

	rotate = XCG_FT_SIDE_TO_ROTATE(side);
	total = XCG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
	if(total == XCG_FT_ANGLE_PI)
		total = -rotate * 2;
	error = ft_stroke_border_arcto(border, &stroker->center, radius, stroker->angle_in + rotate, total);
	border->movable = FALSE;
	return error;
}

static XCG_FT_Error ft_stroker_cap(XCG_FT_Stroker stroker, XCG_FT_Angle angle, XCG_FT_Int side)
{
	XCG_FT_Error error = 0;

	if(stroker->line_cap == XCG_FT_STROKER_LINECAP_ROUND)
	{
		stroker->angle_in = angle;
		stroker->angle_out = angle + XCG_FT_ANGLE_PI;
		error = ft_stroker_arcto(stroker, side);
	}
	else
	{
		XCG_FT_Vector middle, delta;
		XCG_FT_Fixed radius = stroker->radius;
		XCG_FT_StrokeBorder border = stroker->borders + side;
		XCG_FT_Vector_From_Polar(&middle, radius, angle);
		delta.x = side ? middle.y : -middle.y;
		delta.y = side ? -middle.x : middle.x;
		if(stroker->line_cap == XCG_FT_STROKER_LINECAP_SQUARE)
		{
			middle.x += stroker->center.x;
			middle.y += stroker->center.y;
		}
		else
		{
			middle.x = stroker->center.x;
			middle.y = stroker->center.y;
		}
		delta.x += middle.x;
		delta.y += middle.y;
		error = ft_stroke_border_lineto(border, &delta, FALSE);
		if(error)
			goto Exit;
		delta.x = middle.x - delta.x + middle.x;
		delta.y = middle.y - delta.y + middle.y;
		error = ft_stroke_border_lineto(border, &delta, FALSE);
	}
Exit:
	return error;
}

static XCG_FT_Error ft_stroker_inside(XCG_FT_Stroker stroker, XCG_FT_Int side, XCG_FT_Fixed line_length)
{
	XCG_FT_StrokeBorder border = stroker->borders + side;
	XCG_FT_Angle phi, theta, rotate;
	XCG_FT_Fixed length;
	XCG_FT_Vector sigma = { 0, 0 };
	XCG_FT_Vector delta;
	XCG_FT_Error error = 0;
	XCG_FT_Bool intersect;

	rotate = XCG_FT_SIDE_TO_ROTATE(side);
	theta = XCG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;
	if(!border->movable || line_length == 0 || theta > 0x59C000 || theta < -0x59C000)
		intersect = FALSE;
	else
	{
		XCG_FT_Fixed min_length;
		XCG_FT_Vector_Unit(&sigma, theta);
		min_length = ft_pos_abs(XCG_FT_MulDiv(stroker->radius, sigma.y, sigma.x));
		intersect = XCG_FT_BOOL(min_length && stroker->line_length >= min_length && line_length >= min_length);
	}
	if(!intersect)
	{
		XCG_FT_Vector_From_Polar(&delta, stroker->radius, stroker->angle_out + rotate);
		delta.x += stroker->center.x;
		delta.y += stroker->center.y;
		border->movable = FALSE;
	}
	else
	{
		phi = stroker->angle_in + theta + rotate;
		length = XCG_FT_DivFix(stroker->radius, sigma.x);
		XCG_FT_Vector_From_Polar(&delta, length, phi);
		delta.x += stroker->center.x;
		delta.y += stroker->center.y;
	}
	error = ft_stroke_border_lineto(border, &delta, FALSE);
	return error;
}

static XCG_FT_Error ft_stroker_outside(XCG_FT_Stroker stroker, XCG_FT_Int side, XCG_FT_Fixed line_length)
{
	XCG_FT_StrokeBorder border = stroker->borders + side;
	XCG_FT_Error error;
	XCG_FT_Angle rotate;

	if(stroker->line_join == XCG_FT_STROKER_LINEJOIN_ROUND)
		error = ft_stroker_arcto(stroker, side);
	else
	{
		XCG_FT_Fixed radius = stroker->radius;
		XCG_FT_Vector sigma = { 0, 0 };
		XCG_FT_Angle theta = 0, phi = 0;
		XCG_FT_Bool bevel, fixed_bevel;

		rotate = XCG_FT_SIDE_TO_ROTATE(side);
		bevel = XCG_FT_BOOL(stroker->line_join == XCG_FT_STROKER_LINEJOIN_BEVEL);
		fixed_bevel = XCG_FT_BOOL(stroker->line_join != XCG_FT_STROKER_LINEJOIN_MITER_VARIABLE);

if(!bevel)
		{
			theta = XCG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;
			if(theta == XCG_FT_ANGLE_PI2)
				theta = -rotate;
			phi = stroker->angle_in + theta + rotate;
			XCG_FT_Vector_From_Polar(&sigma, stroker->miter_limit, theta);
			if(sigma.x < 0x10000L)
			{
				if(fixed_bevel || ft_pos_abs(theta) > 57)
					bevel = TRUE;
			}
		}
		if(bevel)
		{
			if(fixed_bevel)
			{
				XCG_FT_Vector delta;
				XCG_FT_Vector_From_Polar(&delta, radius, stroker->angle_out + rotate);
				delta.x += stroker->center.x;
				delta.y += stroker->center.y;
				border->movable = FALSE;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
			}
			else
			{
				XCG_FT_Vector middle, delta;
				XCG_FT_Fixed coef;

				XCG_FT_Vector_From_Polar(&middle, XCG_FT_MulFix(radius, stroker->miter_limit), phi);
				coef = XCG_FT_DivFix(0x10000L - sigma.x, sigma.y);
				delta.x = XCG_FT_MulFix(middle.y, coef);
				delta.y = XCG_FT_MulFix(-middle.x, coef);
				middle.x += stroker->center.x;
				middle.y += stroker->center.y;
				delta.x += middle.x;
				delta.y += middle.y;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
				if(error)
					goto Exit;
				delta.x = middle.x - delta.x + middle.x;
				delta.y = middle.y - delta.y + middle.y;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
				if(error)
					goto Exit;
				if(line_length == 0)
				{
					XCG_FT_Vector_From_Polar(&delta, radius, stroker->angle_out + rotate);
					delta.x += stroker->center.x;
					delta.y += stroker->center.y;
					error = ft_stroke_border_lineto(border, &delta, FALSE);
				}
			}
		}
		else
		{
			XCG_FT_Fixed length;
			XCG_FT_Vector delta;
			length = XCG_FT_MulDiv(stroker->radius, stroker->miter_limit, sigma.x);
			XCG_FT_Vector_From_Polar(&delta, length, phi);
			delta.x += stroker->center.x;
			delta.y += stroker->center.y;
			error = ft_stroke_border_lineto(border, &delta, FALSE);
			if(error)
				goto Exit;
			if(line_length == 0)
			{
				XCG_FT_Vector_From_Polar(&delta, stroker->radius, stroker->angle_out + rotate);
				delta.x += stroker->center.x;
				delta.y += stroker->center.y;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
			}
		}
	}
Exit:
	return error;
}

static XCG_FT_Error ft_stroker_process_corner(XCG_FT_Stroker stroker, XCG_FT_Fixed line_length)
{
	XCG_FT_Error error = 0;
	XCG_FT_Angle turn;
	XCG_FT_Int inside_side;

	turn = XCG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
	if(turn == 0)
		goto Exit;
	inside_side = 0;
	if(turn < 0)
		inside_side = 1;
	error = ft_stroker_inside(stroker, inside_side, line_length);
	if(error)
		goto Exit;
	error = ft_stroker_outside(stroker, 1 - inside_side, line_length);
Exit:
	return error;
}

static XCG_FT_Error ft_stroker_subpath_start(XCG_FT_Stroker stroker, XCG_FT_Angle start_angle, XCG_FT_Fixed line_length)
{
	XCG_FT_Vector delta;
	XCG_FT_Vector point;
	XCG_FT_Error error;
	XCG_FT_StrokeBorder border;

	XCG_FT_Vector_From_Polar(&delta, stroker->radius, start_angle + XCG_FT_ANGLE_PI2);
	point.x = stroker->center.x + delta.x;
	point.y = stroker->center.y + delta.y;
	border = stroker->borders;
	error = ft_stroke_border_moveto(border, &point);
	if(error)
		goto Exit;
	point.x = stroker->center.x - delta.x;
	point.y = stroker->center.y - delta.y;
	border++;
	error = ft_stroke_border_moveto(border, &point);
	stroker->subpath_angle = start_angle;
	stroker->first_point = FALSE;
	stroker->subpath_line_length = line_length;
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_LineTo(XCG_FT_Stroker stroker, XCG_FT_Vector * to)
{
	XCG_FT_Error error = 0;
	XCG_FT_StrokeBorder border;
	XCG_FT_Vector delta;
	XCG_FT_Angle angle;
	XCG_FT_Int side;
	XCG_FT_Fixed line_length;

	delta.x = to->x - stroker->center.x;
	delta.y = to->y - stroker->center.y;
	if(delta.x == 0 && delta.y == 0)
		goto Exit;
	line_length = XCG_FT_Vector_Length(&delta);
	angle = XCG_FT_Atan2(delta.x, delta.y);
	XCG_FT_Vector_From_Polar(&delta, stroker->radius, angle + XCG_FT_ANGLE_PI2);
	if(stroker->first_point)
	{
		error = ft_stroker_subpath_start(stroker, angle, line_length);
		if(error)
			goto Exit;
	}
	else
	{
		stroker->angle_out = angle;
		error = ft_stroker_process_corner(stroker, line_length);
		if(error)
			goto Exit;
	}
	for(border = stroker->borders, side = 1; side >= 0; side--, border++)
	{
		XCG_FT_Vector point;
		point.x = to->x + delta.x;
		point.y = to->y + delta.y;
		error = ft_stroke_border_lineto(border, &point, TRUE);
		if(error)
			goto Exit;
		delta.x = -delta.x;
		delta.y = -delta.y;
	}
	stroker->angle_in = angle;
	stroker->center = *to;
	stroker->line_length = line_length;
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_ConicTo(XCG_FT_Stroker stroker, XCG_FT_Vector * control, XCG_FT_Vector * to)
{
	XCG_FT_Error error = 0;
	XCG_FT_Vector bez_stack[34];
	XCG_FT_Vector *arc;
	XCG_FT_Vector *limit = bez_stack + 30;
	XCG_FT_Bool first_arc = TRUE;

	if(XCG_FT_IS_SMALL(stroker->center.x - control->x) &&
		XCG_FT_IS_SMALL(stroker->center.y - control->y) &&
		XCG_FT_IS_SMALL(control->x - to->x) &&
		XCG_FT_IS_SMALL(control->y - to->y))
	{
		stroker->center = *to;
		goto Exit;
	}
	arc = bez_stack;
	arc[0] = *to;
	arc[1] = *control;
	arc[2] = stroker->center;
	while(arc >= bez_stack)
	{
		XCG_FT_Angle angle_in, angle_out;
		angle_in = angle_out = stroker->angle_in;
		if(arc < limit && !ft_conic_is_small_enough(arc, &angle_in, &angle_out))
		{
			if(stroker->first_point)
				stroker->angle_in = angle_in;
			ft_conic_split(arc);
			arc += 2;
			continue;
		}
		if(first_arc)
		{
			first_arc = FALSE;
			if(stroker->first_point)
				error = ft_stroker_subpath_start(stroker, angle_in, 0);
			else
			{
				stroker->angle_out = angle_in;
				error = ft_stroker_process_corner(stroker, 0);
			}
		}
		else if(ft_pos_abs(XCG_FT_Angle_Diff(stroker->angle_in, angle_in)) > XCG_FT_SMALL_CONIC_THRESHOLD / 4)
		{
			stroker->center = arc[2];
			stroker->angle_out = angle_in;
			stroker->line_join = XCG_FT_STROKER_LINEJOIN_ROUND;
			error = ft_stroker_process_corner(stroker, 0);
			stroker->line_join = stroker->line_join_saved;
		}
		if(error)
			goto Exit;
		{
			XCG_FT_Vector ctrl, end;
			XCG_FT_Angle theta, phi, rotate, alpha0 = 0;
			XCG_FT_Fixed length;
			XCG_FT_StrokeBorder border;
			XCG_FT_Int side;

			theta = XCG_FT_Angle_Diff(angle_in, angle_out) / 2;
			phi = angle_in + theta;
			length = XCG_FT_DivFix(stroker->radius, XCG_FT_Cos(theta));
			if(stroker->handle_wide_strokes)
				alpha0 = XCG_FT_Atan2(arc[0].x - arc[2].x, arc[0].y - arc[2].y);
			for(border = stroker->borders, side = 0; side <= 1; side++, border++)
			{
				rotate = XCG_FT_SIDE_TO_ROTATE(side);
				XCG_FT_Vector_From_Polar(&ctrl, length, phi + rotate);
				ctrl.x += arc[1].x;
				ctrl.y += arc[1].y;
				XCG_FT_Vector_From_Polar(&end, stroker->radius, angle_out + rotate);
				end.x += arc[0].x;
				end.y += arc[0].y;

				if(stroker->handle_wide_strokes)
				{
					XCG_FT_Vector start;
					XCG_FT_Angle alpha1;

					start = border->points[border->num_points - 1];
					alpha1 = XCG_FT_Atan2(end.x - start.x, end.y - start.y);
					if(ft_pos_abs(XCG_FT_Angle_Diff(alpha0, alpha1)) >
					XCG_FT_ANGLE_PI / 2)
					{
						XCG_FT_Angle beta, gamma;
						XCG_FT_Vector bvec, delta;
						XCG_FT_Fixed blen, sinA, sinB, alen;

						beta = XCG_FT_Atan2(arc[2].x - start.x, arc[2].y - start.y);
						gamma = XCG_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);

						bvec.x = end.x - start.x;
						bvec.y = end.y - start.y;
						blen = XCG_FT_Vector_Length(&bvec);
						sinA = ft_pos_abs(XCG_FT_Sin(alpha1 - gamma));
						sinB = ft_pos_abs(XCG_FT_Sin(beta - gamma));
						alen = XCG_FT_MulDiv(blen, sinA, sinB);
						XCG_FT_Vector_From_Polar(&delta, alen, beta);
						delta.x += start.x;
						delta.y += start.y;
						border->movable = FALSE;
						error = ft_stroke_border_lineto(border, &delta, FALSE);
						if(error)
							goto Exit;
						error = ft_stroke_border_lineto(border, &end, FALSE);
						if(error)
							goto Exit;
						error = ft_stroke_border_conicto(border, &ctrl, &start);
						if(error)
							goto Exit;
						error = ft_stroke_border_lineto(border, &end, FALSE);
						if(error)
							goto Exit;
						continue;
					}
				}
				error = ft_stroke_border_conicto(border, &ctrl, &end);
				if(error)
					goto Exit;
			}
		}
		arc -= 2;
		stroker->angle_in = angle_out;
	}
	stroker->center = *to;
	stroker->line_length = 0;
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_CubicTo(XCG_FT_Stroker stroker, XCG_FT_Vector *control1, XCG_FT_Vector *control2, XCG_FT_Vector *to)
{
	XCG_FT_Error error = 0;
	XCG_FT_Vector bez_stack[37];
	XCG_FT_Vector *arc;
	XCG_FT_Vector *limit = bez_stack + 32;
	XCG_FT_Bool first_arc = TRUE;

	if(XCG_FT_IS_SMALL(stroker->center.x - control1->x) &&
		XCG_FT_IS_SMALL(stroker->center.y - control1->y) &&
		XCG_FT_IS_SMALL(control1->x - control2->x) &&
		XCG_FT_IS_SMALL(control1->y - control2->y) &&
		XCG_FT_IS_SMALL(control2->x - to->x) &&
		XCG_FT_IS_SMALL(control2->y - to->y))
	{
		stroker->center = *to;
		goto Exit;
	}
	arc = bez_stack;
	arc[0] = *to;
	arc[1] = *control2;
	arc[2] = *control1;
	arc[3] = stroker->center;
	while(arc >= bez_stack)
	{
		XCG_FT_Angle angle_in, angle_mid, angle_out;
		angle_in = angle_out = angle_mid = stroker->angle_in;
		if(arc < limit && !ft_cubic_is_small_enough(arc, &angle_in, &angle_mid, &angle_out))
		{
			if(stroker->first_point)
				stroker->angle_in = angle_in;
			ft_cubic_split(arc);
			arc += 3;
			continue;
		}
		if(first_arc)
		{
			first_arc = FALSE;
			if(stroker->first_point)
				error = ft_stroker_subpath_start(stroker, angle_in, 0);
			else
			{
				stroker->angle_out = angle_in;
				error = ft_stroker_process_corner(stroker, 0);
			}
		}
		else if(ft_pos_abs(XCG_FT_Angle_Diff(stroker->angle_in, angle_in)) > XCG_FT_SMALL_CUBIC_THRESHOLD / 4)
		{
			stroker->center = arc[3];
			stroker->angle_out = angle_in;
			stroker->line_join = XCG_FT_STROKER_LINEJOIN_ROUND;
			error = ft_stroker_process_corner(stroker, 0);
			stroker->line_join = stroker->line_join_saved;
		}
		if(error)
			goto Exit;
		{
			XCG_FT_Vector ctrl1, ctrl2, end;
			XCG_FT_Angle theta1, phi1, theta2, phi2, rotate, alpha0 = 0;
			XCG_FT_Fixed length1, length2;
			XCG_FT_StrokeBorder border;
			XCG_FT_Int side;

			theta1 = XCG_FT_Angle_Diff(angle_in, angle_mid) / 2;
			theta2 = XCG_FT_Angle_Diff(angle_mid, angle_out) / 2;
			phi1 = ft_angle_mean(angle_in, angle_mid);
			phi2 = ft_angle_mean(angle_mid, angle_out);
			length1 = XCG_FT_DivFix(stroker->radius, XCG_FT_Cos(theta1));
			length2 = XCG_FT_DivFix(stroker->radius, XCG_FT_Cos(theta2));
			if(stroker->handle_wide_strokes)
				alpha0 = XCG_FT_Atan2(arc[0].x - arc[3].x, arc[0].y - arc[3].y);
			for(border = stroker->borders, side = 0; side <= 1; side++, border++)
			{
				rotate = XCG_FT_SIDE_TO_ROTATE(side);
				XCG_FT_Vector_From_Polar(&ctrl1, length1, phi1 + rotate);
				ctrl1.x += arc[2].x;
				ctrl1.y += arc[2].y;
				XCG_FT_Vector_From_Polar(&ctrl2, length2, phi2 + rotate);
				ctrl2.x += arc[1].x;
				ctrl2.y += arc[1].y;
				XCG_FT_Vector_From_Polar(&end, stroker->radius, angle_out + rotate);
				end.x += arc[0].x;
				end.y += arc[0].y;
				if(stroker->handle_wide_strokes)
				{
					XCG_FT_Vector start;
					XCG_FT_Angle alpha1;
					start = border->points[border->num_points - 1];
					alpha1 = XCG_FT_Atan2(end.x - start.x, end.y - start.y);
					if(ft_pos_abs(XCG_FT_Angle_Diff(alpha0, alpha1)) >
					XCG_FT_ANGLE_PI / 2)
					{
						XCG_FT_Angle beta, gamma;
						XCG_FT_Vector bvec, delta;
						XCG_FT_Fixed blen, sinA, sinB, alen;
						beta = XCG_FT_Atan2(arc[3].x - start.x, arc[3].y - start.y);
						gamma = XCG_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);
						bvec.x = end.x - start.x;
						bvec.y = end.y - start.y;
						blen = XCG_FT_Vector_Length(&bvec);
						sinA = ft_pos_abs(XCG_FT_Sin(alpha1 - gamma));
						sinB = ft_pos_abs(XCG_FT_Sin(beta - gamma));
						alen = XCG_FT_MulDiv(blen, sinA, sinB);
						XCG_FT_Vector_From_Polar(&delta, alen, beta);
						delta.x += start.x;
						delta.y += start.y;
						border->movable = FALSE;
						error = ft_stroke_border_lineto(border, &delta, FALSE);
						if(error)
							goto Exit;
						error = ft_stroke_border_lineto(border, &end, FALSE);
						if(error)
							goto Exit;
						error = ft_stroke_border_cubicto(border, &ctrl2, &ctrl1, &start);
						if(error)
							goto Exit;
						error = ft_stroke_border_lineto(border, &end, FALSE);
						if(error)
							goto Exit;
						continue;
					}
				}
				error = ft_stroke_border_cubicto(border, &ctrl1, &ctrl2, &end);
				if(error)
					goto Exit;
			}
		}
		arc -= 3;
		stroker->angle_in = angle_out;
	}
	stroker->center = *to;
	stroker->line_length = 0;
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_BeginSubPath(XCG_FT_Stroker stroker, XCG_FT_Vector * to, XCG_FT_Bool open)
{
	stroker->first_point = TRUE;
	stroker->center = *to;
	stroker->subpath_open = open;
	stroker->handle_wide_strokes = XCG_FT_BOOL(stroker->line_join != XCG_FT_STROKER_LINEJOIN_ROUND || (stroker->subpath_open && stroker->line_cap == XCG_FT_STROKER_LINECAP_BUTT));
	stroker->subpath_start = *to;
	stroker->angle_in = 0;
	return 0;
}

static XCG_FT_Error ft_stroker_add_reverse_left(XCG_FT_Stroker stroker, XCG_FT_Bool open)
{
	XCG_FT_StrokeBorder right = stroker->borders + 0;
	XCG_FT_StrokeBorder left = stroker->borders + 1;
	XCG_FT_Int new_points;
	XCG_FT_Error error = 0;

	new_points = left->num_points - left->start;
	if(new_points > 0)
	{
		error = ft_stroke_border_grow(right, (XCG_FT_UInt)new_points);
		if(error)
			goto Exit;
		{
			XCG_FT_Vector *dst_point = right->points + right->num_points;
			XCG_FT_Byte *dst_tag = right->tags + right->num_points;
			XCG_FT_Vector *src_point = left->points + left->num_points - 1;
			XCG_FT_Byte *src_tag = left->tags + left->num_points - 1;
			while(src_point >= left->points + left->start)
			{
				*dst_point = *src_point;
				*dst_tag = *src_tag;
				if(open)
					dst_tag[0] &= ~XCG_FT_STROKE_TAG_BEGIN_END;
				else
				{
					XCG_FT_Byte ttag = (XCG_FT_Byte)(dst_tag[0] & XCG_FT_STROKE_TAG_BEGIN_END);
					if(ttag == XCG_FT_STROKE_TAG_BEGIN || ttag == XCG_FT_STROKE_TAG_END)
						dst_tag[0] ^= XCG_FT_STROKE_TAG_BEGIN_END;
				}
				src_point--;
				src_tag--;
				dst_point++;
				dst_tag++;
			}
		}
		left->num_points = left->start;
		right->num_points += new_points;
		right->movable = FALSE;
		left->movable = FALSE;
	}
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_EndSubPath(XCG_FT_Stroker stroker)
{
	XCG_FT_Error error = 0;

	if(stroker->subpath_open)
	{
		XCG_FT_StrokeBorder right = stroker->borders;
		error = ft_stroker_cap(stroker, stroker->angle_in, 0);
		if(error)
			goto Exit;
		error = ft_stroker_add_reverse_left(stroker, TRUE);
		if(error)
			goto Exit;
		stroker->center = stroker->subpath_start;
		error = ft_stroker_cap(stroker, stroker->subpath_angle + XCG_FT_ANGLE_PI, 0);
		if(error)
			goto Exit;
		ft_stroke_border_close(right, FALSE);
	}
	else
	{
		XCG_FT_Angle turn;
		XCG_FT_Int inside_side;
		if(stroker->center.x != stroker->subpath_start.x || stroker->center.y != stroker->subpath_start.y)
		{
			error = XCG_FT_Stroker_LineTo(stroker, &stroker->subpath_start);
			if(error)
				goto Exit;
		}
		stroker->angle_out = stroker->subpath_angle;
		turn = XCG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
		if(turn != 0)
		{
			inside_side = 0;
			if(turn < 0)
				inside_side = 1;
			error = ft_stroker_inside(stroker, inside_side, stroker->subpath_line_length);
			if(error)
				goto Exit;
			error = ft_stroker_outside(stroker, 1 - inside_side, stroker->subpath_line_length);
			if(error)
				goto Exit;
		}
		ft_stroke_border_close(stroker->borders + 0, FALSE);
		ft_stroke_border_close(stroker->borders + 1, TRUE);
	}
Exit:
	return error;
}

XCG_FT_Error XCG_FT_Stroker_GetBorderCounts(XCG_FT_Stroker stroker, XCG_FT_StrokerBorder border, XCG_FT_UInt * anum_points, XCG_FT_UInt * anum_contours)
{
	XCG_FT_UInt num_points = 0, num_contours = 0;
	XCG_FT_Error error;

	if(!stroker || border > 1)
	{
		error = -1;
		goto Exit;
	}
	error = ft_stroke_border_get_counts(stroker->borders + border, &num_points, &num_contours);
Exit:
	if(anum_points)
		*anum_points = num_points;
	if(anum_contours)
		*anum_contours = num_contours;
	return error;
}

XCG_FT_Error XCG_FT_Stroker_GetCounts(XCG_FT_Stroker stroker, XCG_FT_UInt * anum_points, XCG_FT_UInt * anum_contours)
{
	XCG_FT_UInt count1, count2, num_points = 0;
	XCG_FT_UInt count3, count4, num_contours = 0;
	XCG_FT_Error error;

	error = ft_stroke_border_get_counts(stroker->borders + 0, &count1, &count2);
	if(error)
		goto Exit;
	error = ft_stroke_border_get_counts(stroker->borders + 1, &count3, &count4);
	if(error)
		goto Exit;
	num_points = count1 + count3;
	num_contours = count2 + count4;
Exit:
	*anum_points = num_points;
	*anum_contours = num_contours;
	return error;
}

void XCG_FT_Stroker_ExportBorder(XCG_FT_Stroker stroker, XCG_FT_StrokerBorder border, XCG_FT_Outline * outline)
{
	if(border == XCG_FT_STROKER_BORDER_LEFT || border == XCG_FT_STROKER_BORDER_RIGHT)
	{
		XCG_FT_StrokeBorder sborder = &stroker->borders[border];
		if(sborder->valid)
			ft_stroke_border_export(sborder, outline);
	}
}

void XCG_FT_Stroker_Export(XCG_FT_Stroker stroker, XCG_FT_Outline * outline)
{
	XCG_FT_Stroker_ExportBorder(stroker, XCG_FT_STROKER_BORDER_LEFT, outline);
	XCG_FT_Stroker_ExportBorder(stroker, XCG_FT_STROKER_BORDER_RIGHT, outline);
}

XCG_FT_Error XCG_FT_Stroker_ParseOutline(XCG_FT_Stroker stroker, const XCG_FT_Outline *outline)
{
	XCG_FT_Vector v_last;
	XCG_FT_Vector v_control;
	XCG_FT_Vector v_start;
	XCG_FT_Vector * point;
	XCG_FT_Vector * limit;
	char * tags;
	XCG_FT_Error error;
	XCG_FT_Int n;
	XCG_FT_UInt first;
	XCG_FT_Int tag;

	if(!outline || !stroker)
		return -1;
	XCG_FT_Stroker_Rewind(stroker);
	first = 0;
	for(n = 0; n < outline->n_contours; n++)
	{
		XCG_FT_UInt last;
		last = outline->contours[n];
		limit = outline->points + last;
		if(last <= first)
		{
			first = last + 1;
			continue;
		}
		v_start = outline->points[first];
		v_last = outline->points[last];
		v_control = v_start;
		point = outline->points + first;
		tags = outline->tags + first;
		tag = XCG_FT_CURVE_TAG(tags[0]);
		if(tag == XCG_FT_CURVE_TAG_CUBIC)
			goto Invalid_Outline;
		if(tag == XCG_FT_CURVE_TAG_CONIC)
		{
			if(XCG_FT_CURVE_TAG(outline->tags[last]) == XCG_FT_CURVE_TAG_ON)
			{
				v_start = v_last;
				limit--;
			}
			else
			{
				v_start.x = (v_start.x + v_last.x) / 2;
				v_start.y = (v_start.y + v_last.y) / 2;
			}
			point--;
			tags--;
		}
		error = XCG_FT_Stroker_BeginSubPath(stroker, &v_start, outline->contours_flag[n]);
		if(error)
			goto Exit;
		while(point < limit)
		{
			point++;
			tags++;
			tag = XCG_FT_CURVE_TAG(tags[0]);
			switch(tag)
			{
			case XCG_FT_CURVE_TAG_ON:
			{
				XCG_FT_Vector vec;
				vec.x = point->x;
				vec.y = point->y;
				error = XCG_FT_Stroker_LineTo(stroker, &vec);
				if(error)
					goto Exit;
				continue;
			}

			case XCG_FT_CURVE_TAG_CONIC:
				v_control.x = point->x;
				v_control.y = point->y;
Do_Conic:
				if(point < limit)
				{
					XCG_FT_Vector vec;
					XCG_FT_Vector v_middle;
					point++;
					tags++;
					tag = XCG_FT_CURVE_TAG(tags[0]);
					vec = point[0];
					if(tag == XCG_FT_CURVE_TAG_ON)
					{
						error = XCG_FT_Stroker_ConicTo(stroker, &v_control, &vec);
						if(error)
							goto Exit;
						continue;
					}
					if(tag != XCG_FT_CURVE_TAG_CONIC)
						goto Invalid_Outline;
					v_middle.x = (v_control.x + vec.x) / 2;
					v_middle.y = (v_control.y + vec.y) / 2;
					error = XCG_FT_Stroker_ConicTo(stroker, &v_control, &v_middle);
					if(error)
						goto Exit;
					v_control = vec;
					goto Do_Conic;
				}
				error = XCG_FT_Stroker_ConicTo(stroker, &v_control, &v_start);
				goto Close;

			default:
			{
				XCG_FT_Vector vec1, vec2;
				if(point + 1 > limit ||
				XCG_FT_CURVE_TAG(tags[1]) != XCG_FT_CURVE_TAG_CUBIC)
					goto Invalid_Outline;
				point += 2;
				tags += 2;
				vec1 = point[-2];
				vec2 = point[-1];
				if(point <= limit)
				{
					XCG_FT_Vector vec;
					vec = point[0];
					error = XCG_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &vec);
					if(error)
						goto Exit;
					continue;
				}
				error = XCG_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &v_start);
				goto Close;
			}
			}
		}
Close:
		if(error)
			goto Exit;
		if(stroker->first_point)
		{
			stroker->subpath_open = TRUE;
			error = ft_stroker_subpath_start(stroker, 0, 0);
			if(error)
				goto Exit;
		}
		error = XCG_FT_Stroker_EndSubPath(stroker);
		if(error)
			goto Exit;
		first = last + 1;
	}
	return 0;
Exit:
	return error;
Invalid_Outline:
	return -2;
}
