#ifndef __FRAMEWORK_CORE_L_DOBJECT_H__
#define __FRAMEWORK_CORE_L_DOBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <cairo.h>
#include <cairoint.h>
#include <framework/luahelper.h>

#define MT_DOBJECT	"__mt_dobject__"

enum alignment_t {
	ALIGN_NONE				= 0,
	ALIGN_LEFT				= 1,
	ALIGN_TOP				= 2,
	ALIGN_RIGHT				= 3,
	ALIGN_BOTTOM			= 4,
	ALIGN_LEFT_TOP			= 5,
	ALIGN_RIGHT_TOP			= 6,
	ALIGN_LEFT_BOTTOM		= 7,
	ALIGN_RIGHT_BOTTOM		= 8,
	ALIGN_LEFT_CENTER		= 9,
	ALIGN_TOP_CENTER		= 10,
	ALIGN_RIGHT_CENTER		= 11,
	ALIGN_BOTTOM_CENTER		= 12,
	ALIGN_HORIZONTAL_CENTER	= 13,
	ALIGN_VERTICAL_CENTER	= 14,
	ALIGN_CENTER			= 15,
	ALIGN_LEFT_FILL			= 16,
	ALIGN_TOP_FILL			= 17,
	ALIGN_RIGHT_FILL		= 18,
	ALIGN_BOTTOM_FILL		= 19,
	ALIGN_HORIZONTAL_FILL	= 20,
	ALIGN_VERTICAL_FILL		= 21,
	ALIGN_CENTER_FILL		= 22,
};

struct ldobject_t {
	struct ldobject_t * parent;
	struct list_head entry;
	struct list_head children;

	double width, height;
	double x, y;
	double rotation;
	double scalex, scaley;
	double skewx, skewy;
	double anchorx, anchory;
	double alpha;
	enum alignment_t alignment;
	int visible;
	int touchable;
	int mflag;
	cairo_matrix_t local_matrix;
	cairo_matrix_t global_matrix;

	void (*draw)(lua_State * L, struct ldobject_t * o);
	void * priv;
};

int luaopen_dobject(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_DOBJECT_H__ */
