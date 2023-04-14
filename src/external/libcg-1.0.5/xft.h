#ifndef __XFT_H__
#define __XFT_H__

#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

/*
 * type
 */
typedef signed long XCG_FT_Fixed;
typedef signed int XCG_FT_Int;
typedef unsigned int XCG_FT_UInt;
typedef signed long XCG_FT_Long;
typedef unsigned long XCG_FT_ULong;
typedef signed short XCG_FT_Short;
typedef unsigned char XCG_FT_Byte;
typedef unsigned char XCG_FT_Bool;
typedef int XCG_FT_Error;
typedef signed long XCG_FT_Pos;

typedef struct XCG_FT_Vector_ {
	XCG_FT_Pos x;
	XCG_FT_Pos y;
} XCG_FT_Vector;

/*
 * math
 */
typedef XCG_FT_Fixed XCG_FT_Angle;

XCG_FT_Long XCG_FT_MulFix(XCG_FT_Long a, XCG_FT_Long b);
XCG_FT_Long XCG_FT_MulDiv(XCG_FT_Long a, XCG_FT_Long b, XCG_FT_Long c);
XCG_FT_Long XCG_FT_DivFix(XCG_FT_Long a, XCG_FT_Long b);
XCG_FT_Fixed XCG_FT_Sin(XCG_FT_Angle angle);
XCG_FT_Fixed XCG_FT_Cos(XCG_FT_Angle angle);
XCG_FT_Fixed XCG_FT_Tan(XCG_FT_Angle angle);
XCG_FT_Angle XCG_FT_Atan2(XCG_FT_Fixed x, XCG_FT_Fixed y);
XCG_FT_Angle XCG_FT_Angle_Diff(XCG_FT_Angle angle1, XCG_FT_Angle angle2);
void XCG_FT_Vector_Unit(XCG_FT_Vector *vec, XCG_FT_Angle angle);
void XCG_FT_Vector_Rotate(XCG_FT_Vector *vec, XCG_FT_Angle angle);
XCG_FT_Fixed XCG_FT_Vector_Length(XCG_FT_Vector *vec);
void XCG_FT_Vector_Polarize(XCG_FT_Vector *vec, XCG_FT_Fixed *length, XCG_FT_Angle *angle);
void XCG_FT_Vector_From_Polar(XCG_FT_Vector *vec, XCG_FT_Fixed length, XCG_FT_Angle angle);

/*
 * raster
 */
typedef struct XCG_FT_BBox_ {
	XCG_FT_Pos xMin, yMin;
	XCG_FT_Pos xMax, yMax;
} XCG_FT_BBox;

typedef struct XCG_FT_Outline_ {
	int n_contours;
	int n_points;
	XCG_FT_Vector * points;
	char * tags;
	int * contours;
	char * contours_flag;
	int flags;
} XCG_FT_Outline;

#define XCG_FT_OUTLINE_NONE				0x0
#define XCG_FT_OUTLINE_OWNER			0x1
#define XCG_FT_OUTLINE_EVEN_ODD_FILL	0x2
#define XCG_FT_OUTLINE_REVERSE_FILL		0x4
#define XCG_FT_CURVE_TAG(flag)			(flag & 3)
#define XCG_FT_CURVE_TAG_ON      		1
#define XCG_FT_CURVE_TAG_CONIC			0
#define XCG_FT_CURVE_TAG_CUBIC			2
#define XCG_FT_Curve_Tag_On				XCG_FT_CURVE_TAG_ON
#define XCG_FT_Curve_Tag_Conic			XCG_FT_CURVE_TAG_CONIC
#define XCG_FT_Curve_Tag_Cubic			XCG_FT_CURVE_TAG_CUBIC

typedef struct XCG_FT_Span_ {
	int x;
	int len;
	int y;
	unsigned char coverage;
} XCG_FT_Span;

typedef void (*XCG_FT_SpanFunc)(int count, const XCG_FT_Span * spans, void * user);
#define XCG_FT_Raster_Span_Func  XCG_FT_SpanFunc

#define XCG_FT_RASTER_FLAG_DEFAULT  0x0
#define XCG_FT_RASTER_FLAG_AA       0x1
#define XCG_FT_RASTER_FLAG_DIRECT   0x2
#define XCG_FT_RASTER_FLAG_CLIP     0x4

typedef struct XCG_FT_Raster_Params_ {
	const void * source;
	int flags;
	XCG_FT_SpanFunc gray_spans;
	void * user;
	XCG_FT_BBox clip_box;
} XCG_FT_Raster_Params;

XCG_FT_Error XCG_FT_Outline_Check(XCG_FT_Outline * outline);
void XCG_FT_Outline_Get_CBox(const XCG_FT_Outline * outline, XCG_FT_BBox * acbox);
void XCG_FT_Raster_Render(const XCG_FT_Raster_Params * params);

/*
 * stroker
 */
typedef struct XCG_FT_StrokerRec_ * XCG_FT_Stroker;

typedef enum XCG_FT_Stroker_LineJoin_ {
	XCG_FT_STROKER_LINEJOIN_ROUND = 0,
	XCG_FT_STROKER_LINEJOIN_BEVEL = 1,
	XCG_FT_STROKER_LINEJOIN_MITER_VARIABLE = 2,
	XCG_FT_STROKER_LINEJOIN_MITER = XCG_FT_STROKER_LINEJOIN_MITER_VARIABLE,
	XCG_FT_STROKER_LINEJOIN_MITER_FIXED = 3,
} XCG_FT_Stroker_LineJoin;

typedef enum XCG_FT_Stroker_LineCap_ {
	XCG_FT_STROKER_LINECAP_BUTT = 0,
	XCG_FT_STROKER_LINECAP_ROUND = 1,
	XCG_FT_STROKER_LINECAP_SQUARE = 2,
} XCG_FT_Stroker_LineCap;

typedef enum XCG_FT_StrokerBorder_ {
	XCG_FT_STROKER_BORDER_LEFT = 0,
	XCG_FT_STROKER_BORDER_RIGHT = 1,
} XCG_FT_StrokerBorder;

XCG_FT_Error XCG_FT_Stroker_New(XCG_FT_Stroker * astroker);
void XCG_FT_Stroker_Set(XCG_FT_Stroker stroker, XCG_FT_Fixed radius, XCG_FT_Stroker_LineCap line_cap, XCG_FT_Stroker_LineJoin line_join, XCG_FT_Fixed miter_limit);
XCG_FT_Error XCG_FT_Stroker_ParseOutline(XCG_FT_Stroker stroker, const XCG_FT_Outline * outline);
XCG_FT_Error XCG_FT_Stroker_GetCounts(XCG_FT_Stroker stroker, XCG_FT_UInt * anum_points, XCG_FT_UInt * anum_contours);
void XCG_FT_Stroker_Export(XCG_FT_Stroker stroker, XCG_FT_Outline * outline);
void XCG_FT_Stroker_Done(XCG_FT_Stroker stroker);

#endif /* __XFT_H__ */
