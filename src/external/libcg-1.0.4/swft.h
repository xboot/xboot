#ifndef __SWFT_H__
#define __SWFT_H__

#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
 * type
 */
typedef signed long SW_FT_Fixed;
typedef signed int SW_FT_Int;
typedef unsigned int SW_FT_UInt;
typedef signed long SW_FT_Long;
typedef unsigned long SW_FT_ULong;
typedef signed short SW_FT_Short;
typedef unsigned char SW_FT_Byte;
typedef unsigned char SW_FT_Bool;
typedef int SW_FT_Error;
typedef signed long SW_FT_Pos;

typedef struct SW_FT_Vector_ {
	SW_FT_Pos x;
	SW_FT_Pos y;
} SW_FT_Vector;

/*
 * math
 */
typedef SW_FT_Fixed SW_FT_Angle;

SW_FT_Long SW_FT_MulFix(SW_FT_Long a, SW_FT_Long b);
SW_FT_Long SW_FT_MulDiv(SW_FT_Long a, SW_FT_Long b, SW_FT_Long c);
SW_FT_Long SW_FT_DivFix(SW_FT_Long a, SW_FT_Long b);
SW_FT_Fixed SW_FT_Sin(SW_FT_Angle angle);
SW_FT_Fixed SW_FT_Cos(SW_FT_Angle angle);
SW_FT_Fixed SW_FT_Tan(SW_FT_Angle angle);
SW_FT_Angle SW_FT_Atan2(SW_FT_Fixed x, SW_FT_Fixed y);
SW_FT_Angle SW_FT_Angle_Diff(SW_FT_Angle angle1, SW_FT_Angle angle2);
void SW_FT_Vector_Unit(SW_FT_Vector * vec, SW_FT_Angle angle);
void SW_FT_Vector_Rotate(SW_FT_Vector * vec, SW_FT_Angle angle);
SW_FT_Fixed SW_FT_Vector_Length(SW_FT_Vector * vec);
void SW_FT_Vector_Polarize(SW_FT_Vector * vec, SW_FT_Fixed * length, SW_FT_Angle * angle);
void SW_FT_Vector_From_Polar(SW_FT_Vector * vec, SW_FT_Fixed length, SW_FT_Angle angle);

/*
 * raster
 */
typedef struct SW_FT_BBox_ {
	SW_FT_Pos xMin, yMin;
	SW_FT_Pos xMax, yMax;
} SW_FT_BBox;

typedef struct SW_FT_Outline_ {
	short n_contours;
	short n_points;
	SW_FT_Vector * points;
	char * tags;
	short * contours;
	char * contours_flag;
	int flags;
} SW_FT_Outline;

#define SW_FT_OUTLINE_NONE			0x0
#define SW_FT_OUTLINE_OWNER			0x1
#define SW_FT_OUTLINE_EVEN_ODD_FILL	0x2
#define SW_FT_OUTLINE_REVERSE_FILL	0x4
#define SW_FT_CURVE_TAG(flag)		(flag & 3)
#define SW_FT_CURVE_TAG_ON			1
#define SW_FT_CURVE_TAG_CONIC		0
#define SW_FT_CURVE_TAG_CUBIC		2
#define SW_FT_Curve_Tag_On			SW_FT_CURVE_TAG_ON
#define SW_FT_Curve_Tag_Conic		SW_FT_CURVE_TAG_CONIC
#define SW_FT_Curve_Tag_Cubic		SW_FT_CURVE_TAG_CUBIC

typedef struct SW_FT_RasterRec_ * SW_FT_Raster;

typedef struct SW_FT_Span_ {
	short x;
	short y;
	unsigned short len;
	unsigned char coverage;
} SW_FT_Span;

typedef void (*SW_FT_SpanFunc)(int count, const SW_FT_Span * spans, void * user);
typedef void (*SW_FT_BboxFunc)(int x, int y, int w, int h, void * user);

#define SW_FT_Raster_Span_Func		SW_FT_SpanFunc
#define SW_FT_RASTER_FLAG_DEFAULT	0x0
#define SW_FT_RASTER_FLAG_AA		0x1
#define SW_FT_RASTER_FLAG_DIRECT	0x2
#define SW_FT_RASTER_FLAG_CLIP		0x4

typedef struct SW_FT_Raster_Params_ {
	const void * source;
	int flags;
	SW_FT_SpanFunc gray_spans;
	SW_FT_BboxFunc bbox_cb;
	void * user;
	SW_FT_BBox clip_box;
} SW_FT_Raster_Params;

SW_FT_Error SW_FT_Outline_Check(SW_FT_Outline *outline);
void SW_FT_Outline_Get_CBox(const SW_FT_Outline *outline, SW_FT_BBox *acbox);
typedef int (*SW_FT_Raster_NewFunc)(SW_FT_Raster *raster);
#define SW_FT_Raster_New_Func  SW_FT_Raster_NewFunc
typedef void (*SW_FT_Raster_DoneFunc)(SW_FT_Raster raster);
#define SW_FT_Raster_Done_Func  SW_FT_Raster_DoneFunc
typedef void (*SW_FT_Raster_ResetFunc)(SW_FT_Raster raster, unsigned char * pool_base, unsigned long pool_size);
#define SW_FT_Raster_Reset_Func  SW_FT_Raster_ResetFunc
typedef int (*SW_FT_Raster_RenderFunc)(SW_FT_Raster raster, const SW_FT_Raster_Params * params);
#define SW_FT_Raster_Render_Func  SW_FT_Raster_RenderFunc
typedef struct SW_FT_Raster_Funcs_ {
	SW_FT_Raster_NewFunc raster_new;
	SW_FT_Raster_ResetFunc raster_reset;
	SW_FT_Raster_RenderFunc raster_render;
	SW_FT_Raster_DoneFunc raster_done;
} SW_FT_Raster_Funcs;
extern const SW_FT_Raster_Funcs sw_ft_grays_raster;

/*
 * stroker
 */
typedef struct SW_FT_StrokerRec_ * SW_FT_Stroker;

typedef enum SW_FT_Stroker_LineJoin_ {
	SW_FT_STROKER_LINEJOIN_ROUND = 0,
	SW_FT_STROKER_LINEJOIN_BEVEL = 1,
	SW_FT_STROKER_LINEJOIN_MITER_VARIABLE = 2,
	SW_FT_STROKER_LINEJOIN_MITER = SW_FT_STROKER_LINEJOIN_MITER_VARIABLE,
	SW_FT_STROKER_LINEJOIN_MITER_FIXED = 3,
} SW_FT_Stroker_LineJoin;

typedef enum SW_FT_Stroker_LineCap_ {
	SW_FT_STROKER_LINECAP_BUTT = 0,
	SW_FT_STROKER_LINECAP_ROUND = 1,
	SW_FT_STROKER_LINECAP_SQUARE = 2,
} SW_FT_Stroker_LineCap;

typedef enum SW_FT_StrokerBorder_ {
	SW_FT_STROKER_BORDER_LEFT = 0,
	SW_FT_STROKER_BORDER_RIGHT = 1,
} SW_FT_StrokerBorder;

SW_FT_Error SW_FT_Stroker_New(SW_FT_Stroker * astroker);
void SW_FT_Stroker_Set(SW_FT_Stroker stroker, SW_FT_Fixed radius, SW_FT_Stroker_LineCap line_cap, SW_FT_Stroker_LineJoin line_join, SW_FT_Fixed miter_limit);
SW_FT_Error SW_FT_Stroker_ParseOutline(SW_FT_Stroker stroker, const SW_FT_Outline * outline);
SW_FT_Error SW_FT_Stroker_GetCounts(SW_FT_Stroker stroker, SW_FT_UInt * anum_points, SW_FT_UInt * anum_contours);
void SW_FT_Stroker_Export(SW_FT_Stroker stroker, SW_FT_Outline *outline);
void SW_FT_Stroker_Done(SW_FT_Stroker stroker);

#endif /* __SWFT_H__ */
