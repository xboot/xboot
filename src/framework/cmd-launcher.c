/*
 * kernel/command/cmd-launcher.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <package.h>
#include <shell/shell.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    launcher [framebuffer] [input]\r\n");
}

static const char zh_CN[] = X({
	"launcher":"启动器",
});

static inline void tabbar_main(struct xui_context_t * ctx)
{
	xui_layout_row(ctx, 1, (int[]){ -1 }, 50);
	xui_begin_panel_ex(ctx, "!header", XUI_PANEL_TRANSPARENT);
	{
		xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
		xui_text(ctx, "This is header");
	}
	xui_end_panel(ctx);

	xui_layout_row(ctx, 1, (int[]){ -1 }, 10);
	xui_split(ctx);

	xui_layout_row(ctx, 1, (int[]){ -1 }, -100);
	xui_begin_panel_ex(ctx, "!package", XUI_PANEL_TRANSPARENT);
	{
		struct hmap_entry_t * e;
		int widths[XUI_MAX_WIDTHS];
		int pw = 160;
		int n = clamp(xui_get_container(ctx)->region.w / pw, 1, XUI_MAX_WIDTHS);
		int w = xui_get_container(ctx)->region.w / n;
		for(int i = 0; i < n - 1; i++)
			widths[i] = w;
		widths[n - 1] = -1;
		xui_layout_row(ctx, n, widths, pw * 3 / 2 + 80);

		hmap_for_each_entry(e, get_package_list())
		{
			struct package_t * pkg = (struct package_t *)e->value;
			xui_layout_begin_column(ctx);
			{
				xui_layout_row(ctx, 1, (int[]){ -1 }, -50);
				if(xui_image_ex(ctx, package_get_panel(pkg), 0, XUI_IMAGE_COVER))
				{
					struct window_t * pos, * n;
					int flag = 0;
					list_for_each_entry_safe(pos, n, &ctx->w->wm->window, list)
					{
						if(strcmp(pos->task->name, package_get_path(pkg)) == 0)
						{
							window_to_front(pos);
							flag = 1;
							break;
						}
					}
					if(!flag)
						vmexec(package_get_path(pkg), ((struct task_data_t *)(ctx->priv))->fb, ((struct task_data_t *)(ctx->priv))->input);
				}
				xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
				xui_label_ex(ctx,package_get_name(pkg), XUI_OPT_TEXT_TOP);
			}
			xui_layout_end_column(ctx);
		}
	}
	xui_end_panel(ctx);

	xui_layout_row(ctx, 1, (int[]){ -1 }, 10);
	xui_split(ctx);

	xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
	xui_begin_panel_ex(ctx, "!bottom", XUI_PANEL_TRANSPARENT);
	{
		struct window_t * pos, * n;
		struct slist_t * sl, * e;
		xui_layout_row(ctx, 0, NULL, -1);
		xui_layout_width(ctx, 80);
		sl = slist_alloc();
		list_for_each_entry_safe(pos, n, &ctx->w->wm->window, list)
		{
			if(pos != ctx->w)
				slist_add(sl, pos, "%s", pos->task->name);
		}
		slist_sort(sl);
		slist_for_each_entry(e, sl)
		{
			pos = (struct window_t *)e->priv;
			if(xui_image_ex(ctx, package_get_icon(package_search(pos->task->name)), 0, XUI_IMAGE_COVER))
				window_to_front(pos);
		}
		slist_free(sl);
	}
	xui_end_panel(ctx);
}

static inline void tabbar_setting(struct xui_context_t * ctx)
{
	xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
	xui_badge(ctx, "This is setting");
}

static void launcher_window(struct xui_context_t * ctx)
{
	if(xui_begin_window_ex(ctx, "Launcher Window", NULL, XUI_WINDOW_FULLSCREEN))
	{
		static enum {
			LAUNCHER_TABBAR_MAIN	= 0,
			LAUNCHER_TABBAR_SETTING,
		} tabbar = LAUNCHER_TABBAR_MAIN;

		xui_layout_row(ctx, 2, (int[]){ 60, -1 }, -1);
		xui_begin_panel(ctx, "!tabbar");
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 40);
			if(xui_tabbar(ctx, 62060, NULL, tabbar == LAUNCHER_TABBAR_MAIN))
				tabbar = LAUNCHER_TABBAR_MAIN;
			if(xui_tabbar(ctx, 62061, NULL, tabbar == LAUNCHER_TABBAR_SETTING))
				tabbar = LAUNCHER_TABBAR_SETTING;
		}
		xui_end_panel(ctx);
		xui_begin_panel(ctx, "!context");
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
			switch(tabbar)
			{
			case LAUNCHER_TABBAR_MAIN:
				tabbar_main(ctx);
				break;
			case LAUNCHER_TABBAR_SETTING:
				tabbar_setting(ctx);
				break;
			default:
				break;
			}
		}
		xui_end_panel(ctx);
		xui_end_window(ctx);
	}
}

static void launcher(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	launcher_window(ctx);
	xui_end(ctx);
}

static void launcher_task(struct task_t * task, void * data)
{
	struct task_data_t * td = (struct task_data_t *)data;
	struct xui_context_t * ctx;

	if(td)
	{
		ctx = xui_context_alloc(td->fb, td->input, td);
		if(ctx)
		{
			switch(shash(setting_get("language", NULL)))
			{
			case 0x10d87d65: /* "zh-CN" */
				xui_load_lang(ctx, zh_CN, sizeof(zh_CN));
				break;
			default:
				break;
			}
			ctx->w->launcher = 1;
			xui_loop(ctx, launcher);
			xui_context_free(ctx);
		}
		task_data_free(td);
	}
}

static int do_launcher(int argc, char ** argv)
{
	const char * fb = (argc >= 2) ? argv[1] : NULL;
	const char * input = (argc >= 3) ? argv[2] : NULL;
	task_resume(task_create(NULL, "launcher", launcher_task, task_data_alloc(fb, input, NULL), 0, 0));
	return 0;
}

static struct command_t cmd_launcher = {
	.name	= "launcher",
	.desc	= "show launcher application of xui",
	.usage	= usage,
	.exec	= do_launcher,
};

static __init void launcher_cmd_init(void)
{
	register_command(&cmd_launcher);
}

static __exit void launcher_cmd_exit(void)
{
	unregister_command(&cmd_launcher);
}

command_initcall(launcher_cmd_init);
command_exitcall(launcher_cmd_exit);
