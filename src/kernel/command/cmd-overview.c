/*
 * kernel/command/cmd-overview.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    overview\r\n");
}

static void overview_window(struct xui_context_t * ctx)
{
	static const char * wcstr[] = {
		"Primary",
		"Secondary",
		"Success",
		"Info",
		"Warning",
		"Danger",
		"Light",
		"Dark",
	};

	if(xui_begin_window(ctx, "Overview Window", &(struct region_t){10, 10, 400, 400}))
	{
		struct xui_container_t * win = xui_get_current_container(ctx);
		win->region.w = max(win->region.w, 48);
		win->region.h = max(win->region.h, 48);

		if(xui_begin_tree(ctx, "Window Info"))
		{
			struct xui_container_t * win = xui_get_current_container(ctx);
			xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
			xui_label(ctx, "Position :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.x, win->region.y));
			xui_label(ctx, "Size :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.w, win->region.h));
			xui_label(ctx, "Frame :");
			xui_label(ctx, xui_format(ctx, "%d", ctx->frame));
			xui_label(ctx, "Fps :");
			xui_label(ctx, xui_format(ctx, "%d", ctx->fps));
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Button"))
		{
			if(xui_begin_tree(ctx, "Normal Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, wcstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, wcstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, wcstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, wcstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Normal Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Checkbox"))
		{
			static int states[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			for(int i = 0; i < 8; i++)
			{
				xui_checkbox_ex(ctx, wcstr[i], &states[i], (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Radio"))
		{
			static enum option_t {
				PRIMARY,
				SECONDARY,
				SUCCESS,
				INFO,
				WARNING,
				DANGER,
				LIGHT,
				DARK,
			} op = PRIMARY;
			if(xui_radio_ex(ctx, wcstr[0], (op == PRIMARY), XUI_RADIO_PRIMARY))
				op = PRIMARY;
			if(xui_radio_ex(ctx, wcstr[1], (op == SECONDARY), XUI_RADIO_SECONDARY))
				op = SECONDARY;
			if(xui_radio_ex(ctx, wcstr[2], (op == SUCCESS), XUI_RADIO_SUCCESS))
				op = SUCCESS;
			if(xui_radio_ex(ctx, wcstr[3], (op == INFO), XUI_RADIO_INFO))
				op = INFO;
			if(xui_radio_ex(ctx, wcstr[4], (op == WARNING), XUI_RADIO_WARNING))
				op = WARNING;
			if(xui_radio_ex(ctx, wcstr[5], (op == DANGER), XUI_RADIO_DANGER))
				op = DANGER;
			if(xui_radio_ex(ctx, wcstr[6], (op == LIGHT), XUI_RADIO_LIGHT))
				op = LIGHT;
			if(xui_radio_ex(ctx, wcstr[7], (op == DARK), XUI_RADIO_DARK))
				op = DARK;
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Toggle"))
		{
			static int states[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			for(int i = 0; i < 8; i++)
			{
				xui_toggle_ex(ctx, &states[i], (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Slider"))
		{
			static double h[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_slider_ex(ctx, &h[i], 0, 100, 0, (i << 8) | XUI_SLIDER_HORIZONTAL);
			}
			static double v[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_slider_ex(ctx, &v[i], 0, 100, 0, (i << 8) | XUI_SLIDER_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Number"))
		{
			if(xui_begin_tree(ctx, "Normal Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_ROUNDED | XUI_NUMBER_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Textedit"))
		{
			static char buf[128];
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			if(xui_textedit(ctx, buf, sizeof(buf)) & (1 << 1))
			{
				xui_set_focus(ctx, ctx->last_id);
				buf[0] = 0;
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Badge"))
		{
			if(xui_begin_tree(ctx, "Normal Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8));
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_ROUNDED | XUI_BADGE_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Progress"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_progress_ex(ctx, (i + 1) * 10, (i << 8) | XUI_PROGRESS_HORIZONTAL);
			}
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_progress_ex(ctx, (i + 1) * 10, (i << 8) | XUI_PROGRESS_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Radialbar"))
		{
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 100);
			for(int i = 0; i < 8; i++)
			{
				xui_radialbar_ex(ctx, (i + 1) * 10, (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Spinner"))
		{
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 60);
			for(int i = 0; i < 8; i++)
			{
				xui_spinner_ex(ctx, (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Split"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_split_ex(ctx, (i << 8) | XUI_SPLIT_HORIZONTAL);
			}
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_split_ex(ctx, (i << 8) | XUI_SPLIT_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_header(ctx, "Label"))
		{
			xui_label_ex(ctx, "Label align left", XUI_OPT_TEXT_LEFT);
			xui_label_ex(ctx, "Label align center", XUI_OPT_TEXT_CENTER);
			xui_label_ex(ctx, "Label align right", XUI_OPT_TEXT_RIGHT);
			xui_label_ex(ctx, "Label align top", XUI_OPT_TEXT_TOP);
			xui_label_ex(ctx, "Label align bottom", XUI_OPT_TEXT_BOTTOM);
		}

		if(xui_header(ctx, "Text"))
		{
			xui_text(ctx, "This is a long text to show dynamic window changes on multiline text");
		}

		xui_end_window(ctx);
	}
}

static void overview(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	overview_window(ctx);
	xui_end(ctx);
}

static void overview_task(struct task_t * task, void * data)
{
	struct xui_context_t * ctx = xui_context_alloc(NULL, NULL, NULL);
	xui_loop(ctx, overview);
	xui_context_free(ctx);
}

static int do_overview(int argc, char ** argv)
{
	task_resume(task_create(NULL, "overview", overview_task, NULL, 0, 0));
	return 0;
}

static struct command_t cmd_overview = {
	.name	= "overview",
	.desc	= "show overview application of xui",
	.usage	= usage,
	.exec	= do_overview,
};

static __init void overview_cmd_init(void)
{
	register_command(&cmd_overview);
}

static __exit void overview_cmd_exit(void)
{
	unregister_command(&cmd_overview);
}

command_initcall(overview_cmd_init);
command_exitcall(overview_cmd_exit);
