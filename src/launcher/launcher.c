/*
 * launcher/launcher.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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



static int index = 0;

static void test_window(struct xui_context_t * ctx)
{
	if(xui_begin_window(ctx, "Test Window", &(struct region_t){400, 0, 200, 300}))
	{
		xui_layout_row(ctx, 1, (int[]){ -1 }, 100);

		if(xui_button(ctx, "test1"))
		{
			index = (index + 1) % 5;
		}
		if(xui_button(ctx, "test2"))
		{
			index = (index + 5 - 1) % 5;
		}
		xui_end_window(ctx);
	}
}

static void launcher_window(struct xui_context_t * ctx)
{
	if(xui_begin_window_ex(ctx, "Launcher Window", &(struct region_t){0, 0, 400, 400}, XUI_WINDOW_TRANSPARENT))
	{
		xui_layout_row(ctx, 1, (int[]){ -1 }, -1);

		struct region_t r;
		region_clone(&r, xui_layout_next(ctx));

		for(int i = 0; i < 5; i++)
		{
			if(i == index)
				xui_draw_icon(ctx, ctx->style.font.icon_family, 0xe8db, r.x + i * 128, r.y, 128 + 32, 128 + 32, &(struct color_t){255, 255, 255, 255});
			else
				xui_draw_icon(ctx, ctx->style.font.icon_family, 0xe8db, r.x + i * 128, r.y, 128, 128, &(struct color_t){255, 255, 255, 255});
		}

		xui_end_window(ctx);
	}
}

static void launcher(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	launcher_window(ctx);
	test_window(ctx);
	xui_end(ctx);
}

static void launcher_task(struct task_t * task, void * data)
{
	struct xui_context_t * ctx;

	ctx = xui_context_alloc(task->fb, task->input, data);
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
}

static int do_launcher(int argc, char ** argv)
{
	const char * fb = (argc >= 2) ? argv[1] : NULL;
	const char * input = (argc >= 3) ? argv[2] : NULL;
	task_create(NULL, "launcher", fb, input, launcher_task, NULL, 0, 0);
	return 0;
}

static struct command_t cmd_launcher = {
	.name	= "launcher_new",
	.desc	= "default launcher application",
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
