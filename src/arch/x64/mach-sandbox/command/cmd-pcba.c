/*
 * cmd-pcba.c
 */

#include <xboot.h>
#include <led/led.h>
#include <buzzer/buzzer.h>
#include <vibrator/vibrator.h>
#include <gnss/gnss.h>
#include <camera/camera.h>
#include <command/command.h>
#include <sandbox.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    pcba [framebuffer] [input]\r\n");
}

static const char zh_CN[] = X({
	"Information": "信息",
	"LCD": "液晶屏",
	"Touchscreen": "触摸屏",
	"Backlight": "背光",
	"Led": "发光二极管",
	"Buzzer": "蜂鸣器",
	"Vibrator": "振动马达",
	"Gnns": "全球卫星导航",
	"Camera": "摄像头",
	"Ethernet": "以太网",
	"Wifi": "无线网络",
	"Bluetooth": "蓝牙",
	"Udisk": "优盘",
	"Tfcard": "TF卡",

	"Sleep": "休眠",
	"Reboot": "重启",
	"Shutdown": "关机",
	"Submit": "提交",

	"Play": "播放",
	"Start": "开始",
	"Open": "打开",
	"Close": "关闭",
	"Pass": "通过",
	"Fail": "失败",

	"Unique ID": "唯一序列号",
	"Screen resolution": "屏幕分辨率",
	"Frame": "帧数",
	"Fps": "帧率",

	"RED": "红色",
	"GREEN": "绿色",
	"BLUE": "蓝色",
	"WHITE": "白色",
	"BLACK": "黑色",
});

static struct surface_t * qrc = NULL;

static void xui_badge_result(struct xui_context_t * ctx, int result)
{
	switch(result)
	{
	case 0:
		xui_icon(ctx, 0xf057, &(struct color_t){0xff, 0x5b, 0x5b, 0xff});
		break;
	case 1:
		xui_icon(ctx, 0xf058, &(struct color_t){0x10, 0xc4, 0x69, 0xff});
		break;
	default:
		xui_icon(ctx, 0xf056, &(struct color_t){0xcf, 0xcf, 0xcf, 0xff});
		break;
	}
}

static void pcba_window(struct xui_context_t * ctx)
{
	if(xui_begin_window_ex(ctx, "Test Window", NULL, XUI_WINDOW_FULLSCREEN))
	{
		static char qrcode[128] = { 0 };

		static enum {
			PCBA_ITEM_INFORMATION	= 0,
			PCBA_ITEM_LCD,
			PCBA_ITEM_TOUCHSCREEN,
			PCBA_ITEM_BACKLIGHT,
			PCBA_ITEM_LED,
			PCBA_ITEM_BUZZER,
			PCBA_ITEM_VIBRATOR,
			PCBA_ITEM_GNNS,
			PCBA_ITEM_CAMERA,
			PCBA_ITEM_ETHERNET,
			PCBA_ITEM_WIFI,
			PCBA_ITEM_BLUETOOTH,
			PCBA_ITEM_UDISK,
			PCBA_ITEM_TFCARD,
			PCBA_ITEM_MAX,
		} item = PCBA_ITEM_INFORMATION;

		static int result[] = {
			[PCBA_ITEM_INFORMATION]	= -1,
			[PCBA_ITEM_LCD]			= -1,
			[PCBA_ITEM_TOUCHSCREEN]	= -1,
			[PCBA_ITEM_BACKLIGHT]	= -1,
			[PCBA_ITEM_LED]			= -1,
			[PCBA_ITEM_BUZZER]		= -1,
			[PCBA_ITEM_VIBRATOR]	= -1,
			[PCBA_ITEM_GNNS]		= -1,
			[PCBA_ITEM_CAMERA]		= -1,
			[PCBA_ITEM_ETHERNET]	= -1,
			[PCBA_ITEM_WIFI]		= -1,
			[PCBA_ITEM_BLUETOOTH]	= -1,
			[PCBA_ITEM_UDISK]		= -1,
			[PCBA_ITEM_TFCARD]		= -1,
		};

		xui_layout_row(ctx, 3, (int[]){ xui_get_container(ctx)->region.w * 0.3, 10, -1 }, -1);
		xui_begin_panel(ctx, "!items");
		{
			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_textedit(ctx, qrcode, sizeof(qrcode)) & (1 << 0))
			{
			}
			if(xui_button_ex(ctx, 0xf2ed, NULL, XUI_BUTTON_PRIMARY | XUI_BUTTON_OUTLINE))
				qrcode[0] = 0;

			xui_layout_row(ctx, 1, (int[]){ -1 }, 40);
			xui_split(ctx);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 62637, T("Information"), item == PCBA_ITEM_INFORMATION))
				item = PCBA_ITEM_INFORMATION;
			xui_badge_result(ctx, result[PCBA_ITEM_INFORMATION]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 62060, T("LCD"), item == PCBA_ITEM_LCD))
				item = PCBA_ITEM_LCD;
			xui_badge_result(ctx, result[PCBA_ITEM_LCD]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 62042, T("Touchscreen"), item == PCBA_ITEM_TOUCHSCREEN))
				item = PCBA_ITEM_TOUCHSCREEN;
			xui_badge_result(ctx, result[PCBA_ITEM_TOUCHSCREEN]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf042, T("Backlight"), item == PCBA_ITEM_BACKLIGHT))
				item = PCBA_ITEM_BACKLIGHT;
			xui_badge_result(ctx, result[PCBA_ITEM_BACKLIGHT]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf0eb, T("Led"), item == PCBA_ITEM_LED))
				item = PCBA_ITEM_LED;
			xui_badge_result(ctx, result[PCBA_ITEM_LED]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf2ce, T("Buzzer"), item == PCBA_ITEM_BUZZER))
				item = PCBA_ITEM_BUZZER;
			xui_badge_result(ctx, result[PCBA_ITEM_BUZZER]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf2ce, T("Vibrator"), item == PCBA_ITEM_VIBRATOR))
				item = PCBA_ITEM_VIBRATOR;
			xui_badge_result(ctx, result[PCBA_ITEM_VIBRATOR]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf3c5, T("Gnns"), item == PCBA_ITEM_GNNS))
				item = PCBA_ITEM_GNNS;
			xui_badge_result(ctx, result[PCBA_ITEM_GNNS]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf030, T("Camera"), item == PCBA_ITEM_CAMERA))
				item = PCBA_ITEM_CAMERA;
			xui_badge_result(ctx, result[PCBA_ITEM_CAMERA]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf796, T("Ethernet"), item == PCBA_ITEM_ETHERNET))
				item = PCBA_ITEM_ETHERNET;
			xui_badge_result(ctx, result[PCBA_ITEM_ETHERNET]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf1eb, T("Wifi"), item == PCBA_ITEM_WIFI))
				item = PCBA_ITEM_WIFI;
			xui_badge_result(ctx, result[PCBA_ITEM_WIFI]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf7c0, T("Bluetooth"), item == PCBA_ITEM_BLUETOOTH))
				item = PCBA_ITEM_BLUETOOTH;
			xui_badge_result(ctx, result[PCBA_ITEM_BLUETOOTH]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf0a0, T("Udisk"), item == PCBA_ITEM_UDISK))
				item = PCBA_ITEM_UDISK;
			xui_badge_result(ctx, result[PCBA_ITEM_UDISK]);

			xui_layout_row(ctx, 2, (int[]){ -40, -1 }, 40);
			if(xui_tabbar(ctx, 0xf7c2, T("Tfcard"), item == PCBA_ITEM_TFCARD))
				item = PCBA_ITEM_TFCARD;
			xui_badge_result(ctx, result[PCBA_ITEM_TFCARD]);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 40);
			xui_split(ctx);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 40);
			if(xui_button_ex(ctx, 0xf7b6, T("Sleep"), XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_LEFT))
				machine_sleep();
			if(xui_button_ex(ctx, 0xf2ea, T("Reboot"), XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_LEFT))
				machine_reboot();
			if(xui_button_ex(ctx, 0xf011, T("Shutdown"), XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_LEFT))
				machine_shutdown();

			xui_layout_row(ctx, 1, (int[]){ -1 }, 40);
			xui_split(ctx);

			xui_layout_row(ctx, 1, (int[]){ -1 }, 60);
			if(xui_button_ex(ctx, 0, T("Submit"), XUI_BUTTON_INFO | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
			{
			}
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
				case PCBA_ITEM_INFORMATION:
					{
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
							xui_label(ctx, T("Unique ID"));
							xui_label(ctx, xui_format(ctx, ": %s", machine_uniqueid()));
							xui_label(ctx, T("Screen resolution"));
							xui_label(ctx, xui_format(ctx, ": %d x %d", ctx->screen.w, ctx->screen.h));
							xui_label(ctx, T("Frame"));
							xui_label(ctx, xui_format(ctx, ": %d", ctx->frame));
							xui_label(ctx, T("Fps"));
							xui_label(ctx, xui_format(ctx, ": %d", ctx->fps));
							if(qrc)
							{
								xui_layout_row(ctx, 1, (int[]){ -1 }, 260);
								xui_image_ex(ctx, qrc, 0, XUI_IMAGE_CONTAIN);
							}
						}
						xui_end_panel(ctx);
					}
					break;
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
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 8, 0, &(struct color_t){255, 0, 0, 255});
							xui_control_draw_text(ctx, T("RED"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 1:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 8, 0, &(struct color_t){0, 255, 0, 255});
							xui_control_draw_text(ctx, T("GREEN"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 2:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 8, 0, &(struct color_t){0, 0, 255, 255});
							xui_control_draw_text(ctx, T("BLUE"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 3:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 8, 0, &(struct color_t){255, 255, 255, 255});
							xui_control_draw_text(ctx, T("WHITE"), r, &(struct color_t){0, 0, 0, 255}, XUI_OPT_TEXT_CENTER);
							break;
						case 4:
							xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 8, 0, &(struct color_t){0, 0, 0, 255});
							xui_control_draw_text(ctx, T("BLACK"), r, &(struct color_t){255, 255, 255, 255}, XUI_OPT_TEXT_CENTER);
							break;
						default:
							break;
						}
					}
					break;
				case PCBA_ITEM_TOUCHSCREEN:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
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
				case PCBA_ITEM_BACKLIGHT:
					{
						xui_begin_panel(ctx, "!detial");
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
				case PCBA_ITEM_LED:
					{
						xui_begin_panel(ctx, "!detial");
						{
							struct device_t * pos, * n;
							struct led_t * led;
							int state;
							xui_layout_row(ctx, 2, (int[]){ -100, -1 }, 40);
							list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LED], head)
							{
								led = (struct led_t *)(pos->priv);
								if(!led)
									continue;
								xui_push_id(ctx, &led, sizeof(struct led_t *));
								state = led_get_brightness(led) > 0 ? 1 : 0;
								xui_label(ctx, xui_format(ctx, "%s :", pos->name));
								if(xui_toggle_ex(ctx, &state, XUI_TOGGLE_PRIMARY))
								{
									if(state)
										led_set_brightness(led, CONFIG_MAX_BRIGHTNESS);
									else
										led_set_brightness(led, 0);
								}
								xui_pop_id(ctx);
							}
						}
						xui_end_panel(ctx);
					}
					break;
				case PCBA_ITEM_BUZZER:
					{
						xui_begin_panel(ctx, "!detial");
						{
							struct device_t * pos, * n;
							struct buzzer_t * buzzer;
							xui_layout_row(ctx, 2, (int[]){ -100, -1 }, 40);
							list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BUZZER], head)
							{
								buzzer = (struct buzzer_t *)(pos->priv);
								if(!buzzer)
									continue;
								xui_push_id(ctx, &buzzer, sizeof(struct buzzer *));
								xui_label(ctx, xui_format(ctx, "%s :", pos->name));
								if(xui_button(ctx, T("Play")))
								{
									buzzer_play(buzzer, "AuldLangSyne:d=4,o=5,b=100:g,c.6,8c6,c6,e6,d.6,8c6,d6,8e6,"
										"8d6,c.6,8c6,e6,g6,2a.6,a6,g.6,8e6,e6,c6,d.6,8c6,d6,8e6,8d6,c.6,8a,a,g,2c.6");
								}
								xui_pop_id(ctx);
							}
						}
						xui_end_panel(ctx);
					}
					break;
				case PCBA_ITEM_VIBRATOR:
					{
						xui_begin_panel(ctx, "!detial");
						{
							struct device_t * pos, * n;
							struct vibrator_t * vib;
							xui_layout_row(ctx, 2, (int[]){ -100, -1 }, 40);
							list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_VIBRATOR], head)
							{
								vib = (struct vibrator_t *)(pos->priv);
								if(!vib)
									continue;
								xui_push_id(ctx, &vib, sizeof(struct buzzer *));
								xui_label(ctx, xui_format(ctx, "%s :", pos->name));
								if(xui_button(ctx, T("Play")))
									vibrator_play(vib, "xboot");
								xui_pop_id(ctx);
							}
						}
						xui_end_panel(ctx);
					}
					break;
				case PCBA_ITEM_GNNS:
					{
						xui_begin_panel(ctx, "!detial");
						{
							static struct gnss_t * nav = NULL;
							if(!nav)
							{
								nav = search_first_gnss();
								gnss_enable(nav);
							}
							if(nav)
							{
								struct gnss_nmea_t * nmea = gnss_nmea(nav);
								gnss_refresh(nav);
								if(xui_begin_tree(ctx, "Information"))
								{
									xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
									xui_label(ctx, "Signal:");
									switch(nmea->signal)
									{
									case GNSS_SIGNAL_INVALID:
										xui_label(ctx, "Invalid");
										break;
									case GNSS_SIGNAL_FIX:
										xui_label(ctx, "FIX");
										break;
									case GNSS_SIGNAL_DIFFERENTIAL:
										xui_label(ctx, "Differential");
										break;
									case GNSS_SIGNAL_PPS:
										xui_label(ctx, "Pps");
										break;
									case GNSS_SIGNAL_RTK:
										xui_label(ctx, "Rtk");
										break;
									case GNSS_SIGNAL_FLOAT_RTK:
										xui_label(ctx, "Float rtk");
										break;
									case GNSS_SIGNAL_ESTIMATED:
										xui_label(ctx, "Estimated");
										break;
									case GNSS_SIGNAL_MANUAL:
										xui_label(ctx, "Manual");
										break;
									case GNSS_SIGNAL_SIMULATION:
										xui_label(ctx, "Simulation");
										break;
									default:
										xui_label(ctx, "Unknown");
										break;
									}

									xui_label(ctx, "Fix:");
									switch(nmea->fix)
									{
									case GNSS_FIX_NONE:
										xui_label(ctx, "None");
										break;
									case GNSS_FIX_2D:
										xui_label(ctx, "Fix 2D");
										break;
									case GNSS_FIX_3D:
										xui_label(ctx, "Fix 3D");
										break;
									default:
										xui_label(ctx, "Unknown");
										break;
									}

									xui_label(ctx, "Latitude:");
									xui_label(ctx, xui_format(ctx, "%g", nmea->latitude));

									xui_label(ctx, "Longitude:");
									xui_label(ctx, xui_format(ctx, "%g", nmea->longitude));

									xui_label(ctx, "Altitude:");
									xui_label(ctx, xui_format(ctx, "%g m", nmea->altitude));

									xui_label(ctx, "Speed:");
									xui_label(ctx, xui_format(ctx, "%g km/h", nmea->speed));

									xui_label(ctx, "Track:");
									xui_label(ctx, xui_format(ctx, "%g", nmea->track));

									xui_label(ctx, "Magnetic track:");
									xui_label(ctx, xui_format(ctx, "%g", nmea->mtrack));

									xui_label(ctx, "Magnetic variation:");
									xui_label(ctx, xui_format(ctx, "%g", nmea->magvar));

									xui_end_tree(ctx);
								}
								if(xui_begin_tree(ctx, "UTC Time"))
								{
									xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
									xui_label(ctx, "Data:");
									xui_label(ctx, xui_format(ctx, "%d-%d-%d", nmea->utc.year, nmea->utc.month, nmea->utc.day));

									xui_label(ctx, "Time:");
									xui_label(ctx, xui_format(ctx, "%d:%d:%d:%d", nmea->utc.hour, nmea->utc.minute, nmea->utc.second, nmea->utc.millisecond));

									xui_end_tree(ctx);
								}
								if(xui_begin_tree(ctx, "Precision"))
								{
									xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
									xui_label(ctx, "Position Precision:");
									xui_label(ctx, xui_format(ctx, "%f", nmea->precision.pdop));

									xui_label(ctx, "Horizontal Precision:");
									xui_label(ctx, xui_format(ctx, "%f", nmea->precision.hdop));

									xui_label(ctx, "Vertical Precision:");
									xui_label(ctx, xui_format(ctx, "%f", nmea->precision.vdop));

									xui_end_tree(ctx);
								}
								if(xui_begin_tree(ctx, "Satellite"))
								{
									xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
									xui_label(ctx, "Satellites in used:");
									xui_label(ctx, xui_format(ctx, "%d", nmea->used));

									if(xui_begin_tree(ctx, "Gps"))
									{
										xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
										xui_label(ctx, "Satellites in viewed:");
										xui_label(ctx, xui_format(ctx, "%d", nmea->satellite.gps.n));

										xui_layout_row(ctx, 1, (int[] ) { -1 }, 0);
										for(int i = 0; i < ARRAY_SIZE(nmea->satellite.gps.sv); i++)
										{
											xui_progress(ctx, nmea->satellite.gps.sv[i].snr * 100 / 99);
										}
										xui_end_tree(ctx);
									}

									if(xui_begin_tree(ctx, "Beidou"))
									{
										xui_layout_row(ctx, 2, (int[]){ 150, -1 }, 0);
										xui_label(ctx, "Satellites in viewed:");
										xui_label(ctx, xui_format(ctx, "%d", nmea->satellite.beidou.n));

										xui_layout_row(ctx, 1, (int[] ) { -1 }, 0);
										for(int i = 0; i < ARRAY_SIZE(nmea->satellite.beidou.sv); i++)
										{
											xui_progress(ctx, nmea->satellite.beidou.sv[i].snr * 100 / 99);
										}
										xui_end_tree(ctx);
									}
									xui_end_tree(ctx);
								}
							}
							else
							{
								xui_text(ctx, "No Gnns");
							}
						}
						xui_end_panel(ctx);
					}
					break;
				case PCBA_ITEM_CAMERA:
					{
						static int start = 0;
						static struct camera_t * c = NULL;
						static struct surface_t * s = NULL;
						struct video_frame_t frame;
						if(start)
						{
							if(!c)
							{
								c = search_first_camera();
								if(c && camera_start(c, VIDEO_FORMAT_MJPG, 320, 240) && camera_capture(c, &frame, 3000))
									s = surface_alloc(frame.width, frame.height, NULL);
							}
						}
						else
						{
							if(c)
							{
								camera_stop(c);
								surface_free(s);
								c = NULL;
								s = NULL;
							}
						}
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							if(start)
							{
								xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
								if(c && camera_capture(c, &frame, 0))
									video_frame_to_argb(&frame, s->pixels);
								if(s)
									xui_image_ex(ctx, s, 0, XUI_IMAGE_CONTAIN | XUI_IMAGE_REFRESH);
							}
							else
							{
								xui_layout_set_next(ctx, &(struct region_t){0, (xui_get_container(ctx)->region.h - 120) / 2, xui_get_container(ctx)->region.w - ctx->style.layout.padding * 2, 120}, 1);
								xui_spinner(ctx);
							}
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 2, (int[]){ xui_get_container(ctx)->region.w * 0.5, -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Open"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
							start = 1;
						if(xui_button_ex(ctx, 0, T("Close"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
							start = 0;
					}
					break;
				case PCBA_ITEM_ETHERNET:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Start"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							sandbox_shell("ping -c 5 www.baidu.com 2>&1", log, sizeof(log), 1);
						}
					}
					break;
				case PCBA_ITEM_WIFI:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Start"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							sandbox_shell("iwlist wlan0 scanning 2>&1", log, sizeof(log), 1);
						}
					}
					break;
				case PCBA_ITEM_BLUETOOTH:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Start"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
						}
					}
					break;
				case PCBA_ITEM_UDISK:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Start"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							sandbox_shell(X(
								mount -t vfat /dev/sda1 /mnt/ && ls -l /mnt && umount /mnt;
							), log, sizeof(log), 1);
						}
					}
					break;
				case PCBA_ITEM_TFCARD:
					{
						static char log[2048] = { 0 };
						xui_layout_row(ctx, 1, (int[]){ -1 }, -80);
						xui_begin_panel(ctx, "!detial");
						{
							xui_layout_row(ctx, 1, (int[]) { -1 }, -1);
							xui_text(ctx, log);
						}
						xui_end_panel(ctx);
						xui_layout_row(ctx, 1, (int[]){ -1 }, -1);
						if(xui_button_ex(ctx, 0, T("Start"), XUI_BUTTON_PRIMARY | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE | XUI_OPT_TEXT_CENTER))
						{
							sandbox_shell(X(
								mount -t vfat /dev/mmcblk1p1 /mnt/ && ls -l /mnt && umount /mnt;
							), log, sizeof(log), 1);
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
			xui_begin_panel(ctx, "!result");
			{
				xui_layout_row(ctx, 2, (int[]){ xui_get_container(ctx)->region.w * 0.5, -1 }, -1);
				if(xui_button_ex(ctx, 61796, T("Pass"), XUI_BUTTON_SUCCESS | XUI_BUTTON_ROUNDED | XUI_OPT_TEXT_LEFT))
				{
					result[item] = 1;
					if(item + 1 < PCBA_ITEM_MAX)
						item++;
				}
				if(xui_button_ex(ctx, 61797, T("Fail"), XUI_BUTTON_DANGER | XUI_BUTTON_ROUNDED | XUI_OPT_TEXT_LEFT))
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
			//setting_set("language", "zh-CN");
			switch(shash(setting_get("language", NULL)))
			{
			case 0x10d87d65: /* "zh-CN" */
				xui_load_lang(ctx, zh_CN, sizeof(zh_CN));
				break;
			default:
				break;
			}
			qrc = surface_alloc_qrcode(machine_uniqueid(), 16);
			xui_loop(ctx, pcba);
			surface_free(qrc);
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
