#ifndef __TUI_H__
#define __TUI_H__

#include <configs.h>
#include <default.h>
#include <console/console.h>

/*
 * forward declare
 */
struct tui_widget;

struct tui_widget_ops
{
	/* set parent widget */
	x_bool (*setparent)(struct tui_widget * widget, struct tui_widget * parent);

	/* get parent widget */
	struct tui_widget * (*getparent)(struct tui_widget * widget);

	/* add a child widget */
	x_bool (*addchild)(struct tui_widget * widget, struct tui_widget * child);

	/* remove a child widget */
	x_bool (*removechild)(struct tui_widget * widget, struct tui_widget * child);

	/* set bounds */
	x_bool (*setbounds)(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* get bounds */
	x_bool (*getbounds)(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);

	/* get minimal width and height */
	x_bool (*minsize)(struct tui_widget * widget, x_s32 * w, x_s32 * h);

	/* set widget property */
	x_bool (*setproperty)(struct tui_widget * widget, const x_s8 * name, const x_s8 * value);

	/* paint widget with area */
	x_bool (*paint)(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* destory widget */
	x_bool (*destroy)(struct tui_widget * widget);
};

struct tui_widget
{
	/* bind console */
	struct console * console;

	/* widget's id */
	x_s8 * id;

	/* widget's bounds */
	x_s32 x, y;
	x_s32 w, h;

	/* container layout */
	struct tui_layout * layout;

	/* widget operations */
	struct tui_widget_ops * ops;

	/* the parent widget */
	struct tui_widget * parent;

	/* the widget entry */
	struct list_head entry;

	/* child widget list head */
	struct list_head child;

	/* priv data */
	void * priv;
};


struct tui_widget * find_tui_widget_by_id(struct tui_widget * widget, const x_s8 * id);



#endif /* __TUI_H__ */
