/*
 * kernel/command/cmd-test.c
 */

#include <xboot.h>
#include <command/command.h>

#include <shell/ctrlc.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <xui/xui.h>

static char logbuf[64000];
static int logbuf_updated = 0;
static float bg[3] = { 90, 95, 100 };

static void write_log(const char *text)
{
	if(logbuf[0])
	{
		strcat(logbuf, "\n");
	}
	strcat(logbuf, text);
	logbuf_updated = 1;
}

static void test_window(struct xui_context_t * ctx)
{
	/* do window */
	if(xui_begin_window(ctx, "Demo Window", &(struct region_t){40, 40, 300, 450}))
	{
		struct xui_container_t * win = xui_get_current_container(ctx);
		win->region.w = max(win->region.w, 240);
		win->region.h = max(win->region.h, 300);

		/* window info */
		if(xui_header(ctx, "Window Info"))
		{
			struct xui_container_t *win = xui_get_current_container(ctx);
			char buf[64];
			xui_layout_row(ctx, 2, (int[] ) { 54, -1 }, 0);
			xui_label(ctx, "Position:");
			sprintf(buf, "%d, %d", win->region.x, win->region.y);
			xui_label(ctx, buf);
			xui_label(ctx, "Size:");
			sprintf(buf, "%d, %d", win->region.w, win->region.h);
			xui_label(ctx, buf);
		}

		/* labels + buttons */
		if(xui_header_ex(ctx, "Test Buttons", XUI_OPT_EXPANDED))
		{
			xui_layout_row(ctx, 3, (int[] ) { 86, -110, -1 }, 0);
			xui_label(ctx, "Test buttons 1:");
			if(xui_button(ctx, "Button 1"))
			{
				write_log("Pressed button 1");
			}
			if(xui_button(ctx, "Button 2"))
			{
				write_log("Pressed button 2");
			}
			xui_label(ctx, "Test buttons 2:");
			if(xui_button(ctx, "Button 3"))
			{
				write_log("Pressed button 3");
			}
			if(xui_button(ctx, "Popup"))
			{
				xui_open_popup(ctx, "Test Popup");
			}
			if(xui_begin_popup(ctx, "Test Popup"))
			{
				xui_button(ctx, "Hello");
				xui_button(ctx, "World");
				xui_end_popup(ctx);
			}
		}

		/* tree */
		if(xui_header_ex(ctx, "Tree and Text", XUI_OPT_EXPANDED))
		{
			xui_layout_row(ctx, 2, (int[] ) { 140, -1 }, 0);
			xui_layout_begin_column(ctx);
			if(xui_begin_treenode(ctx, "Test 1"))
			{
				if(xui_begin_treenode(ctx, "Test 1a"))
				{
					xui_label(ctx, "Hello");
					xui_label(ctx, "world");
					xui_end_treenode(ctx);
				}
				if(xui_begin_treenode(ctx, "Test 1b"))
				{
					if(xui_button(ctx, "Button 1"))
					{
						write_log("Pressed button 1");
					}
					if(xui_button(ctx, "Button 2"))
					{
						write_log("Pressed button 2");
					}
					xui_end_treenode(ctx);
				}
				xui_end_treenode(ctx);
			}
			if(xui_begin_treenode(ctx, "Test 2"))
			{
				xui_layout_row(ctx, 2, (int[] ) { 54, 54 }, 0);
				if(xui_button(ctx, "Button 3"))
				{
					write_log("Pressed button 3");
				}
				if(xui_button(ctx, "Button 4"))
				{
					write_log("Pressed button 4");
				}
				if(xui_button(ctx, "Button 5"))
				{
					write_log("Pressed button 5");
				}
				if(xui_button(ctx, "Button 6"))
				{
					write_log("Pressed button 6");
				}
				xui_end_treenode(ctx);
			}
			if(xui_begin_treenode(ctx, "Test 3"))
			{
				static int checks[3] = { 1, 0, 1 };
				xui_checkbox(ctx, "Checkbox 1", &checks[0]);
				xui_checkbox(ctx, "Checkbox 2", &checks[1]);
				xui_checkbox(ctx, "Checkbox 3", &checks[2]);
				xui_end_treenode(ctx);
			}
			xui_layout_end_column(ctx);

			xui_layout_begin_column(ctx);
			xui_layout_row(ctx, 1, (int[] ) { -1 }, 0);
			xui_text(ctx, "Lorem ipsum dolor sit amet, consectetur adipiscing "
					"elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
					"ipsum, eu varius magna felis a nulla.");
			xui_layout_end_column(ctx);
		}

		/* background color sliders */
		if(xui_header_ex(ctx, "Background Color", XUI_OPT_EXPANDED))
		{
			xui_layout_row(ctx, 2, (int[] ) { -78, -1 }, 74);
			/* sliders */
			xui_layout_begin_column(ctx);
			xui_layout_row(ctx, 2, (int[] ) { 46, -1 }, 0);
			xui_label(ctx, "Red:");
			xui_slider(ctx, &bg[0], 0, 255);
			xui_label(ctx, "Green:");
			xui_slider(ctx, &bg[1], 0, 255);
			xui_label(ctx, "Blue:");
			xui_slider(ctx, &bg[2], 0, 255);
			xui_layout_end_column(ctx);
			/* color preview */
			struct region_t * r = xui_layout_next(ctx);
			struct color_t c;
			color_init(&c, bg[0], bg[1], bg[2], 255);
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &c);
			char buf[32];
			sprintf(buf, "#%02X%02X%02X", (int)bg[0], (int)bg[1], (int)bg[2]);
			xui_control_draw_text(ctx, buf, r, &ctx->style.text.text_color, 0);
		}

		xui_end_window(ctx);
	}
}

static void log_window(struct xui_context_t *ctx)
{
	if(xui_begin_window(ctx, "Log Window", &(struct region_t){350, 40, 300, 200}))
	{
		/* output text panel */
		xui_layout_row(ctx, 1, (int[] ) { -1 }, -25);
		xui_begin_panel(ctx, "Log Output");
		struct xui_container_t * panel = xui_get_current_container(ctx);
		xui_layout_row(ctx, 1, (int[] ) { -1 }, -1);
		xui_text(ctx, logbuf);
		xui_end_panel(ctx);
		if(logbuf_updated)
		{
			panel->scroll_y = panel->content_height;
			logbuf_updated = 0;
		}

		/* input textbox + submit button */
		static char buf[128];
		int submitted = 0;
		xui_layout_row(ctx, 2, (int[] ) { -70, -1 }, 0);
		if(xui_textbox(ctx, buf, sizeof(buf)) & XUI_RES_SUBMIT)
		{
			xui_set_focus(ctx, ctx->last_id);
			submitted = 1;
		}
		if(xui_button(ctx, "Submit"))
		{
			submitted = 1;
		}
		if(submitted)
		{
			write_log(buf);
			buf[0] = '\0';
		}

		xui_end_window(ctx);
	}
}

static int uint8_slider(struct xui_context_t * ctx, unsigned char *value, int low, int high)
{
	float tmp;
	xui_push_id(ctx, &value, sizeof(value));
	tmp = *value;
	int res = xui_slider_ex(ctx, &tmp, low, high, 0, "%.0f", 0);
	*value = tmp;
	xui_pop_id(ctx);
	return res;
}

static void style_window(struct xui_context_t * ctx)
{
	static struct { const char * label; int idx; } colors[] = {
		{ "border:",       XUI_COLOR_BORDER      },
		{ "base:",         XUI_COLOR_BASE        },
		{ "basehover:",    XUI_COLOR_BASEHOVER   },
		{ "basefocus:",    XUI_COLOR_BASEFOCUS   },
		{ NULL }
	};

	if(xui_begin_window(ctx, "Style Editor", &(struct region_t){350, 250, 300, 240}))
	{
		int sw = xui_get_current_container(ctx)->body.w * 0.14;
		xui_layout_row(ctx, 6, (int[] ) { 80, sw, sw, sw, sw, -1 }, 0);
		for(int i = 0; colors[i].label; i++)
		{
			xui_label(ctx, colors[i].label);
			uint8_slider(ctx, &ctx->style.colors[i].r, 0, 255);
			uint8_slider(ctx, &ctx->style.colors[i].g, 0, 255);
			uint8_slider(ctx, &ctx->style.colors[i].b, 0, 255);
			uint8_slider(ctx, &ctx->style.colors[i].a, 0, 255);
			struct region_t * r = xui_layout_next(ctx);
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &ctx->style.colors[i]);
		}
		xui_end_window(ctx);
	}
}

void ttt_test(struct xui_context_t * ctx)
{
	if(xui_begin_window(ctx, "Style window", NULL))
	{
		if(xui_header_ex(ctx, "Background Color", XUI_OPT_EXPANDED))
		{
			xui_layout_row(ctx, 2, (int[] ) { -78, -1 }, 74);
			/* sliders */
			xui_layout_begin_column(ctx);
			xui_layout_row(ctx, 2, (int[] ) { 46, -1 }, 0);
			xui_label(ctx, "Red:");
			uint8_slider(ctx, &ctx->style.background_color.r, 0, 255);
			xui_label(ctx, "Green:");
			uint8_slider(ctx, &ctx->style.background_color.g, 0, 255);
			xui_label(ctx, "Blue:");
			uint8_slider(ctx, &ctx->style.background_color.b, 0, 255);
			xui_layout_end_column(ctx);
			/* color preview */
			struct region_t * r = xui_layout_next(ctx);
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &ctx->style.background_color);
			xui_control_draw_text(ctx, xui_format(ctx, "#%02X%02X%02X", ctx->style.background_color.r, ctx->style.background_color.g, ctx->style.background_color.b), r, &ctx->style.text.text_color, 0);
		}

		xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
		if(xui_button(ctx, "button test1"))
		{
		}
		if(xui_button(ctx, "button test2"))
		{
		}
		xui_header(ctx, "header");
		if(xui_begin_treenode(ctx, "treenode"))
		{
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1, }, 40);
			for(int i = 0; i < 8; i++)
			{
				xui_button_ex(ctx, xui_format(ctx, "btn %s %d", "normal", i), (i << 8) | XUI_OPT_TEXT_CENTER);
			}

			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1, }, 50);
			for(int i = 0; i < 8; i++)
			{
				xui_button_ex(ctx, xui_format(ctx, "btn %s %d", "outline", i), (i << 8) | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER);
			}
			xui_end_treenode(ctx);
		}
		xui_end_window(ctx);
	}
}

static void process_frame(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	style_window(ctx);
	log_window(ctx);
	test_window(ctx);
	ttt_test(ctx);
	xui_end(ctx);
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static void xui_test_task(struct task_t * task, void * data)
{
	struct xui_context_t * ctx = xui_context_alloc(NULL, NULL, NULL);
	xui_loop(ctx, process_frame);
	xui_context_free(ctx);
}

static int do_test(int argc, char ** argv)
{
	struct task_t * task;

	task = task_create(NULL, "demo", xui_test_task, NULL, 0, 0);
	task_resume(task);
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
