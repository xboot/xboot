#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <configs.h>
#include <default.h>
#include <xboot/list.h>


enum {
	UNICODE_BS				= 0x0008,
	UNICODE_TAB				= 0x0009,
	UNICODE_LF				= 0x000a,
	UNICODE_CR				= 0x000d,
	UNICODE_SPACE			= 0x0020,

	UNICODE_HLINE			= '-',
	UNICODE_VLINE			= '|',
	UNICODE_LEFT			= '<',
	UNICODE_RIGHT			= '>',
	UNICODE_UP				= '^',
	UNICODE_DOWN			= 'v',
	UNICODE_LEFTUP			= '+',
	UNICODE_RIGHTUP			= '+',
	UNICODE_LEFTDOWN		= '+',
	UNICODE_RIGHTDOWN		= '+',
	UNICODE_CROSS			= '+',
	UNICODE_CUBE			= '#',
};

/*
 * 256 colors for terminal
 */
enum tcolor {
	TCOLOR_BLACK			= 0x00,
	TCOLOR_RED				= 0x01,
	TCOLOR_GREEN			= 0x02,
	TCOLOR_YELLOW			= 0x03,
	TCOLOR_BULE				= 0x04,
	TCOLOR_MAGENTA			= 0x05,
	TCOLOR_CYAN				= 0x06,
	TCOLOR_WHITE			= 0x07,

	TCOLOR_BRIGHT_BLACK		= 0x08,
	TCOLOR_BRIGHT_RED		= 0x09,
	TCOLOR_BRIGHT_GREEN		= 0x0a,
	TCOLOR_BRIGHT_YELLOW	= 0x0b,
	TCOLOR_BRIGHT_BULE		= 0x0c,
	TCOLOR_BRIGHT_MAGENTA	= 0x0d,
	TCOLOR_BRIGHT_CYAN		= 0x0e,
	TCOLOR_BRIGHT_WHITE		= 0x0f,
};

/*
 * define the struct of console
 */
struct console
{
	/* the name of console */
	char * name;

	/* get console's width and height */
	x_bool(*getwh)(struct console * console, x_s32 * w, x_s32 * h);

	/* get cursor position */
	x_bool(*getxy)(struct console * console, x_s32 * x, x_s32 * y);

	/* set cursor position */
	x_bool(*gotoxy)(struct console * console, x_s32 x, x_s32 y);

	/* turn on/off the cursor */
	x_bool(*setcursor)(struct console * console, x_bool on);

	/* get cursor's status */
	x_bool(*getcursor)(struct console * console);

	/* set console's foreground color and background color */
	x_bool(*setcolor)(struct console * console, enum tcolor f, enum tcolor b);

	/* get console foreground color and background color */
	x_bool(*getcolor)(struct console * console, enum tcolor * f, enum tcolor * b);

	/* clear screen */
	x_bool(*cls)(struct console * console);

	/* get a unicode character */
	x_bool(*getcode)(struct console * console, x_u32 * code);

	/* put a unicode character */
	x_bool(*putcode)(struct console * console, x_u32 code);

	/* private data */
	void * priv;
};

struct console_list
{
	struct console * console;
	struct list_head entry;
};

struct console * search_console(const char *name);
x_bool register_console(struct console * console);
x_bool unregister_console(struct console * console);

struct console * get_stdin(void);
struct console * get_stdout(void);
x_bool set_stdinout(const char * in, const char * out);

x_bool console_getwh(struct console * console, x_s32 * w, x_s32 * h);
x_bool console_getxy(struct console * console, x_s32 * x, x_s32 * y);
x_bool console_gotoxy(struct console * console, x_s32 x, x_s32 y);
x_bool console_setcursor(struct console * console, x_bool on);
x_bool console_getcursor(struct console * console);
x_bool console_setcolor(struct console * console, enum tcolor f, enum tcolor b);
x_bool console_getcolor(struct console * console, enum tcolor * f, enum tcolor * b);
x_bool console_cls(struct console * console);
x_bool console_getcode(struct console * console, x_u32 * code);
x_bool console_putcode(struct console * console, x_u32 code);

#endif /* __CONSOLE_H__ */
