/*
 * driver/led/ledtrig-breathing.c
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
#include <led/ledtrig.h>

struct ledtrig_breathing_pdata_t {
	struct timer_t timer;
	struct led_t * led;
	int interval;
	int period;
	int phase;
};

static int ledtrig_breathing_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)(data);
	struct ledtrig_breathing_pdata_t * pdat = (struct ledtrig_breathing_pdata_t *)trigger->priv;
	int brightness;

	pdat->phase += pdat->interval;
	if(pdat->phase < (pdat->period / 2))
	{
		brightness = CONFIG_MAX_BRIGHTNESS * pdat->phase / (pdat->period / 2);
	}
	else if(pdat->phase < pdat->period)
	{
		brightness = CONFIG_MAX_BRIGHTNESS * (pdat->period - pdat->phase) / (pdat->period / 2);
	}
	else
	{
		brightness = 0;
		pdat->phase = 0;
	}

	led_set_brightness(pdat->led, brightness);
	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static void ledtrig_breathing_activity(struct ledtrig_t * trigger)
{
}

static struct device_t * ledtrig_breathing_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledtrig_breathing_pdata_t * pdat;
	struct ledtrig_t * ledtrig;
	struct device_t * dev;
	struct led_t * led;

	led = search_led(dt_read_string(n, "led-name", NULL));
	if(!led)
		return NULL;

	pdat = malloc(sizeof(struct ledtrig_breathing_pdata_t));
	if(!pdat)
		return NULL;

	ledtrig = malloc(sizeof(struct ledtrig_t));
	if(!ledtrig)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, ledtrig_breathing_timer_function, ledtrig);
	pdat->led = led;
	pdat->interval = dt_read_int(n, "interval-ms", 20);
	pdat->period = dt_read_int(n, "period-ms", 3000);
	pdat->phase = 0;

	ledtrig->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	ledtrig->activity = ledtrig_breathing_activity;
	ledtrig->priv = pdat;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));

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

static void ledtrig_breathing_remove(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_breathing_pdata_t * pdat = (struct ledtrig_breathing_pdata_t *)ledtrig->priv;

	if(ledtrig && unregister_ledtrig(ledtrig))
	{
		timer_cancel(&pdat->timer);

		free_device_name(ledtrig->name);
		free(ledtrig->priv);
		free(ledtrig);
	}
}

static void ledtrig_breathing_suspend(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_breathing_pdata_t * pdat = (struct ledtrig_breathing_pdata_t *)ledtrig->priv;

	timer_cancel(&pdat->timer);
}

static void ledtrig_breathing_resume(struct device_t * dev)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)dev->priv;
	struct ledtrig_breathing_pdata_t * pdat = (struct ledtrig_breathing_pdata_t *)ledtrig->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
}

static struct driver_t ledtrig_breathing = {
	.name		= "ledtrig-breathing",
	.probe		= ledtrig_breathing_probe,
	.remove		= ledtrig_breathing_remove,
	.suspend	= ledtrig_breathing_suspend,
	.resume		= ledtrig_breathing_resume,
};

static __init void ledtrig_breathing_driver_init(void)
{
	register_driver(&ledtrig_breathing);
}

static __exit void ledtrig_breathing_driver_exit(void)
{
	unregister_driver(&ledtrig_breathing);
}

driver_initcall(ledtrig_breathing_driver_init);
driver_exitcall(ledtrig_breathing_driver_exit);
