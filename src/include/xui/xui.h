#ifndef __XUI_H__
#define __XUI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/window.h>
#include <graphic/point.h>
#include <graphic/region.h>
#include <graphic/color.h>
#include <graphic/font.h>

#define MU_COMMANDLIST_SIZE     (256 * 1024)
#define MU_ROOTLIST_SIZE        32
#define MU_CONTAINERSTACK_SIZE  32
#define MU_CLIPSTACK_SIZE       32
#define MU_IDSTACK_SIZE         32
#define MU_LAYOUTSTACK_SIZE     16
#define MU_CONTAINERPOOL_SIZE   48
#define MU_TREENODEPOOL_SIZE    48
#define MU_MAX_WIDTHS           16
#define MU_REAL_FMT             "%.3g"
#define MU_SLIDER_FMT           "%.2f"
#define MU_MAX_FMT              127

enum {
	MU_CLIP_PART = 1,
	MU_CLIP_ALL,
};

enum {
	MU_COLOR_TEXT,
	MU_COLOR_BORDER,
	MU_COLOR_WINDOWBG,
	MU_COLOR_TITLEBG,
	MU_COLOR_TITLETEXT,
	MU_COLOR_PANELBG,
	MU_COLOR_BUTTON,
	MU_COLOR_BUTTONHOVER,
	MU_COLOR_BUTTONFOCUS,
	MU_COLOR_BASE,
	MU_COLOR_BASEHOVER,
	MU_COLOR_BASEFOCUS,
	MU_COLOR_SCROLLBASE,
	MU_COLOR_SCROLLTHUMB,
	MU_COLOR_MAX,
};

enum {
	MU_ICON_CLOSE = 1,
	MU_ICON_CHECK,
	MU_ICON_COLLAPSED,
	MU_ICON_EXPANDED,
	MU_ICON_MAX,
};

enum {
	MU_RES_ACTIVE       = (1 << 0),
	MU_RES_SUBMIT       = (1 << 1),
	MU_RES_CHANGE       = (1 << 2),
};

enum {
	MU_OPT_ALIGNCENTER  = (1 << 0),
	MU_OPT_ALIGNRIGHT   = (1 << 1),
	MU_OPT_NOINTERACT   = (1 << 2),
	MU_OPT_NOFRAME      = (1 << 3),
	MU_OPT_NORESIZE     = (1 << 4),
	MU_OPT_NOSCROLL     = (1 << 5),
	MU_OPT_NOCLOSE      = (1 << 6),
	MU_OPT_NOTITLE      = (1 << 7),
	MU_OPT_HOLDFOCUS    = (1 << 8),
	MU_OPT_AUTOSIZE     = (1 << 9),
	MU_OPT_POPUP        = (1 << 10),
	MU_OPT_CLOSED       = (1 << 11),
	MU_OPT_EXPANDED     = (1 << 12),
};

enum {
	MU_MOUSE_LEFT       = (1 << 0),
	MU_MOUSE_RIGHT      = (1 << 1),
	MU_MOUSE_MIDDLE     = (1 << 2),
};

enum {
	MU_KEY_SHIFT        = (1 << 0),
	MU_KEY_CTRL         = (1 << 1),
	MU_KEY_ALT          = (1 << 2),
	MU_KEY_BACKSPACE    = (1 << 3),
	MU_KEY_RETURN       = (1 << 4),
};

typedef struct { int x, y; } mu_Vec2;
typedef struct { int x, y, w, h; } mu_Rect;

enum xui_command_type_t {
	XUI_COMMAND_TYPE_BASE	= 0,
	XUI_COMMAND_TYPE_JUMP	= 1,
	XUI_COMMAND_TYPE_CLIP	= 2,
	XUI_COMMAND_TYPE_RECT	= 3,
	XUI_COMMAND_TYPE_TEXT	= 4,
	XUI_COMMAND_TYPE_ICON	= 5,
};

struct xui_command_base_t {
	enum xui_command_type_t type;
	int size;
};

struct xui_command_jump_t {
	enum xui_command_type_t type;
	int size;

	void * addr;
};

struct xui_command_clip_t {
	enum xui_command_type_t type;
	int size;

	mu_Rect rect;
};

struct xui_command_rect_t {
	enum xui_command_type_t type;
	int size;

	mu_Rect rect;
	struct color_t color;
};

struct xui_command_text_t {
	enum xui_command_type_t type;
	int size;
	void * font;

	int x, y;
	struct color_t color;
	char str[1];
};

struct xui_command_icon_t {
	enum xui_command_type_t type;
	int size;

	int id;
	mu_Rect rect;
	struct color_t color;
};

union xui_command_t {
	struct xui_command_base_t base;
	struct xui_command_jump_t jump;
	struct xui_command_clip_t clip;
	struct xui_command_rect_t rect;
	struct xui_command_text_t text;
	struct xui_command_icon_t icon;
};

struct xui_pool_item_t {
	unsigned int id;
	int last_update;
};

struct xui_layout_t {
	mu_Rect body;
	mu_Rect next;
	int position_x, position_y;
	int size_width, size_height;
	int max_width, max_height;
	int widths[MU_MAX_WIDTHS];
	int items;
	int item_index;
	int next_row;
	int next_type;
	int indent;
};

struct xui_container_t {
	union xui_command_t * head, * tail;
	mu_Rect rect;
	mu_Rect body;
	int content_width;
	int content_height;
	mu_Vec2 scroll_abc;
	int zindex;
	int open;
};

struct xui_style_t {
	struct color_t bgcol;
	void * font;
	int width;
	int height;
	int padding;
	int spacing;
	int indent;
	int title_height;
	int scrollbar_size;
	int thumb_size;
	struct color_t colors[MU_COLOR_MAX];
};

struct xui_context_t {
	/*
	 * Context
	 */
	struct window_t * w;
	struct font_context_t * f;

	/*
	 * Core state
	 */
	struct xui_style_t style;
	struct region_t clip;
	unsigned int hover;
	unsigned int focus;
	unsigned int last_id;
	mu_Rect last_rect;
	int last_zindex;
	int updated_focus;
	int frame;
	struct xui_container_t * hover_root;
	struct xui_container_t * next_hover_root;
	struct xui_container_t * scroll_target;
	char number_edit_buf[MU_MAX_FMT];
	unsigned int number_edit;

	/*
	 * Stack
	 */
	struct {
		int idx;
		char items[MU_COMMANDLIST_SIZE];
	} command_list;

	struct {
		int idx;
		struct xui_container_t * items[MU_ROOTLIST_SIZE];
	} root_list;

	struct {
		int idx;
		struct xui_container_t * items[MU_CONTAINERSTACK_SIZE];
	} container_stack;

	struct {
		int idx;
		mu_Rect items[MU_CLIPSTACK_SIZE];
	} clip_stack;

	struct {
		int idx;
		unsigned int items[MU_IDSTACK_SIZE];
	} id_stack;

	struct {
		int idx;
		struct xui_layout_t items[MU_LAYOUTSTACK_SIZE];
	} layout_stack;

	/*
	 * Retained state pool
	 */
	struct xui_pool_item_t container_pool[MU_CONTAINERPOOL_SIZE];
	struct xui_container_t containers[MU_CONTAINERPOOL_SIZE];
	struct xui_pool_item_t treenode_pool[MU_TREENODEPOOL_SIZE];

	/*
	 * Input state
	 */
	int mouse_pos_x, mouse_pos_y;
	int last_mouse_pos_x, last_mouse_pos_y;
	int mouse_delta_x, mouse_delta_y;
	int scroll_delta_x, scroll_delta_y;
	int mouse_down;
	int mouse_pressed;
	int key_down;
	int key_pressed;
	char input_text[32];

	/*
	 * Callback
	 */
	int (*text_width)(void * font, const char *str, int len);
	int (*text_height)(void * font);
	void (*draw_frame)(struct xui_context_t *ctx, mu_Rect rect, int colorid);
};

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct xui_style_t * style);
void xui_context_free(struct xui_context_t * ctx);
void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *));

void xui_begin(struct xui_context_t * ctx);
void xui_end(struct xui_context_t * ctx);
void xui_set_focus(struct xui_context_t * ctx, unsigned int id);
void xui_set_front(struct xui_context_t * ctx, struct xui_container_t * c);
unsigned int xui_get_id(struct xui_context_t * ctx, const void * data, int size);
void xui_push_id(struct xui_context_t * ctx, const void * data, int size);
void xui_pop_id(struct xui_context_t * ctx);
mu_Rect mu_get_clip_rect(struct xui_context_t * ctx);
void mu_push_clip_rect(struct xui_context_t * ctx, mu_Rect rect);
void mu_pop_clip_rect(struct xui_context_t * ctx);
int mu_check_clip(struct xui_context_t * ctx, mu_Rect r);
struct xui_container_t * xui_get_current_container(struct xui_context_t * ctx);
struct xui_container_t * xui_get_container(struct xui_context_t * ctx, const char * name);

int xui_pool_init(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id);
int xui_pool_get(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id);
void xui_pool_update(struct xui_context_t * ctx, struct xui_pool_item_t * items, int idx);

void mu_input_mousemove(struct xui_context_t * ctx, int x, int y);
void mu_input_mousedown(struct xui_context_t * ctx, int x, int y, int btn);
void mu_input_mouseup(struct xui_context_t * ctx, int x, int y, int btn);
void mu_input_scroll(struct xui_context_t * ctx, int x, int y);
void mu_input_keydown(struct xui_context_t * ctx, int key);
void mu_input_keyup(struct xui_context_t * ctx, int key);
void mu_input_text(struct xui_context_t * ctx, const char * text);

union xui_command_t * xui_push_command(struct xui_context_t * ctx, enum xui_command_type_t type, int size);
int xui_next_command(struct xui_context_t * ctx, union xui_command_t ** cmd);
void xui_set_clip(struct xui_context_t * ctx, mu_Rect rect);
void mu_draw_rect(struct xui_context_t * ctx, mu_Rect rect, struct color_t * c);
void mu_draw_box(struct xui_context_t * ctx, mu_Rect rect, struct color_t * c);
void mu_draw_text(struct xui_context_t * ctx, void * font, const char * str, int len, int x, int y, struct color_t * c);
void mu_draw_icon(struct xui_context_t * ctx, int id, mu_Rect rect, struct color_t * c);

void xui_layout_width(struct xui_context_t * ctx, int width);
void xui_layout_height(struct xui_context_t  *ctx, int height);
void xui_layout_row(struct xui_context_t * ctx, int items, const int * widths, int height);
void xui_layout_begin_column(struct xui_context_t * ctx);
void xui_layout_end_column(struct xui_context_t * ctx);
void xui_layout_set_next(struct xui_context_t * ctx, mu_Rect r, int relative);
mu_Rect xui_layout_next(struct xui_context_t * ctx);

void xui_draw_control_frame(struct xui_context_t * ctx, unsigned int id, mu_Rect rect, int colorid, int opt);
void xui_draw_control_text(struct xui_context_t * ctx, const char * str, mu_Rect rect, int colorid, int opt);
int xui_mouse_over(struct xui_context_t * ctx, mu_Rect rect);
void xui_update_control(struct xui_context_t * ctx, unsigned int id, mu_Rect rect, int opt);

#define mu_button(ctx, label)             mu_button_ex(ctx, label, 0, MU_OPT_ALIGNCENTER)
#define mu_textbox(ctx, buf, bufsz)       mu_textbox_ex(ctx, buf, bufsz, 0)
#define mu_slider(ctx, value, lo, hi)     mu_slider_ex(ctx, value, lo, hi, 0, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_number(ctx, value, step)       mu_number_ex(ctx, value, step, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_header(ctx, label)             mu_header_ex(ctx, label, 0)
#define mu_begin_treenode(ctx, label)     mu_begin_treenode_ex(ctx, label, 0)
#define mu_begin_window(ctx, title, rect) mu_begin_window_ex(ctx, title, rect, 0)
#define mu_begin_panel(ctx, name)         mu_begin_panel_ex(ctx, name, 0)

void mu_text(struct xui_context_t *ctx, const char *text);
void mu_label(struct xui_context_t *ctx, const char *text);
int mu_button_ex(struct xui_context_t *ctx, const char *label, int icon, int opt);
int mu_checkbox(struct xui_context_t *ctx, const char *label, int *state);
int mu_textbox_raw(struct xui_context_t *ctx, char *buf, int bufsz, unsigned int id, mu_Rect r, int opt);
int mu_textbox_ex(struct xui_context_t *ctx, char *buf, int bufsz, int opt);
int mu_slider_ex(struct xui_context_t *ctx, float *value, float low, float high, float step, const char *fmt, int opt);
int mu_number_ex(struct xui_context_t *ctx, float *value, float step, const char *fmt, int opt);
int mu_header_ex(struct xui_context_t *ctx, const char *label, int opt);
int mu_begin_treenode_ex(struct xui_context_t *ctx, const char *label, int opt);
void mu_end_treenode(struct xui_context_t *ctx);
int mu_begin_window_ex(struct xui_context_t *ctx, const char *title, mu_Rect rect, int opt);
void mu_end_window(struct xui_context_t *ctx);
void mu_open_popup(struct xui_context_t *ctx, const char *name);
int mu_begin_popup(struct xui_context_t *ctx, const char *name);
void mu_end_popup(struct xui_context_t *ctx);
void mu_begin_panel_ex(struct xui_context_t *ctx, const char *name, int opt);
void mu_end_panel(struct xui_context_t *ctx);

mu_Vec2 mu_vec2(int x, int y);
mu_Rect mu_rect(int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif

#endif /* __XUI_H__ */
