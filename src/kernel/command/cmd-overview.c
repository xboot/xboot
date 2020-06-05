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
	if(xui_begin_window(ctx, "Overview Window", &(struct region_t){100, 100, 300, 300}))
	{
		struct xui_container_t * win = xui_get_current_container(ctx);
		win->region.w = max(win->region.w, 50);
		win->region.h = max(win->region.h, 50);

		if(xui_header(ctx, "Window Info"))
		{
			struct xui_container_t * win = xui_get_current_container(ctx);
			xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
			xui_label(ctx, "Position :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.x, win->region.y));
			xui_label(ctx, "Size :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.w, win->region.h));
		}

		if(xui_header(ctx, "Text"))
		{
			xui_label_ex(ctx, "Label align left", XUI_OPT_TEXT_LEFT);
			xui_label_ex(ctx, "Label align center", XUI_OPT_TEXT_CENTER);
			xui_label_ex(ctx, "Label align right", XUI_OPT_TEXT_RIGHT);
			xui_label_ex(ctx, "Label align top", XUI_OPT_TEXT_TOP);
			xui_label_ex(ctx, "Label align bottom", XUI_OPT_TEXT_BOTTOM);
			xui_text(ctx, "This is a long text to show dynamic window changes on multiline text");
		}


		xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
		if(xui_button(ctx, "button test1"))
		{
		}
		if(xui_button(ctx, "button test2"))
		{
		}
		xui_header(ctx, "header");
		static float t = 20;
		xui_number(ctx, &t, 1);
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
