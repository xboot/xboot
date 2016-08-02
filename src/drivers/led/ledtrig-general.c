/*
 * driver/led/ledtrig-general.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <led/ledtrig.h>

struct ledtrig_general_pdata_t {
	struct timer_t timer;
	struct led_t * led;
	int activity;
	int last_activity;
};

static int ledtrig_general_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)(data);
	struct ledtrig_general_pdata_t * pdat = (struct ledtrig_general_pdata_t *)trigger->priv;

	if(pdat->last_activity != pdat->activity)
	{
		pdat->last_activity = pdat->activity;
		led_set_brightness(pdat->led, CONFIG_MAX_BRIGHTNESS);
		timer_forward_now(timer, ms_to_ktime(20));
		return 1;
	}
	else
	{
		led_set_brightness(pdat->led, 0);
		return 0;
	}
}

static void ledtrig_general_activity(struct ledtrig_t * trigger)
{
	struct ledtrig_general_pdata_t * pdat = (struct ledtrig_general_pdata_t *)trigger->priv;

	pdat->activity++;
	timer_start_now(&pdat->timer, ms_to_ktime(20));
}

static struct device_t * ledtrig_general_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledtrig_general_pdata_t * pdat;
	struct ledtrig_t * ledtrig;
	struct device_t * dev;
	struct led_t * led;

	led = search_led(dt_read_string(n, "led", NULL));
	if(!led)
		return NULL;

	pdat = malloc(sizeof(struct ledtrig_general_pdata_t));
	if(!pdat)
		return NULL;

	ledtrig = malloc(sizeof(struct ledtrig_t));
	if(!ledtrig)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, ledtrig_general_timer_function, ledtrig);
	pdat->led = led;
	pdat->activity = 0;
	pdat->last_activity = 0;

	ledtrig->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	ledtrig->activity = ledtrig_general_activity;
	ledtrig->priv = pdat;

	if(!register_ledtrig(&dev, ledtrig))
	{
		timer_cancel(&pdat->timer);

		free_device_name(ledtrig->name);
		free(ledtrig->priv);
		free(ledtrig);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ledtrig_general_remove(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_general_pdata_t * pdat = (struct ledtrig_general_pdata_t *)ledtrig->priv;

	if(ledtrig && unregister_ledtrig(ledtrig))
	{
		timer_cancel(&pdat->timer);

		free_device_name(ledtrig->name);
		free(ledtrig->priv);
		free(ledtrig);
	}
}

static void ledtrig_general_suspend(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_general_pdata_t * pdat = (struct ledtrig_general_pdata_t *)ledtrig->priv;

	timer_cancel(&pdat->timer);
}

static void ledtrig_general_resume(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_general_pdata_t * pdat = (struct ledtrig_general_pdata_t *)ledtrig->priv;

	timer_cancel(&pdat->timer);
}

struct driver_t ledtrig_general = {
	.name		= "ledtrig-general",
	.probe		= ledtrig_general_probe,
	.remove		= ledtrig_general_remove,
	.suspend	= ledtrig_general_suspend,
	.resume		= ledtrig_general_resume,
};

static __init void ledtrig_general_driver_init(void)
{
	register_driver(&ledtrig_general);
}

static __exit void ledtrig_general_driver_exit(void)
{
	unregister_driver(&ledtrig_general);
}

driver_initcall(ledtrig_general_driver_init);
driver_exitcall(ledtrig_general_driver_exit);
