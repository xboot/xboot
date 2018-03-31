/*
 * driver/servo/servo-pwm.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <pwm/pwm.h>
#include <servo/servo.h>

struct servo_pwm_pdata_t {
	struct pwm_t * pwm;
	int period;
	int polarity;
	int from;
	int to;
	int range;
	int angle;
};

static void servo_pwm_set_angle(struct servo_pwm_pdata_t * pdat, int angle)
{
	int r = pdat->range / 2;
	int duty;

	if(angle < -r)
		angle = -r;
	else if(angle > r)
		angle = r;
	duty = pdat->from + (pdat->to - pdat->from) * (angle + r) / pdat->range;
	pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
	pwm_enable(pdat->pwm);
}

static void servo_pwm_enable(struct servo_t * m)
{
	struct servo_pwm_pdata_t * pdat = (struct servo_pwm_pdata_t *)m->priv;
	pwm_enable(pdat->pwm);
}

static void servo_pwm_disable(struct servo_t * m)
{
	struct servo_pwm_pdata_t * pdat = (struct servo_pwm_pdata_t *)m->priv;
	pwm_disable(pdat->pwm);
}

static void servo_pwm_set(struct servo_t * m, int angle)
{
	struct servo_pwm_pdata_t * pdat = (struct servo_pwm_pdata_t *)m->priv;

	if(pdat->angle != angle)
	{
		servo_pwm_set_angle(pdat, angle);
		pdat->angle = angle;
	}
}

static struct device_t * servo_pwm_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct servo_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	struct servo_t * m;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct servo_pwm_pdata_t));
	if(!pdat)
		return NULL;

	m = malloc(sizeof(struct servo_t));
	if(!m)
	{
		free(pdat);
		return NULL;
	}

	pdat->pwm = pwm;
	pdat->period = dt_read_int(n, "pwm-period-ns", 20000 * 1000);
	pdat->polarity = dt_read_bool(n, "pwm-polarity", 0);
	pdat->from = dt_read_int(n, "pwm-duty-ns-from", 500 * 1000);
	pdat->to = dt_read_int(n, "pwm-duty-ns-to", 2500 * 1000);
	pdat->range = dt_read_int(n, "rotation-angle-range", 180);
	pdat->angle = -360;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = servo_pwm_enable;
	m->disable = servo_pwm_disable;
	m->set = servo_pwm_set;
	m->priv = pdat;
	servo_pwm_set(m, dt_read_int(n, "default-angle", 0));

	if(!register_servo(&dev, m))
	{
		free_device_name(m->name);
		free(m->priv);
		free(m);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void servo_pwm_remove(struct device_t * dev)
{
	struct servo_t * m = (struct servo_t *)dev->priv;

	if(m && unregister_servo(m))
	{
		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void servo_pwm_suspend(struct device_t * dev)
{
}

static void servo_pwm_resume(struct device_t * dev)
{
}

static struct driver_t servo_pwm = {
	.name		= "servo-pwm",
	.probe		= servo_pwm_probe,
	.remove		= servo_pwm_remove,
	.suspend	= servo_pwm_suspend,
	.resume		= servo_pwm_resume,
};

static __init void servo_pwm_driver_init(void)
{
	register_driver(&servo_pwm);
}

static __exit void servo_pwm_driver_exit(void)
{
	unregister_driver(&servo_pwm);
}

driver_initcall(servo_pwm_driver_init);
driver_exitcall(servo_pwm_driver_exit);
