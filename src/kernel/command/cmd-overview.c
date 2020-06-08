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
	if(xui_begin_window(ctx, "Overview Window", &(struct region_t){10, 10, 400, 400}))
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

		if(xui_begin_treenode(ctx, "Button"))
		{
			static const char * btnstr[] = {
				"Primary",
				"Secondary",
				"Success",
				"Info",
				"Warning",
				"Danger",
				"Light",
				"Dark",
			};
			if(xui_begin_treenode(ctx, "Normal Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, btnstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Rounded Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, btnstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, btnstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Rounded Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, btnstr[i], 0, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Normal Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Rounded Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_treenode(ctx);
			}

			if(xui_begin_treenode(ctx, "Rounded Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, NULL, 0xf010 + i, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_treenode(ctx);
			}
			xui_end_treenode(ctx);
		}

		if(xui_begin_treenode(ctx, "Checkbox"))
		{
			static int states[3] = { 1, 0, 1 };
			xui_checkbox(ctx, "Checkbox 1", &states[0]);
			xui_checkbox(ctx, "Checkbox 2", &states[1]);
			xui_checkbox(ctx, "Checkbox 3", &states[2]);
			xui_end_treenode(ctx);
		}

		if(xui_header(ctx, "Label"))
		{
			xui_label_ex(ctx, "Label align left", XUI_OPT_TEXT_LEFT);
			xui_label_ex(ctx, "Label align center", XUI_OPT_TEXT_CENTER);
			xui_label_ex(ctx, "Label align right", XUI_OPT_TEXT_RIGHT);
			xui_label_ex(ctx, "Label align top", XUI_OPT_TEXT_TOP);
			xui_label_ex(ctx, "Label align bottom", XUI_OPT_TEXT_BOTTOM);
		}

		if(xui_header(ctx, "Split"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			xui_split(ctx);
			xui_split(ctx);
			xui_split(ctx);
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 80);
			xui_split_ex(ctx, XUI_SPLIT_VERTICAL);
			xui_split_ex(ctx, XUI_SPLIT_VERTICAL);
			xui_split_ex(ctx, XUI_SPLIT_VERTICAL);
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
