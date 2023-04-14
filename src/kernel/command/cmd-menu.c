/*
 * kernel/command/cmd-menu.c
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
#include <command/command.h>

struct menu_item_t {
	int icon;
	char * title;
	char * command;
};

struct menu_ctx_t {
	struct menu_item_t * items;
	int nitem;
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    menu <json file> [framebuffer] [input]\r\n");
}

static const char menu_json[] = X([
	{
		"icon": 61252,
		"title": "Sleep",
		"command": "sleep"
	},
	{
		"icon": 61523,
		"title": "Reboot",
		"command": "reboot"
	},
	{
		"icon": 59564,
		"title": "Shutdown",
		"command": "shutdown"
	},
	{
		"icon": 59513,
		"title": "Exit menu program",
		"command": "kill menu;"
	}
]);

static void menu_window(struct xui_context_t * ctx)
{
	struct menu_ctx_t * mctx = (struct menu_ctx_t *)ctx->priv;

	if(xui_begin_window_ex(ctx, "Menu Window", NULL, XUI_WINDOW_FULLSCREEN | XUI_OPT_NOSCROLL))
	{
		struct xui_container_t * win = xui_get_container(ctx);
		struct region_t * r = &win->region;
		int height = min(mctx->nitem * 45 + (mctx->nitem + 1) * ctx->style.layout.spacing, r->h);
		xui_layout_set_next(ctx, &(struct region_t){ r->w / 3, (r->h - height) / 2, r->w / 3, height}, 1);
		xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
		xui_begin_panel_ex(ctx, "!menu", XUI_PANEL_TRANSPARENT);
		{
			static int idx = 0;
			xui_layout_row(ctx, 1, (int[]){ -1 }, 45);
			for(int i = 0; i < mctx->nitem; i++)
			{
				if(xui_tabbar(ctx, mctx->items[i].icon, mctx->items[i].title, idx == i))
				{
					idx = i;
					shell_system(mctx->items[idx].command);
				}
			}
			if(ctx->key_pressed & (XUI_KEY_UP | XUI_KEY_LEFT | XUI_KEY_VOLUME_UP))
				idx = (idx - 1 + mctx->nitem) % mctx->nitem;
			else if(ctx->key_pressed & (XUI_KEY_DOWN | XUI_KEY_RIGHT | XUI_KEY_VOLUME_DOWN | XUI_KEY_MENU))
				idx = (idx + 1 + mctx->nitem) % mctx->nitem;
			else if(ctx->key_pressed & (XUI_KEY_ENTER | XUI_KEY_POWER))
				shell_system(mctx->items[idx].command);
		}
		xui_end_panel(ctx);
		xui_end_window(ctx);
	}
}

static void menu(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	menu_window(ctx);
	xui_end(ctx);
}

static void menu_task(struct task_t * task, void * data)
{
	struct menu_ctx_t * mctx = (struct menu_ctx_t *)data;
	struct xui_context_t * ctx;

	ctx = xui_context_alloc(task->fb, task->input, mctx);
	if(ctx)
	{
		xui_loop(ctx, menu);
		xui_context_free(ctx);
		if(mctx)
		{
			if(mctx->items && (mctx->nitem > 0))
			{
				for(int i = 0; i < mctx->nitem; i++)
				{
					if(mctx->items[i].title)
						free(mctx->items[i].title);
					if(mctx->items[i].command)
						free(mctx->items[i].command);
				}
				free(mctx->items);
			}
			free(mctx);
		}
	}
}

struct menu_ctx_t * menu_ctx_alloc(const char * json, int len, char * errbuf)
{
	struct menu_ctx_t * mctx = NULL;

	if(json && (len > 0))
	{
		struct json_value_t * v = json_parse(json, len, errbuf);
		if(v && (v->type == JSON_ARRAY) && (v->u.array.length > 0))
		{
			mctx = malloc(sizeof(struct menu_ctx_t) * v->u.array.length);
			if(mctx)
			{
				mctx->nitem = v->u.array.length;
				mctx->items = calloc(mctx->nitem, sizeof(struct menu_item_t));
				if(mctx->items)
				{
					for(int i = 0; i < v->u.array.length; i++)
					{
						struct json_value_t * o = v->u.array.values[i];
						if(o && (o->type == JSON_OBJECT))
						{
							for(int j = 0; j < o->u.object.length; j++)
							{
								struct json_value_t * e = o->u.object.values[j].value;
								switch(shash(o->u.object.values[j].name))
								{
								case 0x7c98572e: /* "icon" */
									if(e && (e->type == JSON_INTEGER))
										mctx->items[i].icon = e->u.integer;
									break;
								case 0x106daa27: /* "title" */
									if(e && (e->type == JSON_STRING))
										mctx->items[i].title = strdup(e->u.string.ptr);
									break;
								case 0xd3639944: /* "command" */
									if(e && (e->type == JSON_STRING))
										mctx->items[i].command = strdup(e->u.string.ptr);
									break;
								default:
									break;
								}
							}
						}
					}
				}
				else
				{
					free(mctx);
					mctx = NULL;
				}
			}
		}
		json_free(v);
	}
	return mctx;
}

static int do_menu(int argc, char ** argv)
{
	struct menu_ctx_t * mctx = NULL;
	const char * fb = (argc >= 3) ? argv[2] : NULL;
	const char * input = (argc >= 4) ? argv[3] : NULL;

	if(argc >= 2)
	{
		char fpath[VFS_MAX_PATH];
		if(shell_realpath(argv[1], fpath) >= 0)
		{
			struct vfs_stat_t st;
			if((vfs_stat(fpath, &st) >= 0) && S_ISREG(st.st_mode) && (st.st_size > 0))
			{
				int fd, len = 0;
				char * json = malloc(st.st_size + 1);
				if(json)
				{
					if((fd = vfs_open(fpath, O_RDONLY, 0)) >= 0)
					{
						for(;;)
						{
							int n = vfs_read(fd, (void *)(json + len), SZ_64K);
							if(n <= 0)
								break;
							len += n;
						}
						vfs_close(fd);
						mctx = menu_ctx_alloc(json, len, NULL);
					}
					free(json);
				}
			}
		}
	}
	if(!mctx)
		mctx = menu_ctx_alloc(menu_json, sizeof(menu_json), NULL);
	if(mctx)
		task_create(NULL, "menu", fb, input, menu_task, mctx, 0, 0);
	return 0;
}

static struct command_t cmd_menu = {
	.name	= "menu",
	.desc	= "menu application based on json",
	.usage	= usage,
	.exec	= do_menu,
};

static __init void menu_cmd_init(void)
{
	register_command(&cmd_menu);
}

static __exit void menu_cmd_exit(void)
{
	unregister_command(&cmd_menu);
}

command_initcall(menu_cmd_init);
command_exitcall(menu_cmd_exit);
