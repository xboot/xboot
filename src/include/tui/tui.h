#ifndef __TUI_H__
#define __TUI_H__

#include <configs.h>
#include <default.h>
#include <console/console.h>

/*
 * forward declare
 */
struct tui_component;


typedef void (*tui_component_callback)(struct tui_component * component, void * data);

struct tui_component_ops
{
	/* get component's id */
	const x_s8 * (*getid)(void * self);

	/* set parent component */
	x_bool (*setparent)(void * self, struct tui_component * parent);

	/* get parent component */
	struct tui_component * (*getparent)(void * self);

	/* add a child component */
	x_bool (*addchild)(void * self, struct tui_component * child);

	/* remove a child component */
	x_bool (*removechild)(void * self, struct tui_component * child);

	/* iterate child component */
	x_bool (*iteratechild)(void * self, tui_component_callback cb, void * data);

	/* set bounds */
	x_bool (*setbounds)(void * self, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* get bounds */
	x_bool (*getbounds)(void * self, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);

	/* get minimal width and height */
	x_bool (*minsize)(void * self, x_s32 * w, x_s32 * h);

	/* set component property */
	x_bool (*setproperty)(void * self, const x_s8 * name, const x_s8 * value);

	/* paint component with area */
	x_bool (*paint)(void * self, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* destory component */
	x_bool (*destroy)(void * self);
};

struct tui_component
{
	/* component's id */
	x_s8 * id;

	/* component's bounds */
	x_s32 x, y;
	x_s32 w, h;

	/* cursor position */
	x_s32 curx, cury;

	/* output console */
	struct console * console;

	/* component operations */
	struct tui_component_ops * ops;

	/* point to self */
	void * self;

	/* parent component */
	struct tui_component * parent;

	/* child component entry */
	struct list_head child;
};

#endif /* __TUI_H__ */
