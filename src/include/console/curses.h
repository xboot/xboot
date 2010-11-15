#ifndef __CURSES_H__
#define __CURSES_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <console/console.h>


struct curses_window
{
	/* console with which window associates */
	struct console * console;

	/* window origin coordinates */
	x_s32 orix, oriy;

	/* window cursor position */
	x_s32 curx, cury;

	/* window dimensions */
	x_s32 width, height;

	/* window fg and bg color */
	enum console_color f, b;

	/* window attributes */
	//attr_t attrs;

	/* parent window */
	struct curses_window * parent;
};


struct curses_window * curses_alloc_topwin(struct console * console);
x_bool curses_free_topwin(struct curses_window * win);

#endif /* __CURSES_H__ */
