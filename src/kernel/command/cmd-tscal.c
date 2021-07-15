/*
 * kernel/command/cmd-tscal.c
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
#include <shell/ctrlc.h>
#include <input/input.h>
#include <xboot/window.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    tscal <framebuffer> <input>\r\n");
}

struct tscal_t {
	int x[5], xfb[5];
	int y[5], yfb[5];
	int a[7];
	int index;
};

static int perform_calibration(struct tscal_t * cal)
{
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;
	int j;

	n = x = y = x2 = y2 = xy = 0;
	for(j = 0; j < 5; j++)
	{
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

	det = n * (x2 * y2 - xy * xy) + x * (xy * y - x * y2) + y * (x * xy - y * x2);
	if(det < 0.1 && det > -0.1)
		return 0;

	a = (x2 * y2 - xy * xy) / det;
	b = (xy * y - x * y2) / det;
	c = (x * xy - y * x2) / det;
	e = (n * y2 - y * y) / det;
	f = (x * y - n * xy) / det;
	i = (n * x2 - x * x) / det;

	z = zx = zy = 0;
	for(j = 0; j < 5; j++)
	{
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j] * cal->x[j]);
		zy += (float)(cal->xfb[j] * cal->y[j]);
	}
	cal->a[0] = (int)((b * z + e * zx + f * zy) * (scaling));
	cal->a[1] = (int)((c * z + f * zx + i * zy) * (scaling));
	cal->a[2] = (int)((a * z + b * zx + c * zy) * (scaling));

	z = zx = zy = 0;
	for(j = 0; j < 5; j++)
	{
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j] * cal->x[j]);
		zy += (float)(cal->yfb[j] * cal->y[j]);
	}
	cal->a[3] = (int)((b * z + e * zx + f * zy) * (scaling));
	cal->a[4] = (int)((c * z + f * zx + i * zy) * (scaling));
	cal->a[5] = (int)((a * z + b * zx + c * zy) * (scaling));
	cal->a[6] = (int)scaling;

	return 1;
}

static void tscal_draw(struct window_t * w, void * o)
{
	struct tscal_t * cal = (struct tscal_t *)o;
	struct color_t c = { 0xff, 0xff, 0x00, 0xff };
	int x = cal->xfb[cal->index];
	int y = cal->yfb[cal->index];

	surface_shape_line(w->s, NULL, &(struct point_t){x - 15, y}, &(struct point_t){x + 15, y}, 2, &c);
	surface_shape_line(w->s, NULL, &(struct point_t){x, y - 15}, &(struct point_t){x, y + 15}, 2, &c);
	surface_shape_circle(w->s, NULL, x, y, 15, 4, &c);
}

static int do_tscal(int argc, char ** argv)
{
	struct window_t * w;
	struct input_t * dev;
	struct event_t e;
	struct tscal_t cal;
	int buf[7];
	int c[7] = {1, 0, 0, 0, 1, 0, 1};
	int running = 1;

	if(argc != 3)
	{
		usage();
		return -1;
	}
	if(!search_framebuffer(argv[1]))
	{
		printf("The framebuffer device '%s' is not exist\r\n", argv[1]);
		usage();
		return -1;
	}
	if(!(dev = search_input(argv[2])))
	{
		printf("The input device '%s' is not exist\r\n", argv[2]);
		usage();
		return -1;
	}
	if(input_ioctl(dev, "touchscreen-get-calibration", buf) < 0)
	{
		printf("The input device '%s' is not a touchscreen\r\n", argv[2]);
		usage();
		return -1;
	}
	if((w = window_alloc(argv[1], argv[2])))
	{
		cal.xfb[0] = 50;
		cal.yfb[0] = 50;
		cal.xfb[1] = window_get_width(w) - 50;
		cal.yfb[1] = 50;
		cal.xfb[2] = window_get_width(w) - 50;
		cal.yfb[2] = window_get_height(w) - 50;
		cal.xfb[3] = 50;
		cal.yfb[3] = window_get_height(w) - 50;
		cal.xfb[4] = window_get_width(w) / 2;
		cal.yfb[4] = window_get_height(w) / 2;
		cal.index = 0;
		input_ioctl(dev, "touchscreen-set-calibration", c);

		while(running)
		{
			if(window_pump_event(w, &e))
			{
				if(e.device == dev)
				{
					switch(e.type)
					{
					case EVENT_TYPE_TOUCH_BEGIN:
						break;
					case EVENT_TYPE_TOUCH_MOVE:
						break;
					case EVENT_TYPE_TOUCH_END:
						cal.x[cal.index] = e.e.touch_end.x;
						cal.y[cal.index] = e.e.touch_end.y;
						if(++cal.index >= 5)
						{
							if(perform_calibration(&cal))
							{
								input_ioctl(dev, "touchscreen-set-calibration", &cal.a[0]);
								printf("[%d, %d, %d, %d, %d, %d, %d]\r\n", cal.a[0], cal.a[1], cal.a[2], cal.a[3], cal.a[4], cal.a[5], cal.a[6]);
								running = 0;
							}
							else
							{
								cal.index = 0;
							}
						}
						break;
					case EVENT_TYPE_SYSTEM_EXIT:
						running = 0;
						break;
					default:
						break;
					}
				}
			}
			if(window_is_active(w))
				window_present(w, &cal, tscal_draw);
			if(ctrlc())
				running = 0;
		}
		window_free(w);
	}
	return 0;
}

static struct command_t cmd_tscal = {
	.name	= "tscal",
	.desc	= "graphical touchscreen calibration",
	.usage	= usage,
	.exec	= do_tscal,
};

static __init void tscal_cmd_init(void)
{
	register_command(&cmd_tscal);
}

static __exit void tscal_cmd_exit(void)
{
	unregister_command(&cmd_tscal);
}

command_initcall(tscal_cmd_init);
command_exitcall(tscal_cmd_exit);
