/*
 * command/cmd-gnss.c
 */

#include <xboot.h>
#include <gnss/gnss.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    gnss\r\n");
}

static void gnss_window(struct xui_context_t * ctx)
{
	struct gnss_t * nav = (struct gnss_t *)ctx->priv;
	struct gnss_nmea_t * nmea = gnss_nmea(nav);

	if(xui_begin_window(ctx, "Global Navigation Satellite System", NULL))
	{
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
		xui_end_window(ctx);
	}
}

static void gnss(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	gnss_window(ctx);
	xui_end(ctx);
}

static void gnss_task(struct task_t * task, void * data)
{
	struct gnss_t * nav = (struct gnss_t *)data;
	struct xui_context_t * ctx = xui_context_alloc(NULL, NULL, NULL, nav);
	gnss_enable(nav);
	xui_loop(ctx, gnss);
	gnss_disable(nav);
	xui_context_free(ctx);
}

static int do_gnss(int argc, char ** argv)
{
	struct gnss_t * nav = search_gnss("gnss-uart.0");
	task_resume(task_create(NULL, "gnss", gnss_task, nav, 0, 0));
	return 0;
}

static struct command_t cmd_gnss = {
	.name	= "gnss",
	.desc	= "global navigation satellite system test tool",
	.usage	= usage,
	.exec	= do_gnss,
};

static __init void gnss_cmd_init(void)
{
	register_command(&cmd_gnss);
}

static __exit void gnss_cmd_exit(void)
{
	unregister_command(&cmd_gnss);
}

command_initcall(gnss_cmd_init);
command_exitcall(gnss_cmd_exit);
