/*
 * swft.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

#include <swft.h>

/*
 * type
 */
typedef int64_t			SW_FT_Int64;
typedef uint64_t		SW_FT_UInt64;
typedef int32_t			SW_FT_Int32;
typedef uint32_t		SW_FT_UInt32;

#define SW_FT_BOOL(x)	((SW_FT_Bool)(x))
#ifndef TRUE
#define TRUE			1
#endif
#ifndef FALSE
#define FALSE			0
#endif

/*
 * math
 */
#define SW_FT_MIN(a, b)		((a) < (b) ? (a) : (b))
#define SW_FT_MAX(a, b)		((a) > (b) ? (a) : (b))
#define SW_FT_ABS(a)		((a) < 0 ? -(a) : (a))
#define SW_FT_HYPOT(x, y)	(x = SW_FT_ABS(x), y = SW_FT_ABS(y), x > y ? x + (3 * y >> 3) : y + (3 * x >> 3))
#define SW_FT_ANGLE_PI		(180L << 16)
#define SW_FT_ANGLE_2PI		(SW_FT_ANGLE_PI * 2)
#define SW_FT_ANGLE_PI2		(SW_FT_ANGLE_PI / 2)
#define SW_FT_ANGLE_PI4		(SW_FT_ANGLE_PI / 4)

#define SW_FT_MSB(x)			(31 - __builtin_clz(x))
#define SW_FT_PAD_FLOOR(x, n)	((x) & ~((n)-1))
#define SW_FT_PAD_ROUND(x, n)	SW_FT_PAD_FLOOR((x) + ((n) / 2), n)
#define SW_FT_PAD_CEIL(x, n)	SW_FT_PAD_FLOOR((x) + ((n)-1), n)

#define SW_FT_MOVE_SIGN(x, s) \
	do { \
		if(x < 0) { \
			x = -x; \
			s = -s; \
		} \
	} while(0)

SW_FT_Long SW_FT_MulFix(SW_FT_Long a, SW_FT_Long b)
{
	SW_FT_Int s = 1;
	SW_FT_Long c;

	SW_FT_MOVE_SIGN(a, s);
	SW_FT_MOVE_SIGN(b, s);
	c = (SW_FT_Long)(((SW_FT_Int64)a * b + 0x8000L) >> 16);
	return (s > 0) ? c : -c;
}

SW_FT_Long SW_FT_MulDiv(SW_FT_Long a, SW_FT_Long b, SW_FT_Long c)
{
	SW_FT_Int s = 1;
	SW_FT_Long d;

	SW_FT_MOVE_SIGN(a, s);
	SW_FT_MOVE_SIGN(b, s);
	SW_FT_MOVE_SIGN(c, s);
	d = (SW_FT_Long)(c > 0 ? ((SW_FT_Int64)a * b + (c >> 1)) / c : 0x7FFFFFFFL);
	return (s > 0) ? d : -d;
}

SW_FT_Long SW_FT_DivFix(SW_FT_Long a, SW_FT_Long b)
{
	SW_FT_Int s = 1;
	SW_FT_Long q;

	SW_FT_MOVE_SIGN(a, s);
	SW_FT_MOVE_SIGN(b, s);
	q = (SW_FT_Long)(b > 0 ? (((SW_FT_UInt64)a << 16) + (b >> 1)) / b : 0x7FFFFFFFL);
	return (s < 0 ? -q : q);
}

#define SW_FT_TRIG_SCALE		(0xDBD95B16UL)
#define SW_FT_TRIG_SAFE_MSB		(29)
#define SW_FT_TRIG_MAX_ITERS	(23)

static const SW_FT_Fixed ft_trig_arctan_table[] = {
	1740967L, 919879L, 466945L, 234379L, 117304L, 58666L, 29335L, 14668L,
	7334L,    3667L,   1833L,   917L,    458L,    229L,   115L,   57L,
	29L,      14L,     7L,      4L,      2L,      1L
};

static SW_FT_Fixed ft_trig_downscale(SW_FT_Fixed val)
{
	SW_FT_Fixed s;
	SW_FT_Int64 v;

	s = val;
	val = SW_FT_ABS(val);
	v = (val * (SW_FT_Int64)SW_FT_TRIG_SCALE) + 0x100000000UL;
	val = (SW_FT_Fixed)(v >> 32);
	return (s >= 0) ? val : -val;
}

static SW_FT_Int ft_trig_prenorm(SW_FT_Vector * vec)
{
	SW_FT_Pos x, y;
	SW_FT_Int shift;

	x = vec->x;
	y = vec->y;
	shift = SW_FT_MSB(SW_FT_ABS(x) | SW_FT_ABS(y));
	if(shift <= SW_FT_TRIG_SAFE_MSB)
	{
		shift = SW_FT_TRIG_SAFE_MSB - shift;
		vec->x = (SW_FT_Pos)((SW_FT_ULong)x << shift);
		vec->y = (SW_FT_Pos)((SW_FT_ULong)y << shift);
	}
	else
	{
		shift -= SW_FT_TRIG_SAFE_MSB;
		vec->x = x >> shift;
		vec->y = y >> shift;
		shift = -shift;
	}
	return shift;
}

static void ft_trig_pseudo_rotate(SW_FT_Vector * vec, SW_FT_Angle theta)
{
	SW_FT_Int i;
	SW_FT_Fixed x, y, xtemp, b;
	const SW_FT_Fixed * arctanptr;

	x = vec->x;
	y = vec->y;
	while(theta < -SW_FT_ANGLE_PI4)
	{
		xtemp = y;
		y = -x;
		x = xtemp;
		theta += SW_FT_ANGLE_PI2;
	}
	while(theta > SW_FT_ANGLE_PI4)
	{
		xtemp = -y;
		y = x;
		x = xtemp;
		theta -= SW_FT_ANGLE_PI2;
	}
	arctanptr = ft_trig_arctan_table;
	for(i = 1, b = 1; i < SW_FT_TRIG_MAX_ITERS; b <<= 1, i++)
	{
		SW_FT_Fixed v1 = ((y + b) >> i);
		SW_FT_Fixed v2 = ((x + b) >> i);
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

static void ft_trig_pseudo_polarize(SW_FT_Vector * vec)
{
	SW_FT_Angle theta;
	SW_FT_Int i;
	SW_FT_Fixed x, y, xtemp, b;
	const SW_FT_Fixed * arctanptr;

	x = vec->x;
	y = vec->y;
	if(y > x)
	{
		if(y > -x)
		{
			theta = SW_FT_ANGLE_PI2;
			xtemp = y;
			y = -x;
			x = xtemp;
		}
		else
		{
			theta = y > 0 ? SW_FT_ANGLE_PI : -SW_FT_ANGLE_PI;
			x = -x;
			y = -y;
		}
	}
	else
	{
		if(y < -x)
		{
			theta = -SW_FT_ANGLE_PI2;
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
	for(i = 1, b = 1; i < SW_FT_TRIG_MAX_ITERS; b <<= 1, i++)
	{
		SW_FT_Fixed v1 = ((y + b) >> i);
		SW_FT_Fixed v2 = ((x + b) >> i);
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
		theta = SW_FT_PAD_ROUND(theta, 32);
	else
		theta = -SW_FT_PAD_ROUND(-theta, 32);
	vec->x = x;
	vec->y = theta;
}

SW_FT_Fixed SW_FT_Cos(SW_FT_Angle angle)
{
	SW_FT_Vector v;

	v.x = SW_FT_TRIG_SCALE >> 8;
	v.y = 0;
	ft_trig_pseudo_rotate(&v, angle);
	return (v.x + 0x80L) >> 8;
}

SW_FT_Fixed SW_FT_Sin(SW_FT_Angle angle)
{
	return SW_FT_Cos(SW_FT_ANGLE_PI2 - angle);
}

SW_FT_Fixed SW_FT_Tan(SW_FT_Angle angle)
{
	SW_FT_Vector v;

	v.x = SW_FT_TRIG_SCALE >> 8;
	v.y = 0;
	ft_trig_pseudo_rotate(&v, angle);
	return SW_FT_DivFix(v.y, v.x);
}

SW_FT_Angle SW_FT_Atan2(SW_FT_Fixed dx, SW_FT_Fixed dy)
{
	SW_FT_Vector v;

	if(dx == 0 && dy == 0)
		return 0;
	v.x = dx;
	v.y = dy;
	ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	return v.y;
}

void SW_FT_Vector_Unit(SW_FT_Vector * vec, SW_FT_Angle angle)
{
	vec->x = SW_FT_TRIG_SCALE >> 8;
	vec->y = 0;
	ft_trig_pseudo_rotate(vec, angle);
	vec->x = (vec->x + 0x80L) >> 8;
	vec->y = (vec->y + 0x80L) >> 8;
}

#define SW_FT_SIGN_LONG(x)	((x) >> (sizeof(signed long) * 8 - 1))
void SW_FT_Vector_Rotate(SW_FT_Vector * vec, SW_FT_Angle angle)
{
	SW_FT_Int shift;
	SW_FT_Vector v;

	v.x = vec->x;
	v.y = vec->y;
	if(angle && (v.x != 0 || v.y != 0))
	{
		shift = ft_trig_prenorm(&v);
		ft_trig_pseudo_rotate(&v, angle);
		v.x = ft_trig_downscale(v.x);
		v.y = ft_trig_downscale(v.y);
		if(shift > 0)
		{
			SW_FT_Int32 half = (SW_FT_Int32)1L << (shift - 1);
			vec->x = (v.x + half + SW_FT_SIGN_LONG(v.x)) >> shift;
			vec->y = (v.y + half + SW_FT_SIGN_LONG(v.y)) >> shift;
		}
		else
		{
			shift = -shift;
			vec->x = (SW_FT_Pos)((SW_FT_ULong)v.x << shift);
			vec->y = (SW_FT_Pos)((SW_FT_ULong)v.y << shift);
		}
	}
}

SW_FT_Fixed SW_FT_Vector_Length(SW_FT_Vector * vec)
{
	SW_FT_Int shift;
	SW_FT_Vector v;

	v = *vec;
	if(v.x == 0)
		return SW_FT_ABS(v.y);
	else if(v.y == 0)
		return SW_FT_ABS(v.x);
	shift = ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	v.x = ft_trig_downscale(v.x);
	if(shift > 0)
		return (v.x + (1 << (shift - 1))) >> shift;
	return (SW_FT_Fixed)((SW_FT_UInt32)v.x << -shift);
}

void SW_FT_Vector_Polarize(SW_FT_Vector * vec, SW_FT_Fixed * length, SW_FT_Angle *angle)
{
	SW_FT_Int shift;
	SW_FT_Vector v;

	v = *vec;
	if(v.x == 0 && v.y == 0)
		return;
	shift = ft_trig_prenorm(&v);
	ft_trig_pseudo_polarize(&v);
	v.x = ft_trig_downscale(v.x);
	*length = (shift >= 0) ? (v.x >> shift) : (SW_FT_Fixed)((SW_FT_UInt32)v.x << -shift);
	*angle = v.y;
}

void SW_FT_Vector_From_Polar(SW_FT_Vector * vec, SW_FT_Fixed length, SW_FT_Angle angle)
{
	vec->x = length;
	vec->y = 0;
	SW_FT_Vector_Rotate(vec, angle);
}

SW_FT_Angle SW_FT_Angle_Diff(SW_FT_Angle angle1, SW_FT_Angle angle2)
{
	SW_FT_Angle delta = angle2 - angle1;

	while(delta <= -SW_FT_ANGLE_PI)
		delta += SW_FT_ANGLE_2PI;
	while(delta > SW_FT_ANGLE_PI)
		delta -= SW_FT_ANGLE_2PI;
	return delta;
}

/*
 * raster
 */

#define SW_FT_ERR_XCAT(x, y) x##y
#define SW_FT_ERR_CAT(x, y) SW_FT_ERR_XCAT(x, y)
#define SW_FT_BEGIN_STMNT do {
#define SW_FT_END_STMNT \
    }                   \
    while (0)
#define SW_FT_UINT_MAX UINT_MAX
#define SW_FT_INT_MAX INT_MAX
#define SW_FT_ULONG_MAX ULONG_MAX
#define SW_FT_CHAR_BIT CHAR_BIT

#define ft_memset memset
#define ft_setjmp setjmp
#define ft_longjmp longjmp
#define ft_jmp_buf jmp_buf
typedef ptrdiff_t SW_FT_PtrDist;

#define ErrRaster_Invalid_Mode -2
#define ErrRaster_Invalid_Outline -1
#define ErrRaster_Invalid_Argument -3
#define ErrRaster_Memory_Overflow -4
#define SW_FT_BEGIN_HEADER
#define SW_FT_END_HEADER

#define SW_FT_UNUSED(x) (x) = (x)
#define SW_FT_THROW(e) SW_FT_ERR_CAT(ErrRaster_, e)
#define SW_FT_RENDER_POOL_SIZE 16384L

typedef int (*SW_FT_Outline_MoveToFunc)(const SW_FT_Vector* to, void* user);
#define SW_FT_Outline_MoveTo_Func SW_FT_Outline_MoveToFunc
typedef int (*SW_FT_Outline_LineToFunc)(const SW_FT_Vector* to, void* user);
#define SW_FT_Outline_LineTo_Func SW_FT_Outline_LineToFunc
typedef int (*SW_FT_Outline_ConicToFunc)(const SW_FT_Vector* control,
                                         const SW_FT_Vector* to, void* user);
#define SW_FT_Outline_ConicTo_Func SW_FT_Outline_ConicToFunc
typedef int (*SW_FT_Outline_CubicToFunc)(const SW_FT_Vector* control1,
                                         const SW_FT_Vector* control2,
                                         const SW_FT_Vector* to, void* user);
#define SW_FT_Outline_CubicTo_Func SW_FT_Outline_CubicToFunc

typedef struct SW_FT_Outline_Funcs_ {
	SW_FT_Outline_MoveToFunc move_to;
	SW_FT_Outline_LineToFunc line_to;
	SW_FT_Outline_ConicToFunc conic_to;
	SW_FT_Outline_CubicToFunc cubic_to;
	int shift;
	SW_FT_Pos delta;
} SW_FT_Outline_Funcs;

#define SW_FT_DEFINE_OUTLINE_FUNCS(class_, move_to_, line_to_, conic_to_,      \
                                   cubic_to_, shift_, delta_)                  \
    static const SW_FT_Outline_Funcs class_ = {move_to_,  line_to_, conic_to_, \
                                               cubic_to_, shift_,   delta_};

#define SW_FT_DEFINE_RASTER_FUNCS(class_, raster_new_, raster_reset_, \
                                  raster_render_, raster_done_)       \
    const SW_FT_Raster_Funcs class_ = {raster_new_, raster_reset_,    \
                                       raster_render_, raster_done_};

#ifndef SW_FT_MEM_SET
#define SW_FT_MEM_SET(d, s, c) ft_memset(d, s, c)
#endif

#ifndef SW_FT_MEM_ZERO
#define SW_FT_MEM_ZERO(dest, count) SW_FT_MEM_SET(dest, 0, count)
#endif

#undef RAS_ARG
#undef RAS_ARG_
#undef RAS_VAR
#undef RAS_VAR_

#ifndef SW_FT_STATIC_RASTER
#define RAS_ARG gray_PWorker worker
#define RAS_ARG_ gray_PWorker worker,
#define RAS_VAR worker
#define RAS_VAR_ worker,
#else
#define RAS_ARG
#define RAS_ARG_
#define RAS_VAR
#define RAS_VAR_
#endif

#define PIXEL_BITS	8
#undef FLOOR
#undef CEILING
#undef TRUNC
#undef SCALED

#define ONE_PIXEL (1L << PIXEL_BITS)
#define PIXEL_MASK (-1L << PIXEL_BITS)
#define TRUNC(x) ((TCoord)((x) >> PIXEL_BITS))
#define SUBPIXELS(x) ((TPos)(x) << PIXEL_BITS)
#define FLOOR(x) ((x) & -ONE_PIXEL)
#define CEILING(x) (((x) + ONE_PIXEL - 1) & -ONE_PIXEL)
#define ROUND(x) (((x) + ONE_PIXEL / 2) & -ONE_PIXEL)

#if PIXEL_BITS >= 6
#define UPSCALE(x) ((x) << (PIXEL_BITS - 6))
#define DOWNSCALE(x) ((x) >> (PIXEL_BITS - 6))
#else
#define UPSCALE(x) ((x) >> (6 - PIXEL_BITS))
#define DOWNSCALE(x) ((x) << (6 - PIXEL_BITS))
#endif

#define SW_FT_DIV_MOD(type, dividend, divisor, quotient, remainder) \
    SW_FT_BEGIN_STMNT(quotient) = (type)((dividend) / (divisor));   \
    (remainder) = (type)((dividend) % (divisor));                   \
    if ((remainder) < 0) {                                          \
        (quotient)--;                                               \
        (remainder) += (type)(divisor);                             \
    }                                                               \
    SW_FT_END_STMNT

#define SW_FT_UDIVPREP(b) \
    long b##_r = (long)(SW_FT_ULONG_MAX >> PIXEL_BITS) / (b)
#define SW_FT_UDIV(a, b)                              \
    (((unsigned long)(a) * (unsigned long)(b##_r)) >> \
     (sizeof(long) * SW_FT_CHAR_BIT - PIXEL_BITS))

typedef long TCoord;
typedef long TPos;

#if PIXEL_BITS <= 7
typedef int TArea;
#else
#if SW_FT_UINT_MAX == 0xFFFFU
typedef long Area;
#else
typedef int TArea;
#endif
#endif

#define SW_FT_MAX_GRAY_SPANS	256
typedef struct TCell_* PCell;

typedef struct TCell_ {
	TPos x;
	TCoord cover;
	TArea area;
	PCell next;
} TCell;

typedef struct gray_TWorker_ {
	TCoord ex, ey;
	TPos min_ex, max_ex;
	TPos min_ey, max_ey;
	TPos count_ex, count_ey;
	TArea area;
	TCoord cover;
	int invalid;
	PCell cells;
	SW_FT_PtrDist max_cells;
	SW_FT_PtrDist num_cells;
	TPos x, y;
	SW_FT_Vector bez_stack[32 * 3 + 1];
	int lev_stack[32];
	SW_FT_Outline outline;
	SW_FT_BBox clip_box;
	int bound_left;
	int bound_top;
	int bound_right;
	int bound_bottom;
	SW_FT_Span gray_spans[SW_FT_MAX_GRAY_SPANS];
	int num_gray_spans;
	SW_FT_Raster_Span_Func render_span;
	void *render_span_data;
	int band_size;
	int band_shoot;
	ft_jmp_buf jump_buffer;
	void *buffer;
	long buffer_size;
	PCell *ycells;
	TPos ycount;
} gray_TWorker, *gray_PWorker;

#ifndef SW_FT_STATIC_RASTER
#define ras (*worker)
#else
static gray_TWorker ras;
#endif

typedef struct gray_TRaster_ {
	void * memory;
} gray_TRaster, *gray_PRaster;

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
	ras.bound_left = INT_MAX;
	ras.bound_top = INT_MAX;
	ras.bound_right = INT_MIN;
	ras.bound_bottom = INT_MIN;
}

static void gray_compute_cbox(RAS_ARG)
{
	SW_FT_Outline *outline = &ras.outline;
	SW_FT_Vector *vec = outline->points;
	SW_FT_Vector *limit = vec + outline->n_points;

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
		ft_longjmp(ras.jump_buffer, 1);
	cell = ras.cells + ras.num_cells++;
	cell->x = x;
	cell->area = 0;
	cell->cover = 0;
	cell->next = *pcell;
	*pcell = cell;
Exit:
	return cell;
}

static void gray_record_cell(RAS_ARG)
{
	if(ras.area | ras.cover)
	{
		PCell cell = gray_find_cell(RAS_VAR);
		cell->area += ras.area;
		cell->cover += ras.cover;
	}
}

static void gray_set_cell(RAS_ARG_ TCoord ex, TCoord ey)
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
			gray_record_cell(RAS_VAR);
		ras.area = 0;
		ras.cover = 0;
		ras.ex = ex;
		ras.ey = ey;
	}
	ras.invalid = ((unsigned)ey >= (unsigned)ras.count_ey || ex >= ras.count_ex);
}

static void gray_start_cell(RAS_ARG_ TCoord ex, TCoord ey)
{
	if(ex > ras.max_ex)
		ex = (TCoord)(ras.max_ex);
	if(ex < ras.min_ex)
		ex = (TCoord)(ras.min_ex - 1);
	ras.area = 0;
	ras.cover = 0;
	ras.ex = ex - ras.min_ex;
	ras.ey = ey - ras.min_ey;
	ras.invalid = 0;
	gray_set_cell(RAS_VAR_ ex, ey);
}

static void gray_render_line(RAS_ARG_ TPos to_x, TPos to_y)
{
	TPos dx, dy, fx1, fy1, fx2, fy2;
	TCoord ex1, ex2, ey1, ey2;

	ex1 = TRUNC(ras.x);
	ex2 = TRUNC(to_x);
	ey1 = TRUNC(ras.y);
	ey2 = TRUNC(to_y);
	if((ey1 >= ras.max_ey && ey2 >= ras.max_ey) || (ey1 < ras.min_ey && ey2 < ras.min_ey))
		goto End;
	dx = to_x - ras.x;
	dy = to_y - ras.y;
	fx1 = ras.x - SUBPIXELS(ex1);
	fy1 = ras.y - SUBPIXELS(ey1);
	if(ex1 == ex2 && ey1 == ey2)
		;
	else if(dy == 0)
	{
		ex1 = ex2;
		gray_set_cell(RAS_VAR_ ex1, ey1);
	}
	else if(dx == 0)
	{
		if(dy > 0)
			do {
				fy2 = ONE_PIXEL;
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * fx1 * 2;
				fy1 = 0;
				ey1++;
				gray_set_cell(RAS_VAR_ ex1, ey1);
			} while(ey1 != ey2);
		else
			do {
				fy2 = 0;
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * fx1 * 2;
				fy1 = ONE_PIXEL;
				ey1--;
				gray_set_cell(RAS_VAR_ ex1, ey1);
			} while(ey1 != ey2);
	}
	else
	{
		TArea prod = dx * fy1 - dy * fx1;
		SW_FT_UDIVPREP(dx);
		SW_FT_UDIVPREP(dy);
		do {
			if(prod <= 0 && prod - dx * ONE_PIXEL > 0)
			{
				fx2 = 0;
				fy2 = (TPos)SW_FT_UDIV(-prod, -dx);
				prod -= dy * ONE_PIXEL;
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * (fx1 + fx2);
				fx1 = ONE_PIXEL;
				fy1 = fy2;
				ex1--;
			}
			else if(prod - dx * ONE_PIXEL <= 0 && prod - dx * ONE_PIXEL + dy * ONE_PIXEL > 0)
			{
				prod -= dx * ONE_PIXEL;
				fx2 = (TPos)SW_FT_UDIV(-prod, dy);
				fy2 = ONE_PIXEL;
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * (fx1 + fx2);
				fx1 = fx2;
				fy1 = 0;
				ey1++;
			}
			else if(prod - dx * ONE_PIXEL + dy * ONE_PIXEL <= 0 && prod + dy * ONE_PIXEL >= 0)
			{
				prod += dy * ONE_PIXEL;
				fx2 = ONE_PIXEL;
				fy2 = (TPos)SW_FT_UDIV(prod, dx);
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * (fx1 + fx2);
				fx1 = 0;
				fy1 = fy2;
				ex1++;
			}
			else
			{
				fx2 = (TPos)SW_FT_UDIV(prod, -dy);
				fy2 = 0;
				prod += dx * ONE_PIXEL;
				ras.cover += (fy2 - fy1);
				ras.area += (fy2 - fy1) * (fx1 + fx2);
				fx1 = fx2;
				fy1 = ONE_PIXEL;
				ey1--;
			}
			gray_set_cell(RAS_VAR_ ex1, ey1);
		} while(ex1 != ex2 || ey1 != ey2);
	}
	fx2 = to_x - SUBPIXELS(ex2);
	fy2 = to_y - SUBPIXELS(ey2);
	ras.cover += (fy2 - fy1);
	ras.area += (fy2 - fy1) * (fx1 + fx2);
End:
	ras.x = to_x;
	ras.y = to_y;
}

static void gray_split_conic(SW_FT_Vector * base)
{
	TPos a, b;

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

static void gray_render_conic(RAS_ARG_ const SW_FT_Vector * control, const SW_FT_Vector * to)
{
	TPos dx, dy;
	TPos min, max, y;
	int top, level;
	int *levels;
	SW_FT_Vector *arc;

	levels = ras.lev_stack;
	arc = ras.bez_stack;
	arc[0].x = UPSCALE(to->x);
	arc[0].y = UPSCALE(to->y);
	arc[1].x = UPSCALE(control->x);
	arc[1].y = UPSCALE(control->y);
	arc[2].x = ras.x;
	arc[2].y = ras.y;
	top = 0;
	dx = SW_FT_ABS(arc[2].x + arc[0].x - 2 * arc[1].x);
	dy = SW_FT_ABS(arc[2].y + arc[0].y - 2 * arc[1].y);
	if(dx < dy)
		dx = dy;
	if(dx < ONE_PIXEL / 4)
		goto Draw;
	min = max = arc[0].y;
	y = arc[1].y;
	if(y < min)
		min = y;
	if(y > max)
		max = y;
	y = arc[2].y;
	if(y < min)
		min = y;
	if(y > max)
		max = y;
	if(TRUNC(min) >= ras.max_ey || TRUNC(max) < ras.min_ey)
		goto Draw;
	level = 0;
	do {
		dx >>= 2;
		level++;
	} while(dx > ONE_PIXEL / 4);
	levels[0] = level;
	do {
		level = levels[top];
		if(level > 0)
		{
			gray_split_conic(arc);
			arc += 2;
			top++;
			levels[top] = levels[top - 1] = level - 1;
			continue;
		}
Draw:
		gray_render_line(RAS_VAR_ arc[0].x, arc[0].y);
		top--;
		arc -= 2;
	} while(top >= 0);
}

static void gray_split_cubic(SW_FT_Vector * base)
{
	TPos a, b, c;

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

static void gray_render_cubic(RAS_ARG_ const SW_FT_Vector * control1, const SW_FT_Vector * control2, const SW_FT_Vector * to)
{
	SW_FT_Vector * arc = ras.bez_stack;

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
		if( SW_FT_ABS( 2 * arc[0].x - 3 * arc[1].x + arc[3].x ) > ONE_PIXEL / 2 ||
			SW_FT_ABS( 2 * arc[0].y - 3 * arc[1].y + arc[3].y ) > ONE_PIXEL / 2 ||
			SW_FT_ABS( arc[0].x - 3 * arc[2].x + 2 * arc[3].x ) > ONE_PIXEL / 2 ||
			SW_FT_ABS( arc[0].y - 3 * arc[2].y + 2 * arc[3].y ) > ONE_PIXEL / 2)
			goto Split;
		gray_render_line( RAS_VAR_ arc[0].x, arc[0].y);
		if(arc == ras.bez_stack)
			return;
		arc -= 3;
		continue;
Split:
		gray_split_cubic(arc);
		arc += 3;
	}
}

static int gray_move_to(const SW_FT_Vector * to, gray_PWorker worker)
{
	TPos x, y;

	if(!ras.invalid)
		gray_record_cell(RAS_VAR);
	x = UPSCALE(to->x);
	y = UPSCALE(to->y);
	gray_start_cell(RAS_VAR_ TRUNC(x), TRUNC(y));
	worker->x = x;
	worker->y = y;
	return 0;
}

static int gray_line_to(const SW_FT_Vector *to, gray_PWorker worker)
{
	gray_render_line(RAS_VAR_ UPSCALE(to->x), UPSCALE(to->y));
	return 0;
}

static int gray_conic_to(const SW_FT_Vector * control, const SW_FT_Vector * to, gray_PWorker worker)
{
	gray_render_conic(RAS_VAR_ control, to);
	return 0;
}

static int gray_cubic_to(const SW_FT_Vector * control1, const SW_FT_Vector * control2, const SW_FT_Vector * to, gray_PWorker worker)
{
	gray_render_cubic(RAS_VAR_ control1, control2, to);
	return 0;
}

static void gray_hline(RAS_ARG_ TCoord x, TCoord y, TPos area, TCoord acount)
{
	int coverage;

	coverage = (int)(area >> (PIXEL_BITS * 2 + 1 - 8));
	if(coverage < 0)
		coverage = -coverage;
	if(ras.outline.flags & SW_FT_OUTLINE_EVEN_ODD_FILL)
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
	if(x >= 32767)
		x = 32767;
	if(y >= SW_FT_INT_MAX)
		y = SW_FT_INT_MAX;
	if(coverage)
	{
		SW_FT_Span * span;
		int count;

		if(x < ras.bound_left)
			ras.bound_left = x;
		if(y < ras.bound_top)
			ras.bound_top = y;
		if(y > ras.bound_bottom)
			ras.bound_bottom = y;
		if(x + acount > ras.bound_right)
			ras.bound_right = x + acount;
		count = ras.num_gray_spans;
		span = ras.gray_spans + count - 1;
		if(count > 0 && span->y == y && (int)span->x + span->len == (int)x && span->coverage == coverage)
		{
			span->len = (unsigned short)(span->len + acount);
			return;
		}
		if(count >= SW_FT_MAX_GRAY_SPANS)
		{
			if(ras.render_span && count > 0)
				ras.render_span(count, ras.gray_spans, ras.render_span_data);
			ras.num_gray_spans = 0;
			span = ras.gray_spans;
		}
		else
			span++;
		span->x = (short)x;
		span->y = (short)y;
		span->len = (unsigned short)acount;
		span->coverage = (unsigned char)coverage;
		ras.num_gray_spans++;
	}
}

static void gray_sweep(RAS_ARG)
{
	int yindex;

	if(ras.num_cells == 0)
		return;
	ras.num_gray_spans = 0;
	for(yindex = 0; yindex < ras.ycount; yindex++)
	{
		PCell cell = ras.ycells[yindex];
		TCoord cover = 0;
		TCoord x = 0;

		for(; cell != NULL; cell = cell->next)
		{
			TPos area;
			if(cell->x > x && cover != 0)
				gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2), cell->x - x);
			cover += cell->cover;
			area = cover * (ONE_PIXEL * 2) - cell->area;
			if(area != 0 && cell->x >= 0)
				gray_hline(RAS_VAR_ cell->x, yindex, area, 1);
			x = cell->x + 1;
		}
		if(cover != 0)
			gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2),
			ras.count_ex - x);
	}
	if(ras.render_span && ras.num_gray_spans > 0)
		ras.render_span(ras.num_gray_spans, ras.gray_spans, ras.render_span_data);
}

static int SW_FT_Outline_Decompose(const SW_FT_Outline * outline, const SW_FT_Outline_Funcs * func_interface, void * user)
{
#undef SCALED
#define SCALED(x) (((x) << shift) - delta)
	SW_FT_Vector v_last;
	SW_FT_Vector v_control;
	SW_FT_Vector v_start;
	SW_FT_Vector *point;
	SW_FT_Vector *limit;
	char *tags;
	int error;
	int n;
	int first;
	char tag;
	int shift;
	TPos delta;

	if(!outline || !func_interface)
		return SW_FT_THROW(Invalid_Argument);
	shift = func_interface->shift;
	delta = func_interface->delta;
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
		tag = SW_FT_CURVE_TAG(tags[0]);
		if(tag == SW_FT_CURVE_TAG_CUBIC)
			goto Invalid_Outline;
		if(tag == SW_FT_CURVE_TAG_CONIC)
		{
			if(SW_FT_CURVE_TAG(outline->tags[last]) == SW_FT_CURVE_TAG_ON)
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
		error = func_interface->move_to(&v_start, user);
		if(error)
			goto Exit;
		while(point < limit)
		{
			point++;
			tags++;
			tag = SW_FT_CURVE_TAG(tags[0]);
			switch(tag)
			{
			case SW_FT_CURVE_TAG_ON:
			{
				SW_FT_Vector vec;
				vec.x = SCALED(point->x);
				vec.y = SCALED(point->y);
				error = func_interface->line_to(&vec, user);
				if(error)
					goto Exit;
				continue;
			}
			case SW_FT_CURVE_TAG_CONIC:
				v_control.x = SCALED(point->x);
				v_control.y = SCALED(point->y);
Do_Conic:
				if(point < limit)
				{
					SW_FT_Vector vec;
					SW_FT_Vector v_middle;
					point++;
					tags++;
					tag = SW_FT_CURVE_TAG(tags[0]);
					vec.x = SCALED(point->x);
					vec.y = SCALED(point->y);
					if(tag == SW_FT_CURVE_TAG_ON)
					{
						error = func_interface->conic_to(&v_control, &vec, user);
						if(error)
							goto Exit;
						continue;
					}
					if(tag != SW_FT_CURVE_TAG_CONIC)
						goto Invalid_Outline;
					v_middle.x = (v_control.x + vec.x) / 2;
					v_middle.y = (v_control.y + vec.y) / 2;
					error = func_interface->conic_to(&v_control, &v_middle, user);
					if(error)
						goto Exit;
					v_control = vec;
					goto Do_Conic;
				}
				error = func_interface->conic_to(&v_control, &v_start, user);
				goto Close;
			default:
			{
				SW_FT_Vector vec1, vec2;
				if(point + 1 > limit ||
				SW_FT_CURVE_TAG(tags[1]) != SW_FT_CURVE_TAG_CUBIC)
					goto Invalid_Outline;
				point += 2;
				tags += 2;
				vec1.x = SCALED(point[-2].x);
				vec1.y = SCALED(point[-2].y);
				vec2.x = SCALED(point[-1].x);
				vec2.y = SCALED(point[-1].y);
				if(point <= limit)
				{
					SW_FT_Vector vec;
					vec.x = SCALED(point->x);
					vec.y = SCALED(point->y);
					error = func_interface->cubic_to(&vec1, &vec2, &vec, user);
					if(error)
						goto Exit;
					continue;
				}
				error = func_interface->cubic_to(&vec1, &vec2, &v_start, user);
				goto Close;
			}
			}
		}
		error = func_interface->line_to(&v_start, user);
Close:
		if(error)
			goto Exit;
		first = last + 1;
	}
	return 0;
Exit:
	return error;
Invalid_Outline:
	return SW_FT_THROW(Invalid_Outline);
}

typedef struct gray_TBand_ {
	TPos min, max;
} gray_TBand;

SW_FT_DEFINE_OUTLINE_FUNCS(func_interface,
		(SW_FT_Outline_MoveTo_Func)gray_move_to,
		(SW_FT_Outline_LineTo_Func)gray_line_to,
		(SW_FT_Outline_ConicTo_Func)gray_conic_to,
		(SW_FT_Outline_CubicTo_Func)gray_cubic_to, 0, 0)

static int gray_convert_glyph_inner(RAS_ARG)
{
	volatile int error = 0;

	if(ft_setjmp(ras.jump_buffer) == 0)
	{
		error = SW_FT_Outline_Decompose(&ras.outline, &func_interface, &ras);
		if(!ras.invalid)
			gray_record_cell(RAS_VAR);
	}
	else
		error = SW_FT_THROW(Memory_Overflow);
	return error;
}

static int gray_convert_glyph(RAS_ARG)
{
	gray_TBand bands[40];
	gray_TBand *volatile band;
	int volatile n, num_bands;
	TPos volatile min, max, max_y;
	SW_FT_BBox *clip;

	gray_compute_cbox(RAS_VAR);
	clip = &ras.clip_box;
	if(ras.max_ex <= clip->xMin || ras.min_ex >= clip->xMax ||
	ras.max_ey <= clip->yMin || ras.min_ey >= clip->yMax)
		return 0;
	if(ras.min_ex < clip->xMin)
		ras.min_ex = clip->xMin;
	if(ras.min_ey < clip->yMin)
		ras.min_ey = clip->yMin;
	if(ras.max_ex > clip->xMax)
		ras.max_ex = clip->xMax;
	if(ras.max_ey > clip->yMax)
		ras.max_ey = clip->yMax;
	ras.count_ex = ras.max_ex - ras.min_ex;
	ras.count_ey = ras.max_ey - ras.min_ey;
	num_bands = (int)((ras.max_ey - ras.min_ey) / ras.band_size);
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
				long cell_start, cell_end, cell_mod;
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
				if(ras.cells >= cells_max)
					goto ReduceBands;
				ras.max_cells = cells_max - ras.cells;
				if(ras.max_cells < 2)
					goto ReduceBands;
				for(yindex = 0; yindex < ras.ycount; yindex++)
					ras.ycells[yindex] = NULL;
			}
			ras.num_cells = 0;
			ras.invalid = 1;
			ras.min_ey = band->min;
			ras.max_ey = band->max;
			ras.count_ey = band->max - band->min;
			error = gray_convert_glyph_inner(RAS_VAR);
			if(!error)
			{
				gray_sweep(RAS_VAR);
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
				return 1;
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
	if(ras.band_shoot > 8 && ras.band_size > 16)
		ras.band_size = ras.band_size / 2;
	return 0;
}

static int gray_raster_render(gray_PRaster raster, const SW_FT_Raster_Params * params)
{
	SW_FT_UNUSED(raster);
	const SW_FT_Outline * outline = (const SW_FT_Outline*)params->source;
	gray_TWorker worker[1];
	TCell buffer[SW_FT_RENDER_POOL_SIZE / sizeof(TCell)];
	long buffer_size = sizeof(buffer);
	int band_size = (int)(buffer_size / (long)(sizeof(TCell) * 8));
	if(!outline)
		return SW_FT_THROW(Invalid_Outline);
	if(outline->n_points == 0 || outline->n_contours <= 0)
		return 0;
	if(!outline->contours || !outline->points)
		return SW_FT_THROW(Invalid_Outline);
	if(outline->n_points != outline->contours[outline->n_contours - 1] + 1)
		return SW_FT_THROW(Invalid_Outline);
	if(!(params->flags & SW_FT_RASTER_FLAG_AA))
		return SW_FT_THROW(Invalid_Mode);
	if(params->flags & SW_FT_RASTER_FLAG_CLIP)
		ras.clip_box = params->clip_box;
	else
	{
		ras.clip_box.xMin = -32768L;
		ras.clip_box.yMin = -32768L;
		ras.clip_box.xMax = 32767L;
		ras.clip_box.yMax = 32767L;
	}
	gray_init_cells(RAS_VAR_ buffer, buffer_size);
	ras.outline = *outline;
	ras.num_cells = 0;
	ras.invalid = 1;
	ras.band_size = band_size;
	ras.num_gray_spans = 0;
	ras.render_span = (SW_FT_Raster_Span_Func)params->gray_spans;
	ras.render_span_data = params->user;
	gray_convert_glyph(RAS_VAR);
	params->bbox_cb(ras.bound_left, ras.bound_top,
	ras.bound_right - ras.bound_left,
	ras.bound_bottom - ras.bound_top + 1, params->user);
	return 1;
}

static int gray_raster_new(SW_FT_Raster *araster)
{
	static gray_TRaster the_raster;
	*araster = (SW_FT_Raster)&the_raster;
	SW_FT_MEM_ZERO(&the_raster, sizeof(the_raster));
	return 0;
}

static void gray_raster_done(SW_FT_Raster raster)
{
	SW_FT_UNUSED(raster);
}

static void gray_raster_reset(SW_FT_Raster raster, char *pool_base, long pool_size)
{
	SW_FT_UNUSED(raster);
	SW_FT_UNUSED(pool_base);
	SW_FT_UNUSED(pool_size);
}

SW_FT_DEFINE_RASTER_FUNCS(sw_ft_grays_raster,
	(SW_FT_Raster_New_Func)gray_raster_new,
	(SW_FT_Raster_Reset_Func)gray_raster_reset,
	(SW_FT_Raster_Render_Func)gray_raster_render,
	(SW_FT_Raster_Done_Func)gray_raster_done)

/*
 * stroker
 */
#define SW_FT_SMALL_CONIC_THRESHOLD	(SW_FT_ANGLE_PI / 6)
#define SW_FT_SMALL_CUBIC_THRESHOLD	(SW_FT_ANGLE_PI / 8)
#define SW_FT_IS_SMALL(x)			((x) > -2 && (x) < 2)

static SW_FT_Pos ft_pos_abs(SW_FT_Pos x)
{
	return x >= 0 ? x : -x;
}

static void ft_conic_split(SW_FT_Vector * base)
{
	SW_FT_Pos a, b;

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

static SW_FT_Bool ft_conic_is_small_enough(SW_FT_Vector * base, SW_FT_Angle * angle_in, SW_FT_Angle * angle_out)
{
	SW_FT_Vector d1, d2;
	SW_FT_Angle theta;
	SW_FT_Int close1, close2;

	d1.x = base[1].x - base[2].x;
	d1.y = base[1].y - base[2].y;
	d2.x = base[0].x - base[1].x;
	d2.y = base[0].y - base[1].y;
	close1 = SW_FT_IS_SMALL(d1.x) && SW_FT_IS_SMALL(d1.y);
	close2 = SW_FT_IS_SMALL(d2.x) && SW_FT_IS_SMALL(d2.y);

	if(close1)
	{
		if(close2)
		{
		}
		else
		{
			*angle_in = *angle_out = SW_FT_Atan2(d2.x, d2.y);
		}
	}
	else
	{
		if(close2)
		{
			*angle_in = *angle_out = SW_FT_Atan2(d1.x, d1.y);
		}
		else
		{
			*angle_in = SW_FT_Atan2(d1.x, d1.y);
			*angle_out = SW_FT_Atan2(d2.x, d2.y);
		}
	}
	theta = ft_pos_abs(SW_FT_Angle_Diff(*angle_in, *angle_out));
	return SW_FT_BOOL(theta < SW_FT_SMALL_CONIC_THRESHOLD);
}

static void ft_cubic_split(SW_FT_Vector * base)
{
	SW_FT_Pos a, b, c;

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

static SW_FT_Angle ft_angle_mean(SW_FT_Angle angle1, SW_FT_Angle angle2)
{
	return angle1 + SW_FT_Angle_Diff(angle1, angle2) / 2;
}

static SW_FT_Bool ft_cubic_is_small_enough(SW_FT_Vector * base, SW_FT_Angle * angle_in, SW_FT_Angle * angle_mid, SW_FT_Angle * angle_out)
{
	SW_FT_Vector d1, d2, d3;
	SW_FT_Angle theta1, theta2;
	SW_FT_Int close1, close2, close3;

	d1.x = base[2].x - base[3].x;
	d1.y = base[2].y - base[3].y;
	d2.x = base[1].x - base[2].x;
	d2.y = base[1].y - base[2].y;
	d3.x = base[0].x - base[1].x;
	d3.y = base[0].y - base[1].y;
	close1 = SW_FT_IS_SMALL(d1.x) && SW_FT_IS_SMALL(d1.y);
	close2 = SW_FT_IS_SMALL(d2.x) && SW_FT_IS_SMALL(d2.y);
	close3 = SW_FT_IS_SMALL(d3.x) && SW_FT_IS_SMALL(d3.y);

	if(close1)
	{
		if(close2)
		{
			if(close3)
			{
			}
			else
			{
				*angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d3.x, d3.y);
			}
		}
		else
		{
			if(close3)
			{
				*angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d2.x, d2.y);
			}
			else
			{
				*angle_in = *angle_mid = SW_FT_Atan2(d2.x, d2.y);
				*angle_out = SW_FT_Atan2(d3.x, d3.y);
			}
		}
	}
	else
	{
		if(close2)
		{
			if(close3)
			{
				*angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d1.x, d1.y);
			}
			else
			{
				*angle_in = SW_FT_Atan2(d1.x, d1.y);
				*angle_out = SW_FT_Atan2(d3.x, d3.y);
				*angle_mid = ft_angle_mean(*angle_in, *angle_out);
			}
		}
		else
		{
			if(close3)
			{
				*angle_in = SW_FT_Atan2(d1.x, d1.y);
				*angle_mid = *angle_out = SW_FT_Atan2(d2.x, d2.y);
			}
			else
			{
				*angle_in = SW_FT_Atan2(d1.x, d1.y);
				*angle_mid = SW_FT_Atan2(d2.x, d2.y);
				*angle_out = SW_FT_Atan2(d3.x, d3.y);
			}
		}
	}
	theta1 = ft_pos_abs(SW_FT_Angle_Diff(*angle_in, *angle_mid));
	theta2 = ft_pos_abs(SW_FT_Angle_Diff(*angle_mid, *angle_out));
	return SW_FT_BOOL(theta1 < SW_FT_SMALL_CUBIC_THRESHOLD && theta2 < SW_FT_SMALL_CUBIC_THRESHOLD);
}

typedef enum SW_FT_StrokeTags_ {
	SW_FT_STROKE_TAG_ON = 1,
	SW_FT_STROKE_TAG_CUBIC = 2,
	SW_FT_STROKE_TAG_BEGIN = 4,
	SW_FT_STROKE_TAG_END = 8,
} SW_FT_StrokeTags;

#define SW_FT_STROKE_TAG_BEGIN_END	(SW_FT_STROKE_TAG_BEGIN | SW_FT_STROKE_TAG_END)

typedef struct SW_FT_StrokeBorderRec_ {
	SW_FT_UInt num_points;
	SW_FT_UInt max_points;
	SW_FT_Vector * points;
	SW_FT_Byte * tags;
	SW_FT_Bool movable;
	SW_FT_Int start;
	SW_FT_Bool valid;
} SW_FT_StrokeBorderRec, * SW_FT_StrokeBorder;

SW_FT_Error SW_FT_Outline_Check(SW_FT_Outline * outline)
{
	if(outline)
	{
		SW_FT_Int n_points = outline->n_points;
		SW_FT_Int n_contours = outline->n_contours;
		SW_FT_Int end0, end;
		SW_FT_Int n;

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

void SW_FT_Outline_Get_CBox(const SW_FT_Outline * outline, SW_FT_BBox * acbox)
{
	SW_FT_Pos xMin, yMin, xMax, yMax;

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
			SW_FT_Vector * vec = outline->points;
			SW_FT_Vector * limit = vec + outline->n_points;
			xMin = xMax = vec->x;
			yMin = yMax = vec->y;
			vec++;
			for(; vec < limit; vec++)
			{
				SW_FT_Pos x, y;
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

static SW_FT_Error ft_stroke_border_grow(SW_FT_StrokeBorder border, SW_FT_UInt new_points)
{
	SW_FT_UInt old_max = border->max_points;
	SW_FT_UInt new_max = border->num_points + new_points;
	SW_FT_Error error = 0;

	if(new_max > old_max)
	{
		SW_FT_UInt cur_max = old_max;
		while(cur_max < new_max)
			cur_max += (cur_max >> 1) + 16;
		border->points = (SW_FT_Vector*)realloc(border->points, cur_max * sizeof(SW_FT_Vector));
		border->tags = (SW_FT_Byte*)realloc(border->tags, cur_max * sizeof(SW_FT_Byte));
		if(!border->points || !border->tags)
			goto Exit;
		border->max_points = cur_max;
	}
Exit:
	return error;
}

static void ft_stroke_border_close(SW_FT_StrokeBorder border, SW_FT_Bool reverse)
{
	SW_FT_UInt start = border->start;
	SW_FT_UInt count = border->num_points;

	if(count <= start + 1U)
		border->num_points = start;
	else
	{
		border->num_points = --count;
		border->points[start] = border->points[count];
		if(reverse)
		{
			{
				SW_FT_Vector *vec1 = border->points + start + 1;
				SW_FT_Vector *vec2 = border->points + count - 1;
				for(; vec1 < vec2; vec1++, vec2--)
				{
					SW_FT_Vector tmp;
					tmp = *vec1;
					*vec1 = *vec2;
					*vec2 = tmp;
				}
			}
			{
				SW_FT_Byte *tag1 = border->tags + start + 1;
				SW_FT_Byte *tag2 = border->tags + count - 1;
				for(; tag1 < tag2; tag1++, tag2--)
				{
					SW_FT_Byte tmp;
					tmp = *tag1;
					*tag1 = *tag2;
					*tag2 = tmp;
				}
			}
		}
		border->tags[start] |= SW_FT_STROKE_TAG_BEGIN;
		border->tags[count - 1] |= SW_FT_STROKE_TAG_END;
	}
	border->start = -1;
	border->movable = FALSE;
}

static SW_FT_Error ft_stroke_border_lineto(SW_FT_StrokeBorder border, SW_FT_Vector * to, SW_FT_Bool movable)
{
	SW_FT_Error error = 0;

	if(border->movable)
	{
		border->points[border->num_points - 1] = *to;
	}
	else
	{
		if(border->num_points > 0&&
		SW_FT_IS_SMALL(border->points[border->num_points - 1].x - to->x) &&
		SW_FT_IS_SMALL(border->points[border->num_points - 1].y - to->y))
			return error;
		error = ft_stroke_border_grow(border, 1);
		if(!error)
		{
			SW_FT_Vector *vec = border->points + border->num_points;
			SW_FT_Byte *tag = border->tags + border->num_points;
			vec[0] = *to;
			tag[0] = SW_FT_STROKE_TAG_ON;
			border->num_points += 1;
		}
	}
	border->movable = movable;
	return error;
}

static SW_FT_Error ft_stroke_border_conicto(SW_FT_StrokeBorder border, SW_FT_Vector * control, SW_FT_Vector * to)
{
	SW_FT_Error error;

	error = ft_stroke_border_grow(border, 2);
	if(!error)
	{
		SW_FT_Vector *vec = border->points + border->num_points;
		SW_FT_Byte *tag = border->tags + border->num_points;
		vec[0] = *control;
		vec[1] = *to;
		tag[0] = 0;
		tag[1] = SW_FT_STROKE_TAG_ON;
		border->num_points += 2;
	}
	border->movable = FALSE;
	return error;
}

static SW_FT_Error ft_stroke_border_cubicto(SW_FT_StrokeBorder border, SW_FT_Vector * control1, SW_FT_Vector * control2, SW_FT_Vector * to)
{
	SW_FT_Error error;

	error = ft_stroke_border_grow(border, 3);
	if(!error)
	{
		SW_FT_Vector *vec = border->points + border->num_points;
		SW_FT_Byte *tag = border->tags + border->num_points;
		vec[0] = *control1;
		vec[1] = *control2;
		vec[2] = *to;
		tag[0] = SW_FT_STROKE_TAG_CUBIC;
		tag[1] = SW_FT_STROKE_TAG_CUBIC;
		tag[2] = SW_FT_STROKE_TAG_ON;
		border->num_points += 3;
	}
	border->movable = FALSE;
	return error;
}

#define SW_FT_ARC_CUBIC_ANGLE	(SW_FT_ANGLE_PI / 2)

static SW_FT_Error ft_stroke_border_arcto(SW_FT_StrokeBorder border, SW_FT_Vector * center, SW_FT_Fixed radius, SW_FT_Angle angle_start, SW_FT_Angle angle_diff)
{
	SW_FT_Fixed coef;
	SW_FT_Vector a0, a1, a2, a3;
	SW_FT_Int i, arcs = 1;
	SW_FT_Error error = 0;

	while(angle_diff > SW_FT_ARC_CUBIC_ANGLE * arcs || -angle_diff > SW_FT_ARC_CUBIC_ANGLE * arcs)
		arcs++;
	coef = SW_FT_Tan(angle_diff / (4 * arcs));
	coef += coef / 3;
	SW_FT_Vector_From_Polar(&a0, radius, angle_start);
	a1.x = SW_FT_MulFix(-a0.y, coef);
	a1.y = SW_FT_MulFix(a0.x, coef);
	a0.x += center->x;
	a0.y += center->y;
	a1.x += a0.x;
	a1.y += a0.y;
	for(i = 1; i <= arcs; i++)
	{
		SW_FT_Vector_From_Polar(&a3, radius, angle_start + i * angle_diff / arcs);
		a2.x = SW_FT_MulFix(a3.y, coef);
		a2.y = SW_FT_MulFix(-a3.x, coef);
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

static SW_FT_Error ft_stroke_border_moveto(SW_FT_StrokeBorder border, SW_FT_Vector * to)
{
	if(border->start >= 0)
		ft_stroke_border_close(border, FALSE);
	border->start = border->num_points;
	border->movable = FALSE;
	return ft_stroke_border_lineto(border, to, FALSE);
}

static void ft_stroke_border_init(SW_FT_StrokeBorder border)
{
	border->points = NULL;
	border->tags = NULL;
	border->num_points = 0;
	border->max_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static void ft_stroke_border_reset(SW_FT_StrokeBorder border)
{
	border->num_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static void ft_stroke_border_done(SW_FT_StrokeBorder border)
{
	free(border->points);
	free(border->tags);

	border->num_points = 0;
	border->max_points = 0;
	border->start = -1;
	border->valid = FALSE;
}

static SW_FT_Error ft_stroke_border_get_counts(SW_FT_StrokeBorder border, SW_FT_UInt * anum_points, SW_FT_UInt * anum_contours)
{
	SW_FT_Error error = 0;
	SW_FT_UInt num_points = 0;
	SW_FT_UInt num_contours = 0;
	SW_FT_UInt count = border->num_points;
	SW_FT_Vector *point = border->points;
	SW_FT_Byte *tags = border->tags;
	SW_FT_Int in_contour = 0;

	for(; count > 0; count--, num_points++, point++, tags++)
	{
		if(tags[0] & SW_FT_STROKE_TAG_BEGIN)
		{
			if(in_contour != 0)
				goto Fail;
			in_contour = 1;
		}
		else if(in_contour == 0)
			goto Fail;
		if(tags[0] & SW_FT_STROKE_TAG_END)
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

static void ft_stroke_border_export(SW_FT_StrokeBorder border, SW_FT_Outline *outline)
{
	memcpy(outline->points + outline->n_points, border->points, border->num_points * sizeof(SW_FT_Vector));
	{
		SW_FT_UInt count = border->num_points;
		SW_FT_Byte *read = border->tags;
		SW_FT_Byte *write = (SW_FT_Byte*)outline->tags + outline->n_points;
		for(; count > 0; count--, read++, write++)
		{
			if(*read & SW_FT_STROKE_TAG_ON)
				*write = SW_FT_CURVE_TAG_ON;
			else if(*read & SW_FT_STROKE_TAG_CUBIC)
				*write = SW_FT_CURVE_TAG_CUBIC;
			else
				*write = SW_FT_CURVE_TAG_CONIC;
		}
	}
	{
		SW_FT_UInt count = border->num_points;
		SW_FT_Byte *tags = border->tags;
		SW_FT_Short *write = outline->contours + outline->n_contours;
		SW_FT_Short idx = (SW_FT_Short)outline->n_points;
		for(; count > 0; count--, tags++, idx++)
		{
			if(*tags & SW_FT_STROKE_TAG_END)
			{
				*write++ = idx;
				outline->n_contours++;
			}
		}
	}
	outline->n_points = (short)(outline->n_points + border->num_points);
	SW_FT_Outline_Check(outline);
}

#define SW_FT_SIDE_TO_ROTATE(s)		(SW_FT_ANGLE_PI2 - (s) * SW_FT_ANGLE_PI)

typedef struct SW_FT_StrokerRec_ {
	SW_FT_Angle angle_in;
	SW_FT_Angle angle_out;
	SW_FT_Vector center;
	SW_FT_Fixed line_length;
	SW_FT_Bool first_point;
	SW_FT_Bool subpath_open;
	SW_FT_Angle subpath_angle;
	SW_FT_Vector subpath_start;
	SW_FT_Fixed subpath_line_length;
	SW_FT_Bool handle_wide_strokes;
	SW_FT_Stroker_LineCap line_cap;
	SW_FT_Stroker_LineJoin line_join;
	SW_FT_Stroker_LineJoin line_join_saved;
	SW_FT_Fixed miter_limit;
	SW_FT_Fixed radius;
	SW_FT_StrokeBorderRec borders[2];
} SW_FT_StrokerRec;

SW_FT_Error SW_FT_Stroker_New(SW_FT_Stroker * astroker)
{
	SW_FT_Error error = 0;
	SW_FT_Stroker stroker = NULL;
	stroker = (SW_FT_StrokerRec*)calloc(1, sizeof(SW_FT_StrokerRec));
	if(stroker)
	{
		ft_stroke_border_init(&stroker->borders[0]);
		ft_stroke_border_init(&stroker->borders[1]);
	}
	*astroker = stroker;
	return error;
}

void SW_FT_Stroker_Rewind(SW_FT_Stroker stroker)
{
	if(stroker)
	{
		ft_stroke_border_reset(&stroker->borders[0]);
		ft_stroke_border_reset(&stroker->borders[1]);
	}
}

void SW_FT_Stroker_Set(SW_FT_Stroker stroker, SW_FT_Fixed radius, SW_FT_Stroker_LineCap line_cap, SW_FT_Stroker_LineJoin line_join, SW_FT_Fixed miter_limit)
{
	stroker->radius = radius;
	stroker->line_cap = line_cap;
	stroker->line_join = line_join;
	stroker->miter_limit = miter_limit;
	if(stroker->miter_limit < 0x10000)
		stroker->miter_limit = 0x10000;
	stroker->line_join_saved = line_join;
	SW_FT_Stroker_Rewind(stroker);
}

void SW_FT_Stroker_Done(SW_FT_Stroker stroker)
{
	if(stroker)
	{
		ft_stroke_border_done(&stroker->borders[0]);
		ft_stroke_border_done(&stroker->borders[1]);

		free(stroker);
	}
}

static SW_FT_Error ft_stroker_arcto(SW_FT_Stroker stroker, SW_FT_Int side)
{
	SW_FT_Angle total, rotate;
	SW_FT_Fixed radius = stroker->radius;
	SW_FT_Error error = 0;
	SW_FT_StrokeBorder border = stroker->borders + side;

	rotate = SW_FT_SIDE_TO_ROTATE(side);
	total = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
	if(total == SW_FT_ANGLE_PI)
		total = -rotate * 2;
	error = ft_stroke_border_arcto(border, &stroker->center, radius, stroker->angle_in + rotate, total);
	border->movable = FALSE;
	return error;
}

static SW_FT_Error ft_stroker_cap(SW_FT_Stroker stroker, SW_FT_Angle angle, SW_FT_Int side)
{
	SW_FT_Error error = 0;

	if(stroker->line_cap == SW_FT_STROKER_LINECAP_ROUND)
	{
		stroker->angle_in = angle;
		stroker->angle_out = angle + SW_FT_ANGLE_PI;
		error = ft_stroker_arcto(stroker, side);
	}
	else
	{
		SW_FT_Vector middle, delta;
		SW_FT_Fixed radius = stroker->radius;
		SW_FT_StrokeBorder border = stroker->borders + side;
		SW_FT_Vector_From_Polar(&middle, radius, angle);
		delta.x = side ? middle.y : -middle.y;
		delta.y = side ? -middle.x : middle.x;
		if(stroker->line_cap == SW_FT_STROKER_LINECAP_SQUARE)
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

static SW_FT_Error ft_stroker_inside(SW_FT_Stroker stroker, SW_FT_Int side, SW_FT_Fixed line_length)
{
	SW_FT_StrokeBorder border = stroker->borders + side;
	SW_FT_Angle phi, theta, rotate;
	SW_FT_Fixed length;
	SW_FT_Vector sigma, delta;
	SW_FT_Error error = 0;
	SW_FT_Bool intersect;

	rotate = SW_FT_SIDE_TO_ROTATE(side);
	theta = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;

	if(!border->movable || line_length == 0 || theta > 0x59C000 || theta < -0x59C000)
		intersect = FALSE;
	else
	{
		SW_FT_Fixed min_length;
		SW_FT_Vector_Unit(&sigma, theta);
		min_length = ft_pos_abs(SW_FT_MulDiv(stroker->radius, sigma.y, sigma.x));
		intersect = SW_FT_BOOL(min_length && stroker->line_length >= min_length && line_length >= min_length);
	}
	if(!intersect)
	{
		SW_FT_Vector_From_Polar(&delta, stroker->radius, stroker->angle_out + rotate);
		delta.x += stroker->center.x;
		delta.y += stroker->center.y;
		border->movable = FALSE;
	}
	else
	{
		phi = stroker->angle_in + theta + rotate;
		length = SW_FT_DivFix(stroker->radius, sigma.x);
		SW_FT_Vector_From_Polar(&delta, length, phi);
		delta.x += stroker->center.x;
		delta.y += stroker->center.y;
	}
	error = ft_stroke_border_lineto(border, &delta, FALSE);
	return error;
}

static SW_FT_Error ft_stroker_outside(SW_FT_Stroker stroker, SW_FT_Int side, SW_FT_Fixed line_length)
{
	SW_FT_StrokeBorder border = stroker->borders + side;
	SW_FT_Error error;
	SW_FT_Angle rotate;

	if(stroker->line_join == SW_FT_STROKER_LINEJOIN_ROUND)
		error = ft_stroker_arcto(stroker, side);
	else
	{
		SW_FT_Fixed radius = stroker->radius;
		SW_FT_Vector sigma;
		SW_FT_Angle theta = 0, phi = 0;
		SW_FT_Bool bevel, fixed_bevel;

		rotate = SW_FT_SIDE_TO_ROTATE(side);
		bevel = SW_FT_BOOL(stroker->line_join == SW_FT_STROKER_LINEJOIN_BEVEL);
		fixed_bevel = SW_FT_BOOL(stroker->line_join != SW_FT_STROKER_LINEJOIN_MITER_VARIABLE);

		if(!bevel)
		{
			theta = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;
			if(theta == SW_FT_ANGLE_PI2)
				theta = -rotate;
			phi = stroker->angle_in + theta + rotate;
			SW_FT_Vector_From_Polar(&sigma, stroker->miter_limit, theta);
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
				SW_FT_Vector delta;
				SW_FT_Vector_From_Polar(&delta, radius, stroker->angle_out + rotate);
				delta.x += stroker->center.x;
				delta.y += stroker->center.y;
				border->movable = FALSE;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
			}
			else
			{
				SW_FT_Vector middle, delta;
				SW_FT_Fixed coef;

				SW_FT_Vector_From_Polar(&middle, SW_FT_MulFix(radius, stroker->miter_limit), phi);
				coef = SW_FT_DivFix(0x10000L - sigma.x, sigma.y);
				delta.x = SW_FT_MulFix(middle.y, coef);
				delta.y = SW_FT_MulFix(-middle.x, coef);
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
					SW_FT_Vector_From_Polar(&delta, radius, stroker->angle_out + rotate);
					delta.x += stroker->center.x;
					delta.y += stroker->center.y;
					error = ft_stroke_border_lineto(border, &delta, FALSE);
				}
			}
		}
		else
		{
			SW_FT_Fixed length;
			SW_FT_Vector delta;
			length = SW_FT_MulDiv(stroker->radius, stroker->miter_limit, sigma.x);
			SW_FT_Vector_From_Polar(&delta, length, phi);
			delta.x += stroker->center.x;
			delta.y += stroker->center.y;
			error = ft_stroke_border_lineto(border, &delta, FALSE);
			if(error)
				goto Exit;
			if(line_length == 0)
			{
				SW_FT_Vector_From_Polar(&delta, stroker->radius, stroker->angle_out + rotate);
				delta.x += stroker->center.x;
				delta.y += stroker->center.y;
				error = ft_stroke_border_lineto(border, &delta, FALSE);
			}
		}
	}
Exit:
	return error;
}

static SW_FT_Error ft_stroker_process_corner(SW_FT_Stroker stroker, SW_FT_Fixed line_length)
{
	SW_FT_Error error = 0;
	SW_FT_Angle turn;
	SW_FT_Int inside_side;

	turn = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
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

static SW_FT_Error ft_stroker_subpath_start(SW_FT_Stroker stroker, SW_FT_Angle start_angle, SW_FT_Fixed line_length)
{
	SW_FT_Vector delta;
	SW_FT_Vector point;
	SW_FT_Error error;
	SW_FT_StrokeBorder border;

	SW_FT_Vector_From_Polar(&delta, stroker->radius, start_angle + SW_FT_ANGLE_PI2);
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

SW_FT_Error SW_FT_Stroker_LineTo(SW_FT_Stroker stroker, SW_FT_Vector * to)
{
	SW_FT_Error error = 0;
	SW_FT_StrokeBorder border;
	SW_FT_Vector delta;
	SW_FT_Angle angle;
	SW_FT_Int side;
	SW_FT_Fixed line_length;

	delta.x = to->x - stroker->center.x;
	delta.y = to->y - stroker->center.y;
	if(delta.x == 0 && delta.y == 0)
		goto Exit;
	line_length = SW_FT_Vector_Length(&delta);
	angle = SW_FT_Atan2(delta.x, delta.y);
	SW_FT_Vector_From_Polar(&delta, stroker->radius, angle + SW_FT_ANGLE_PI2);
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
		SW_FT_Vector point;
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

SW_FT_Error SW_FT_Stroker_ConicTo(SW_FT_Stroker stroker, SW_FT_Vector * control, SW_FT_Vector * to)
{
	SW_FT_Error error = 0;
	SW_FT_Vector bez_stack[34];
	SW_FT_Vector *arc;
	SW_FT_Vector *limit = bez_stack + 30;
	SW_FT_Bool first_arc = TRUE;

	if(SW_FT_IS_SMALL(stroker->center.x - control->x) &&
		SW_FT_IS_SMALL(stroker->center.y - control->y) &&
		SW_FT_IS_SMALL(control->x - to->x) &&
		SW_FT_IS_SMALL(control->y - to->y))
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
		SW_FT_Angle angle_in, angle_out;
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
		else if(ft_pos_abs(SW_FT_Angle_Diff(stroker->angle_in, angle_in)) > SW_FT_SMALL_CONIC_THRESHOLD / 4)
		{
			stroker->center = arc[2];
			stroker->angle_out = angle_in;
			stroker->line_join = SW_FT_STROKER_LINEJOIN_ROUND;
			error = ft_stroker_process_corner(stroker, 0);
			stroker->line_join = stroker->line_join_saved;
		}
		if(error)
			goto Exit;
		{
			SW_FT_Vector ctrl, end;
			SW_FT_Angle theta, phi, rotate, alpha0 = 0;
			SW_FT_Fixed length;
			SW_FT_StrokeBorder border;
			SW_FT_Int side;

			theta = SW_FT_Angle_Diff(angle_in, angle_out) / 2;
			phi = angle_in + theta;
			length = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta));
			if(stroker->handle_wide_strokes)
				alpha0 = SW_FT_Atan2(arc[0].x - arc[2].x, arc[0].y - arc[2].y);
			for(border = stroker->borders, side = 0; side <= 1; side++, border++)
			{
				rotate = SW_FT_SIDE_TO_ROTATE(side);
				SW_FT_Vector_From_Polar(&ctrl, length, phi + rotate);
				ctrl.x += arc[1].x;
				ctrl.y += arc[1].y;
				SW_FT_Vector_From_Polar(&end, stroker->radius, angle_out + rotate);
				end.x += arc[0].x;
				end.y += arc[0].y;

				if(stroker->handle_wide_strokes)
				{
					SW_FT_Vector start;
					SW_FT_Angle alpha1;
					start = border->points[border->num_points - 1];
					alpha1 = SW_FT_Atan2(end.x - start.x, end.y - start.y);
					if(ft_pos_abs(SW_FT_Angle_Diff(alpha0, alpha1)) > SW_FT_ANGLE_PI / 2)
					{
						SW_FT_Angle beta, gamma;
						SW_FT_Vector bvec, delta;
						SW_FT_Fixed blen, sinA, sinB, alen;
						beta = SW_FT_Atan2(arc[2].x - start.x, arc[2].y - start.y);
						gamma = SW_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);
						bvec.x = end.x - start.x;
						bvec.y = end.y - start.y;
						blen = SW_FT_Vector_Length(&bvec);
						sinA = ft_pos_abs(SW_FT_Sin(alpha1 - gamma));
						sinB = ft_pos_abs(SW_FT_Sin(beta - gamma));
						alen = SW_FT_MulDiv(blen, sinA, sinB);
						SW_FT_Vector_From_Polar(&delta, alen, beta);
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
Exit:
	return error;
}

SW_FT_Error SW_FT_Stroker_CubicTo(SW_FT_Stroker stroker, SW_FT_Vector * control1, SW_FT_Vector * control2, SW_FT_Vector * to)
{
	SW_FT_Error error = 0;
	SW_FT_Vector bez_stack[37];
	SW_FT_Vector *arc;
	SW_FT_Vector *limit = bez_stack + 32;
	SW_FT_Bool first_arc = TRUE;

	if(SW_FT_IS_SMALL(stroker->center.x - control1->x) &&
		SW_FT_IS_SMALL(stroker->center.y - control1->y) &&
		SW_FT_IS_SMALL(control1->x - control2->x) &&
		SW_FT_IS_SMALL(control1->y - control2->y) &&
		SW_FT_IS_SMALL(control2->x - to->x) &&
		SW_FT_IS_SMALL(control2->y - to->y))
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
		SW_FT_Angle angle_in, angle_mid, angle_out;
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
		else if(ft_pos_abs(SW_FT_Angle_Diff(stroker->angle_in, angle_in)) > SW_FT_SMALL_CUBIC_THRESHOLD / 4)
		{
			stroker->center = arc[3];
			stroker->angle_out = angle_in;
			stroker->line_join = SW_FT_STROKER_LINEJOIN_ROUND;
			error = ft_stroker_process_corner(stroker, 0);
			stroker->line_join = stroker->line_join_saved;
		}
		if(error)
			goto Exit;
		{
			SW_FT_Vector ctrl1, ctrl2, end;
			SW_FT_Angle theta1, phi1, theta2, phi2, rotate, alpha0 = 0;
			SW_FT_Fixed length1, length2;
			SW_FT_StrokeBorder border;
			SW_FT_Int side;

			theta1 = SW_FT_Angle_Diff(angle_in, angle_mid) / 2;
			theta2 = SW_FT_Angle_Diff(angle_mid, angle_out) / 2;
			phi1 = ft_angle_mean(angle_in, angle_mid);
			phi2 = ft_angle_mean(angle_mid, angle_out);
			length1 = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta1));
			length2 = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta2));
			if(stroker->handle_wide_strokes)
				alpha0 = SW_FT_Atan2(arc[0].x - arc[3].x, arc[0].y - arc[3].y);
			for(border = stroker->borders, side = 0; side <= 1; side++, border++)
			{
				rotate = SW_FT_SIDE_TO_ROTATE(side);
				SW_FT_Vector_From_Polar(&ctrl1, length1, phi1 + rotate);
				ctrl1.x += arc[2].x;
				ctrl1.y += arc[2].y;
				SW_FT_Vector_From_Polar(&ctrl2, length2, phi2 + rotate);
				ctrl2.x += arc[1].x;
				ctrl2.y += arc[1].y;
				SW_FT_Vector_From_Polar(&end, stroker->radius, angle_out + rotate);
				end.x += arc[0].x;
				end.y += arc[0].y;
				if(stroker->handle_wide_strokes)
				{
					SW_FT_Vector start;
					SW_FT_Angle alpha1;
					start = border->points[border->num_points - 1];
					alpha1 = SW_FT_Atan2(end.x - start.x, end.y - start.y);
					if(ft_pos_abs(SW_FT_Angle_Diff(alpha0, alpha1)) >
					SW_FT_ANGLE_PI / 2)
					{
						SW_FT_Angle beta, gamma;
						SW_FT_Vector bvec, delta;
						SW_FT_Fixed blen, sinA, sinB, alen;
						beta = SW_FT_Atan2(arc[3].x - start.x, arc[3].y - start.y);
						gamma = SW_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);
						bvec.x = end.x - start.x;
						bvec.y = end.y - start.y;
						blen = SW_FT_Vector_Length(&bvec);
						sinA = ft_pos_abs(SW_FT_Sin(alpha1 - gamma));
						sinB = ft_pos_abs(SW_FT_Sin(beta - gamma));
						alen = SW_FT_MulDiv(blen, sinA, sinB);
						SW_FT_Vector_From_Polar(&delta, alen, beta);
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
Exit:
	return error;
}

SW_FT_Error SW_FT_Stroker_BeginSubPath(SW_FT_Stroker stroker, SW_FT_Vector * to, SW_FT_Bool open)
{
	stroker->first_point = TRUE;
	stroker->center = *to;
	stroker->subpath_open = open;
	stroker->handle_wide_strokes = SW_FT_BOOL(stroker->line_join != SW_FT_STROKER_LINEJOIN_ROUND || (stroker->subpath_open && stroker->line_cap == SW_FT_STROKER_LINECAP_BUTT));
	stroker->subpath_start = *to;
	stroker->angle_in = 0;
	return 0;
}

static SW_FT_Error ft_stroker_add_reverse_left(SW_FT_Stroker stroker, SW_FT_Bool open)
{
	SW_FT_StrokeBorder right = stroker->borders + 0;
	SW_FT_StrokeBorder left = stroker->borders + 1;
	SW_FT_Int new_points;
	SW_FT_Error error = 0;

	new_points = left->num_points - left->start;
	if(new_points > 0)
	{
		error = ft_stroke_border_grow(right, (SW_FT_UInt)new_points);
		if(error)
			goto Exit;
		{
			SW_FT_Vector *dst_point = right->points + right->num_points;
			SW_FT_Byte *dst_tag = right->tags + right->num_points;
			SW_FT_Vector *src_point = left->points + left->num_points - 1;
			SW_FT_Byte *src_tag = left->tags + left->num_points - 1;
			while(src_point >= left->points + left->start)
			{
				*dst_point = *src_point;
				*dst_tag = *src_tag;
				if(open)
					dst_tag[0] &= ~SW_FT_STROKE_TAG_BEGIN_END;
				else
				{
					SW_FT_Byte ttag = (SW_FT_Byte)(dst_tag[0] & SW_FT_STROKE_TAG_BEGIN_END);
					if(ttag == SW_FT_STROKE_TAG_BEGIN || ttag == SW_FT_STROKE_TAG_END)
						dst_tag[0] ^= SW_FT_STROKE_TAG_BEGIN_END;
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

SW_FT_Error SW_FT_Stroker_EndSubPath(SW_FT_Stroker stroker)
{
	SW_FT_Error error = 0;

	if(stroker->subpath_open)
	{
		SW_FT_StrokeBorder right = stroker->borders;
		error = ft_stroker_cap(stroker, stroker->angle_in, 0);
		if(error)
			goto Exit;
		error = ft_stroker_add_reverse_left(stroker, TRUE);
		if(error)
			goto Exit;
		stroker->center = stroker->subpath_start;
		error = ft_stroker_cap(stroker, stroker->subpath_angle + SW_FT_ANGLE_PI, 0);
		if(error)
			goto Exit;
		ft_stroke_border_close(right, FALSE);
	}
	else
	{
		SW_FT_Angle turn;
		SW_FT_Int inside_side;
		if(stroker->center.x != stroker->subpath_start.x || stroker->center.y != stroker->subpath_start.y)
		{
			error = SW_FT_Stroker_LineTo(stroker, &stroker->subpath_start);
			if(error)
				goto Exit;
		}
		stroker->angle_out = stroker->subpath_angle;
		turn = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
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

SW_FT_Error SW_FT_Stroker_GetBorderCounts(SW_FT_Stroker stroker, SW_FT_StrokerBorder border, SW_FT_UInt * anum_points, SW_FT_UInt * anum_contours)
{
	SW_FT_UInt num_points = 0, num_contours = 0;
	SW_FT_Error error;

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

SW_FT_Error SW_FT_Stroker_GetCounts(SW_FT_Stroker stroker, SW_FT_UInt * anum_points, SW_FT_UInt * anum_contours)
{
	SW_FT_UInt count1, count2, num_points = 0;
	SW_FT_UInt count3, count4, num_contours = 0;
	SW_FT_Error error;

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

void SW_FT_Stroker_ExportBorder(SW_FT_Stroker stroker, SW_FT_StrokerBorder border, SW_FT_Outline * outline)
{
	if(border == SW_FT_STROKER_BORDER_LEFT || border == SW_FT_STROKER_BORDER_RIGHT)
	{
		SW_FT_StrokeBorder sborder = &stroker->borders[border];
		if(sborder->valid)
			ft_stroke_border_export(sborder, outline);
	}
}

void SW_FT_Stroker_Export(SW_FT_Stroker stroker, SW_FT_Outline * outline)
{
	SW_FT_Stroker_ExportBorder(stroker, SW_FT_STROKER_BORDER_LEFT, outline);
	SW_FT_Stroker_ExportBorder(stroker, SW_FT_STROKER_BORDER_RIGHT, outline);
}

SW_FT_Error SW_FT_Stroker_ParseOutline(SW_FT_Stroker stroker, const SW_FT_Outline * outline)
{
	SW_FT_Vector v_last;
	SW_FT_Vector v_control;
	SW_FT_Vector v_start;
	SW_FT_Vector *point;
	SW_FT_Vector *limit;
	char *tags;
	SW_FT_Error error;
	SW_FT_Int n;
	SW_FT_UInt first;
	SW_FT_Int tag;

	if(!outline || !stroker)
		return -1;
	SW_FT_Stroker_Rewind(stroker);
	first = 0;
	for(n = 0; n < outline->n_contours; n++)
	{
		SW_FT_UInt last;
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
		tag = SW_FT_CURVE_TAG(tags[0]);
		if(tag == SW_FT_CURVE_TAG_CUBIC)
			goto Invalid_Outline;
		if(tag == SW_FT_CURVE_TAG_CONIC)
		{
			if(SW_FT_CURVE_TAG(outline->tags[last]) == SW_FT_CURVE_TAG_ON)
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
		error = SW_FT_Stroker_BeginSubPath(stroker, &v_start, outline->contours_flag[n]);
		if(error)
			goto Exit;
		while(point < limit)
		{
			point++;
			tags++;
			tag = SW_FT_CURVE_TAG(tags[0]);
			switch(tag)
			{
			case SW_FT_CURVE_TAG_ON:
			{
				SW_FT_Vector vec;
				vec.x = point->x;
				vec.y = point->y;
				error = SW_FT_Stroker_LineTo(stroker, &vec);
				if(error)
					goto Exit;
				continue;
			}

			case SW_FT_CURVE_TAG_CONIC:
				v_control.x = point->x;
				v_control.y = point->y;
Do_Conic:
				if(point < limit)
				{
					SW_FT_Vector vec;
					SW_FT_Vector v_middle;
					point++;
					tags++;
					tag = SW_FT_CURVE_TAG(tags[0]);
					vec = point[0];
					if(tag == SW_FT_CURVE_TAG_ON)
					{
						error = SW_FT_Stroker_ConicTo(stroker, &v_control, &vec);
						if(error)
							goto Exit;
						continue;
					}
					if(tag != SW_FT_CURVE_TAG_CONIC)
						goto Invalid_Outline;
					v_middle.x = (v_control.x + vec.x) / 2;
					v_middle.y = (v_control.y + vec.y) / 2;
					error = SW_FT_Stroker_ConicTo(stroker, &v_control, &v_middle);
					if(error)
						goto Exit;
					v_control = vec;
					goto Do_Conic;
				}
				error = SW_FT_Stroker_ConicTo(stroker, &v_control, &v_start);
				goto Close;

			default:
			{
				SW_FT_Vector vec1, vec2;
				if(point + 1 > limit ||
				SW_FT_CURVE_TAG(tags[1]) != SW_FT_CURVE_TAG_CUBIC)
					goto Invalid_Outline;
				point += 2;
				tags += 2;
				vec1 = point[-2];
				vec2 = point[-1];
				if(point <= limit)
				{
					SW_FT_Vector vec;
					vec = point[0];
					error = SW_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &vec);
					if(error)
						goto Exit;
					continue;
				}
				error = SW_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &v_start);
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
		error = SW_FT_Stroker_EndSubPath(stroker);
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
