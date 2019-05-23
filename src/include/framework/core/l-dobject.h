#ifndef __FRAMEWORK_CORE_L_DOBJECT_H__
#define __FRAMEWORK_CORE_L_DOBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <xboot/window.h>
#include <framework/luahelper.h>

#define MT_DOBJECT	"__mt_dobject__"

enum dobject_type_t {
	DOBJECT_TYPE_CONTAINER			= 0,
	DOBJECT_TYPE_IMAGE				= 1,
	DOBJECT_TYPE_NINEPATCH			= 2,
	DOBJECT_TYPE_SHAPE				= 3,
	DOBJECT_TYPE_TEXT				= 4,
};

enum collider_type_t {
	COLLIDER_TYPE_NONE				= 0,
	COLLIDER_TYPE_CIRCLE			= 1,
	COLLIDER_TYPE_ELLIPSE			= 2,
	COLLIDER_TYPE_RECTANGLE			= 3,
	COLLIDER_TYPE_ROUNDED_RECTANGLE	= 4,
	COLLIDER_TYPE_POLYGON			= 5,
};

struct ldobject_t {
	struct ldobject_t * parent;
	struct list_head entry;
	struct list_head children;
	enum dobject_type_t dtype;

	double width, height;
	double x, y;
	double rotation;
	double scalex, scaley;
	double skewx, skewy;
	double anchorx, anchory;
	double alpha;

	struct {
		double red;
		double green;
		double blue;
		double alpha;
	} background;

	struct {
		int style;
		double grow;
		double shrink;
		double basis;
		double width, height;
		struct {
			double left;
			double top;
			double right;
			double bottom;
		} margin;

		double x, y;
		double w, h;
	} layout;

	enum collider_type_t ctype;
	union {
		struct {
			double x;
			double y;
			double radius;
		} circle;

		struct {
			double x;
			double y;
			double width;
			double height;
		} ellipse;

		struct {
			double x;
			double y;
			double width;
			double height;
		} rectangle;

		struct {
			double x;
			double y;
			double width;
			double height;
			double radius;
		} rounded_rectangle;

		struct {
			double * points;
			int length;
		} polygon;
	} hit;

	int visible;
	int touchable;
	int mflag;
	struct matrix_t local_matrix;
	struct matrix_t global_matrix;
	struct region_t global_bounds;
	struct region_t dirty_bounds;

	void (*draw)(struct ldobject_t * o, struct window_t * w);
	void * priv;
};

int luaopen_dobject(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_DOBJECT_H__ */
