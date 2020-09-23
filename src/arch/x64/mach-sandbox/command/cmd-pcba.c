/*
 * cmd-pcba.c
 */

#include <xboot.h>
#include <camera/camera.h>
#include <command/command.h>
#include <sandbox.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    pcba [framebuffer] [input]\r\n");
}

static const char zh_CN[] = X({
	"Fail": "失败",
	"Pass": "成功",
	"Unknown": "未知",

	"RED": "红色",
	"GREEN": "绿色",
	"BLUE": "蓝色",
	"WHITE": "白色",
	"BLACK": "黑色",

	"LCD": "液晶屏",
	"Backlight": "背光",
	"Touchscreen": "触摸屏",
	"Camera": "摄像头",
});

static void xui_badge_result(struct xui_context_t * ctx, int result)
{
	switch(result)
	{
	case 0:
		xui_badge_ex(ctx, T("Fail"), XUI_BADGE_DANGER | XUI_BADGE_ROUNDED);
		break;
	case 1:
		xui_badge_ex(ctx, T("Pass"), XUI_BADGE_SUCCESS | XUI_BADGE_ROUNDED);
		break;
	default:
		xui_badge_ex(ctx, T("Unknown"), XUI_BADGE_INFO | XUI_BADGE_ROUNDED);
		break;
	}
}

static void pcba_window(struct xui_context_t * ctx)
{
	if(xui_begin_window_ex(ctx, "Test Window", NULL, 0))//XUI_WINDOW_FULLSCREEN))
	{
		static enum {
			PCBA_ITEM_LCD			= 0,
			PCBA_ITEM_BACKLIGHT		= 1,
			PCBA_ITEM_TOUCHSCREEN	= 2,
			PCBA_ITEM_CAMERA		= 3,
			PCBA_ITEM_MAX			= 4,
		} item = PCBA_ITEM_LCD;

		static int result[] = {
			[PCBA_ITEM_LCD]			= -1,
			[PCBA_ITEM_BACKLIGHT]	= -1,
			[PCBA_ITEM_TOUCHSCREEN]	= -1,
			[PCBA_ITEM_CAMERA]		= -1,
		};

		xui_layout_row(ctx, 3, (int[]){ xui_get_container(ctx)->region.w * 0.3, 10, -1 }, -1);
		xui_begin_panel_ex(ctx, "!items", XUI_PANEL_TRANSPARENT);
		{
			xui_layout_row(ctx, 2, (int[]){ -80, -1 }, 40);
			if(xui_tabbar(ctx, 60683, T("LCD"), item == PCBA_ITEM_LCD))
				item = PCBA_ITEM_LCD;
			xui_badge_result(ctx, result[PCBA_ITEM_LCD]);

			xui_layout_row(ctx, 2, (int[]){ -80, -1 }, 40);
			if(xui_tabbar(ctx, 0xe9bb, T("Backlight"), item == PCBA_ITEM_BACKLIGHT))
				item = PCBA_ITEM_BACKLIGHT;
			xui_badge_result(ctx, result[PCBA_ITEM_BACKLIGHT]);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 10);
			xui_split_ex(ctx, XUI_SPLIT_HORIZONTAL | XUI_SPLIT_PRIMARY);

			xui_layout_row(ctx, 2, (int[]){ -80, -1 }, 40);
			if(xui_tabbar(ctx, 0xe9bb, T("Touchscreen"), item == PCBA_ITEM_TOUCHSCREEN))
				item = PCBA_ITEM_TOUCHSCREEN;
			xui_badge_result(ctx, result[PCBA_ITEM_TOUCHSCREEN]);

			xui_layout_row(ctx, 2, (int[]){ -80, -1 }, 40);
			if(xui_tabbar(ctx, 0xe9bb, T("Camera"), item == PCBA_ITEM_CAMERA))
				item = PCBA_ITEM_CAMERA;
			xui_badge_result(ctx, result[PCBA_ITEM_CAMERA]);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			xui_split(ctx);
			if(xui_button(ctx, "chinese"))
				xui_load_lang(ctx, zh_CN, sizeof(zh_CN));
			if(xui_button(ctx, "default"))
				xui_load_lang(ctx, NULL, 0);
		}
		xui_end_panel(ctx);

		xui_split_ex(ctx, XUI_SPLIT_VERTICAL | XUI_SPLIT_SUCCESS);

		xui_layout_begin_column(ctx);
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
			xui_begin_panel_ex(ctx, "!item", XUI_PANEL_TRANSPARENT);
			{
				xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
				switch(item)
				{
				case PCBA_ITEM_LCD:
					{
						static int index = 0;
						unsigned int id = xui_get_id(ctx, &index, sizeof(index));
						struct region_t * r = xui_layout_next(ctx);
						xui_control_update(ctx, id, r, 0);
						if((ctx->active == id) && (ctx->mouse.up & XUI_MOUSE_LEFT))
							index = (index + 1) % 5;
						switch(index)
						{
						case 0:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 12, 0, &(struct color_t){255, 0, 0, 255});
							xui_control_draw_text(ctx, T("RED"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 1:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 12, 0, &(struct color_t){0, 255, 0, 255});
							xui_control_draw_text(ctx, T("GREEN"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 2:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 12, 0, &(struct color_t){0, 0, 255, 255});
							xui_control_draw_text(ctx, T("BLUE"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 3:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 12, 0, &(struct color_t){255, 255, 255, 255});
							xui_control_draw_text(ctx, T("WHITE"), r, &(struct color_t){0, 0, 0, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 4:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 12, 0, &(struct color_t){0, 0, 0, 255});
							xui_control_draw_text(ctx, T("BLACK"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						default:
							break;
						}
					}
					break;
				case PCBA_ITEM_BACKLIGHT:
					{
						xui_begin_panel(ctx, "!panel");
						{
							xui_layout_set_next(ctx, &(struct region_t){0, (xui_get_container(ctx)->region.h - 40) / 2, xui_get_container(ctx)->region.w - ctx->style.layout.padding * 2, 40}, 1);
							static int brightness = -1;
							if(brightness < 0)
								brightness = window_get_backlight(ctx->w);
							if(xui_slider_int(ctx, &brightness, 0, CONFIG_MAX_BRIGHTNESS, 1))
								window_set_backlight(ctx->w, brightness);
						}
						xui_end_panel(ctx);
					}
					break;
				case PCBA_ITEM_TOUCHSCREEN:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!panel");
						{
							xui_layout_row(ctx, 1, (int[]) { -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, "Start", XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							sandbox_shell("./test.sh 2>&1", log, sizeof(log), 1);
						}
					}
					break;
				case PCBA_ITEM_CAMERA:
					{
						static struct camera_t * c = NULL;
						static struct surface_t * s = NULL;
						struct video_frame_t frame;
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!panel");
						{
							xui_layout_row(ctx, 1, (int[]) { -1 }, -1);
							if(c)
							{
								if(camera_capture(c, &frame, 0))
									video_frame_to_argb(&frame, s->pixels);
								xui_image_ex(ctx, s, 0, XUI_IMAGE_CONTAIN | XUI_IMAGE_REFRESH);
							}
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, "Start", XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							if(!c)
							{
								c = search_first_camera();
								if(!camera_start(c, VIDEO_FORMAT_MJPG, 320, 240))
									c = NULL;
								if(!camera_capture(c, &frame, 3000))
								{
									camera_stop(c);
									c = NULL;
								}
								s = surface_alloc(frame.width, frame.height, NULL);
							}
						}
					}
					break;
				default:
					break;
				}
			}
			xui_end_panel(ctx);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 10);
			xui_split_ex(ctx, XUI_SPLIT_HORIZONTAL | XUI_SPLIT_SUCCESS);

			xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
			xui_begin_panel_ex(ctx, "!result", XUI_PANEL_TRANSPARENT);
			{
				xui_layout_row(ctx, 2, (int[]){ xui_get_container(ctx)->region.w * 0.5, -1 }, -1);
				if(xui_button_ex(ctx, 60133, T("Pass"), XUI_BUTTON_SUCCESS | XUI_BUTTON_ROUNDED | XUI_OPT_TEXT_LEFT))
				{
					result[item] = 1;
					if(item + 1 < PCBA_ITEM_MAX)
						item++;
				}
				if(xui_button_ex(ctx, 60130, T("Fail"), XUI_BUTTON_DANGER | XUI_BUTTON_ROUNDED | XUI_OPT_TEXT_LEFT))
				{
					result[item] = 0;
					if(item + 1 < PCBA_ITEM_MAX)
						item++;
				}
			}
			xui_end_panel(ctx);
		}
		xui_layout_end_column(ctx);
		xui_end_window(ctx);
	}
}

static void pcba(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	pcba_window(ctx);
	xui_end(ctx);
}

static void pcba_task(struct task_t * task, void * data)
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
			xui_loop(ctx, pcba);
			xui_context_free(ctx);
		}
		task_data_free(td);
	}
}

static int do_pcba(int argc, char ** argv)
{
	const char * fb = (argc >= 2) ? argv[1] : NULL;
	const char * input = (argc >= 3) ? argv[2] : NULL;
	task_resume(task_create(NULL, "pcba", pcba_task, task_data_alloc(fb, input, NULL), 0, 0));
	return 0;
}

static struct command_t cmd_pcba = {
	.name	= "pcba",
	.desc	= "pcba test tool using xui",
	.usage	= usage,
	.exec	= do_pcba,
};

static __init void pcba_cmd_init(void)
{
	register_command(&cmd_pcba);
}

static __exit void pcba_cmd_exit(void)
{
	unregister_command(&cmd_pcba);
}

command_initcall(pcba_cmd_init);
command_exitcall(pcba_cmd_exit);
