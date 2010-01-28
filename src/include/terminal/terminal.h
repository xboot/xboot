#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>


/*
 * terminal type
 */
enum terminal_type {
	TERMINAL_SERIAL,
	TERMINAL_LCD,
	TERMINAL_KEYBOARD
};

/*
 * terminal color enum type.
 */
enum terminal_color {
	TERMINAL_NONE,
	TERMINAL_BLACK,
	TERMINAL_RED,
	TERMINAL_GREEN,
	TERMINAL_YELLOW,
	TERMINAL_BULE,
	TERMINAL_MAGENTA,
	TERMINAL_CYAN,
	TERMINAL_WHITE
};

/*
 * define the struct of terminal.
 */
struct terminal
{
	/* the name of terminal */
	char * name;

	/* terminal type */
	enum terminal_type type;

	/* get termianl's width and height */
	x_bool(*getwh)(struct terminal * term, x_s32 * w, x_s32 * h);

	/* set cursor position */
	x_bool(*setxy)(struct terminal * term, x_s32 x, x_s32 y);

	/* get cursor position */
	x_bool(*getxy)(struct terminal * term, x_s32 * x, x_s32 * y);

	/* move cursor to left with n line */
	x_bool(*cursor_left)(struct terminal * term, x_u32 n);

	/* move cursor to right with n line */
	x_bool(*cursor_right)(struct terminal * term, x_u32 n);

	/* move cursor to up with n line */
	x_bool(*cursor_up)(struct terminal * term, x_u32 n);

	/* move cursor to down with n line */
	x_bool(*cursor_down)(struct terminal * term, x_u32 n);

	/* move cursor to home */
	x_bool(*cursor_home)(struct terminal * term);

	/* save cursor */
	x_bool(*cursor_save)(struct terminal * term);

	/* restore cursor */
	x_bool(*cursor_restore)(struct terminal * term);

	/* hide cursor */
	x_bool(*cursor_hide)(struct terminal * term);

	/* show cursor */
	x_bool(*cursor_show)(struct terminal * term);

	/* flash cursor */
	x_bool(*cursor_flash)(struct terminal * term);

	/* highlight mode */
	x_bool(*mode_highlight)(struct terminal * term);

	/* underline mode */
	x_bool(*mode_underline)(struct terminal * term);

	/* reverse mode */
	x_bool(*mode_reverse)(struct terminal * term);

	/* blanking mode */
	x_bool(*mode_blanking)(struct terminal * term);

	/* close all attribute */
	x_bool(*mode_closeall)(struct terminal * term);

	/* set terminal's front color and background color */
	x_bool(*set_color)(struct terminal * term, enum terminal_color f, enum terminal_color b);

	/* get terminal's front color and background color */
	x_bool(*get_color)(struct terminal * term, enum terminal_color * f, enum terminal_color * b);

	/* clear screen */
	x_bool(*clear_screen)(struct terminal * term);

	/* terminal read */
	x_s32 (*read)(struct terminal * term, x_u8 * buf, x_s32 count);

	/* terminal write */
	x_s32 (*write)(struct terminal * term, const x_u8 * buf, x_s32 count);

	/* private data */
	void * priv;
};


/*
 * the list of terminal.
 */
struct terminal_list
{
	struct terminal * term;
	struct hlist_node node;
};

/*
 * stdout list.
 */
struct terminal_stdout_list
{
	/* pointer terminal */
	struct terminal * term;

	/* write function cache, for speed up */
	x_s32 (*write)(struct terminal *, const x_u8 *, x_s32);

	/* for hash list */
	struct hlist_node node;
};

/*
 * stdin list.
 */
struct terminal_stdin_list
{
	/* pointer terminal */
	struct terminal * term;

	/* read function cache, for speed up */
	x_s32 (*read)(struct terminal *, x_u8 *, x_s32);

	/* for hash list */
	struct hlist_node node;
};

/*
 * stdout console for configure.
 */
struct stdout
{
	const char * name;
};

/*
 * stdin in for configure.
 */
struct stdin
{
	const char * name;
};


struct terminal * search_terminal(const char *name);
x_bool register_terminal(struct terminal * term);
x_bool unregister_terminal(struct terminal * term);

struct terminal_stdout_list * search_terminal_stdout(const char * name);
x_bool add_terminal_stdout(const char * name);
x_bool del_terminal_stdout(const char * name);
struct terminal_stdin_list * search_terminal_stdin(const char * name);
x_bool add_terminal_stdin(const char * name);
x_bool del_terminal_stdin(const char * name);

void set_stdout_status(x_bool enable);
x_bool get_stdout_status(void);
x_bool stdout_terminal_getwh(x_s32 * w, x_s32 * h);

#endif /* __TERMINAL_H__ */
