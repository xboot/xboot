#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <xboot/list.h>

enum {
	UNICODE_BS				= 0x0008,
	UNICODE_TAB				= 0x0009,
	UNICODE_LF				= 0x000a,
	UNICODE_CR				= 0x000d,
	UNICODE_SPACE			= 0x0020,

	UNICODE_HLINE			= 0x2500,
	UNICODE_VLINE			= 0x2502,

	UNICODE_LEFTTOP			= 0x250c,
	UNICODE_RIGHTTOP		= 0x2510,
	UNICODE_LEFTBOTTOM		= 0x2514,
	UNICODE_RIGHTBOTTOM		= 0x2518,

	UNICODE_UP				= 0x25b2,
	UNICODE_RIGHT			= 0x25b6,
	UNICODE_DOWN			= 0x25bc,
	UNICODE_LEFT			= 0x25c0,
};

enum tcolor_t {
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

struct console_t
{
	/* the name of console */
	char * name;

	/* get console's width and height */
	bool_t(*getwh)(struct console_t * console, s32_t * w, s32_t * h);

	/* get cursor position */
	bool_t(*getxy)(struct console_t * console, s32_t * x, s32_t * y);

	/* set cursor position */
	bool_t(*gotoxy)(struct console_t * console, s32_t x, s32_t y);

	/* turn on/off the cursor */
	bool_t(*setcursor)(struct console_t * console, bool_t on);

	/* get cursor's status */
	bool_t(*getcursor)(struct console_t * console);

	/* set console's foreground color and background color */
	bool_t(*setcolor)(struct console_t * console, enum tcolor_t f, enum tcolor_t b);

	/* get console foreground color and background color */
	bool_t(*getcolor)(struct console_t * console, enum tcolor_t * f, enum tcolor_t * b);

	/* clear screen */
	bool_t(*cls)(struct console_t * console);

	/* get a unicode character */
	bool_t(*getcode)(struct console_t * console, u32_t * code);

	/* put a unicode character */
	bool_t(*putcode)(struct console_t * console, u32_t code);

	/* turn no or turn off the console */
	bool_t(*onoff)(struct console_t * console, bool_t flag);

	/* private data */
	void * priv;
};

struct console_list_t
{
	struct console_t * console;
	struct list_head entry;
};

struct console_t * get_console_stdin(void);
struct console_t * get_console_stdout(void);
struct console_t * get_console_stderr(void);

bool_t console_stdin_getcode(u32_t * code);
bool_t console_stdin_getcode_with_timeout(u32_t * code, u32_t timeout);
bool_t console_stdout_putc(char c);
bool_t console_stderr_putc(char c);

struct console_t * search_console(const char *name);
bool_t register_console(struct console_t * console);
bool_t unregister_console(struct console_t * console);

bool_t console_stdio_set(const char * in, const char * out, const char * err);
bool_t console_stdio_load(char * file);
bool_t console_stdio_save(char * file);

bool_t console_getwh(struct console_t * console, s32_t * w, s32_t * h);
bool_t console_getxy(struct console_t * console, s32_t * x, s32_t * y);
bool_t console_gotoxy(struct console_t * console, s32_t x, s32_t y);
bool_t console_setcursor(struct console_t * console, bool_t on);
bool_t console_getcursor(struct console_t * console);
bool_t console_setcolor(struct console_t * console, enum tcolor_t f, enum tcolor_t b);
bool_t console_getcolor(struct console_t * console, enum tcolor_t * f, enum tcolor_t * b);
bool_t console_cls(struct console_t * console);
bool_t console_getcode(struct console_t * console, u32_t * code);
bool_t console_putcode(struct console_t * console, u32_t code);
bool_t console_onoff(struct console_t * console, bool_t flag);

int console_print(struct console_t * console, const char * fmt, ...);
bool_t console_hline(struct console_t * console, u32_t code, u32_t x0, u32_t y0, u32_t x);
bool_t console_vline(struct console_t * console, u32_t code, u32_t x0, u32_t y0, u32_t y);
bool_t console_rect(struct console_t * console, u32_t hline, u32_t vline, u32_t lt, u32_t rt, u32_t lb, u32_t rb, u32_t x, u32_t y, u32_t w, u32_t h);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
