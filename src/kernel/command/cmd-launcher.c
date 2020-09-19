/*
 * kernel/command/cmd-launcher.c
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
#include <framework/vm.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    launcher [framebuffer] [input]\r\n");
}

static const char zh_CN[] = X({
	"launcher":"启动器",
});

static void launcher_window(struct xui_context_t * ctx)
{
	struct package_t * pkg;
	struct hmap_entry_t * e;

	if(xui_begin_window_ex(ctx, "Launcher Window", NULL, XUI_WINDOW_FULLSCREEN))
	{
		xui_layout_row(ctx, 1, (int[]){ -1 }, 100);
		xui_text(ctx, "This is head");

		xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
		xui_split(ctx);

		xui_layout_row(ctx, 1, (int[]){ -1 }, -50);
		xui_begin_panel(ctx, "Main panel");
		xui_layout_row(ctx, 0, NULL, -1);
		xui_layout_width(ctx, 200);
		hmap_for_each_entry(e, __package_list)
		{
			xui_layout_begin_column(ctx);
			xui_layout_row(ctx, 1, (int[]){ -1 }, -50);
			pkg = e->value;

			if(xui_button(ctx, package_get_name(pkg)))
			{
				struct window_t * pos, * n;
				int runing = 0;
				list_for_each_entry_safe(pos, n, &ctx->w->wm->window, list)
				{
					if(strcmp(pos->task->name, package_get_path(pkg)) == 0)
					{
						window_to_front(pos);
						runing = 1;
						break;
					}
				}
				if(!runing)
					vmexec(package_get_path(pkg), ((struct task_data_t *)(ctx->priv))->fb, ((struct task_data_t *)(ctx->priv))->input);
			}
			//xui_image_ex(ctx, package_get_panel(pkg), 0, XUI_IMAGE_COVER);

			xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
			xui_label(ctx, package_get_name(pkg));
			xui_layout_end_column(ctx);
		}
		xui_end_panel(ctx);

		xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
		xui_split(ctx);
		xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
		xui_text(ctx, "This is bottom");

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
			window_set_launcher(ctx->w, 1);
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
