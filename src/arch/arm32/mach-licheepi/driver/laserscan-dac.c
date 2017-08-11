/*
 * laserscan-dac.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <dac/dac.h>
#include <gpio/gpio.h>
#include <laserscan/laserscan.h>

struct laserscan_dac_pdata_t {
	struct dac_t * dac;
	int xchannel, ychannel;
	int xlimit, ylimit;
	int pps;
	int gpio;
	int gpiocfg;
	int active_low;

	float a11, a12, a13;
	float a21, a22, a23;
	float a31, a32, a33;
	float x, y;
	u8_t r, g, b, a;

	ktime_t last;
	int interval;
};

static inline void dac_set_position(struct laserscan_dac_pdata_t * pdat, float x, float y)
{
	float tx, ty, w;
	int px, py;

	if(x >= -1 && x <= 1 && y >= -1 && y <= 1)
	{
		w = pdat->a13 * x + pdat->a23 * y + pdat->a33;
		tx = (pdat->a11 * x + pdat->a21 * y + pdat->a31) / w;
		ty = (pdat->a12 * x + pdat->a22 * y + pdat->a32) / w;

		px = tx * pdat->xlimit + pdat->xlimit;
		py = ty * pdat->ylimit + pdat->ylimit;
		dac_write_raw(pdat->dac, pdat->xchannel, px);
		dac_write_raw(pdat->dac, pdat->ychannel, py);

		ktime_t timeout = ktime_add_ns(pdat->last, pdat->interval);
		while(ktime_before(ktime_get(), timeout));
		pdat->last = ktime_get();
	}
}

/*
#define MOVE_TIME_MICROS (15000)
void moveToTimed(struct laserscan_dac_pdata_t * pdat, float x1, float y1, float x2, float y2, int micros)
{
	if (micros > MOVE_TIME_MICROS) {
		int steps = micros / MOVE_TIME_MICROS;
		//int remain = micros % MOVE_TIME_MICROS;
		float dx = (x2 - x1) / steps;
		float dy = (y2 - y1) / steps;
		for (int i = 1; i < steps; i++) {
			x1 += dx;
			y1 += dy;
			dac_set_position(pdat, x1, y1);
		}
		//udelay(remain);
		dac_set_position(pdat, x2, y2);
	}
	else
	{
		dac_set_position(pdat, x2, y2);
	}
}

void moveToSpeedLimit(struct laserscan_dac_pdata_t * pdat, float x1, float y1, float x2, float y2, int distPerS){
	float dx = (x2 - x1);
	float dy = (y2 - y1);
	int micros = 0;
	int sx = dx > 0;
	int sy = dy > 0;

	if(!sx)
		dx = -dx;
	if(!sy)
		dy = -dy;

	if(dx > dy){
		micros = (dx / distPerS) * 1000000.0;
	}else{
		micros = (dy / distPerS) * 1000000.0;
	}
	moveToTimed(pdat, x1, y1, x2, y2, micros);
}

static inline void dac_line(struct laserscan_dac_pdata_t * pdat, int x1, int y1, int x2, int y2)
{
	moveToSpeedLimit(pdat, x1, y1, x2, y2, 60000);
}
*/
static void laserscan_dac_perspective(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	pdat->a13 = x;
	pdat->a23 = y;
}

static void laserscan_dac_translate(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	pdat->a31 = x;
	pdat->a32 = y;
}

static void laserscan_dac_scale(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	pdat->a11 = x;
	pdat->a22 = y;
}

static void laserscan_dac_shear(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	pdat->a21 = x;
	pdat->a12 = y;
}

static void laserscan_dac_set_color(struct laserscan_t * l, u8_t r, u8_t g, u8_t b, u8_t a)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;

	pdat->r = r;
	pdat->g = g;
	pdat->b = b;
	if(pdat->a != a)
	{
		if(a == 0)
			gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
		else
			gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
		pdat->a = a;
	}
}

static void laserscan_dac_get_color(struct laserscan_t * l, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;

	if(r)
		*r = pdat->r;
	if(g)
		*g = pdat->g;
	if(b)
		*b = pdat->b;
	if(a)
		*a = pdat->a;
}

static void laserscan_dac_move_to(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	dac_set_position(pdat, x, y);
	pdat->x = x;
	pdat->y = y;
}

static void laserscan_dac_rel_move_to(struct laserscan_t * l, float dx, float dy)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	laserscan_dac_move_to(l, pdat->x + dx, pdat->y + dy);
}

static void laserscan_dac_line_to(struct laserscan_t * l, float x, float y)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
}

static void laserscan_dac_rel_line_to(struct laserscan_t * l, float dx, float dy)
{
	struct laserscan_dac_pdata_t * pdat = (struct laserscan_dac_pdata_t *)l->priv;
	laserscan_dac_line_to(l, pdat->x + dx, pdat->y + dy);
}

static void laserscan_dac_curve_to(struct laserscan_t * l, float x1, float y1, float x2, float y2, float x3, float y3)
{
}

static void laserscan_dac_rel_curve_to(struct laserscan_t * l, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3)
{
}

static void laserscan_dac_arc(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
}

static void laserscan_dac_arc_negative(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
}

static struct device_t * laserscan_dac_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct laserscan_dac_pdata_t * pdat;
	struct dac_t * dac;
	struct laserscan_t * l;
	struct device_t * dev;

	if(!(dac = search_dac(dt_read_string(n, "dac-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct laserscan_dac_pdata_t));
	if(!pdat)
		return NULL;

	l = malloc(sizeof(struct laserscan_t));
	if(!l)
	{
		free(pdat);
		return NULL;
	}

	pdat->dac = dac;
	pdat->xchannel = dt_read_int(n, "dac-channel-x", 0);
	pdat->ychannel = dt_read_int(n, "dac-channel-y", 1);
	pdat->xlimit = (1 << pdat->dac->resolution) / 2;
	pdat->ylimit = (1 << pdat->dac->resolution) / 2;
	pdat->pps = dt_read_int(n, "points-per-second", 30000);
	pdat->gpio = dt_read_int(n, "gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->active_low = dt_read_bool(n, "active-low", 0);
	pdat->a11 = 1; pdat->a12 = 0; pdat->a13 = 0;
	pdat->a21 = 0; pdat->a22 = 1; pdat->a23 = 0;
	pdat->a31 = 0; pdat->a32 = 0; pdat->a33 = 1;
	pdat->x = 0; pdat->y = 0;
	pdat->r = 0; pdat->g = 0; pdat->b = 0; pdat->a = 0;
	pdat->last = ktime_get();
	pdat->interval = 1000000000ULL / pdat->pps;

	l->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	l->perspective = laserscan_dac_perspective;
	l->translate = laserscan_dac_translate;
	l->scale = laserscan_dac_scale;
	l->shear = laserscan_dac_shear;
	l->set_color = laserscan_dac_set_color;
	l->get_color = laserscan_dac_get_color;
	l->move_to = laserscan_dac_move_to;
	l->rel_move_to = laserscan_dac_rel_move_to;
	l->line_to = laserscan_dac_line_to;
	l->rel_line_to = laserscan_dac_rel_line_to;
	l->curve_to = laserscan_dac_curve_to;
	l->rel_curve_to = laserscan_dac_rel_curve_to;
	l->arc = laserscan_dac_arc;
	l->arc_negative = laserscan_dac_arc_negative;
	l->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);

	if(!register_laserscan(&dev, l))
	{
		free_device_name(l->name);
		free(l->priv);
		free(l);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void laserscan_dac_remove(struct device_t * dev)
{
	struct laserscan_t * l = (struct laserscan_t *)dev->priv;

	if(l && unregister_laserscan(l))
	{
		free_device_name(l->name);
		free(l->priv);
		free(l);
	}
}

static void laserscan_dac_suspend(struct device_t * dev)
{
}

static void laserscan_dac_resume(struct device_t * dev)
{
}

static struct driver_t laserscan_dac = {
	.name		= "laserscan-dac",
	.probe		= laserscan_dac_probe,
	.remove		= laserscan_dac_remove,
	.suspend	= laserscan_dac_suspend,
	.resume		= laserscan_dac_resume,
};

static __init void laserscan_dac_driver_init(void)
{
	register_driver(&laserscan_dac);
}

static __exit void laserscan_dac_driver_exit(void)
{
	unregister_driver(&laserscan_dac);
}

driver_initcall(laserscan_dac_driver_init);
driver_exitcall(laserscan_dac_driver_exit);
