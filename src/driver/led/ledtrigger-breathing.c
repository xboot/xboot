/*
 * driver/led/ledtrigger-breathing.c
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
#include <led/ledtrigger.h>

struct ledtrigger_breathing_pdata_t {
	struct timer_t timer;
	struct led_t * led;
	int interval;
	int period;
	int phase;
};

static int ledtrigger_breathing_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)(data);
	struct ledtrigger_breathing_pdata_t * pdat = (struct ledtrigger_breathing_pdata_t *)trigger->priv;
	int brightness;

	pdat->phase += pdat->interval;
	if(pdat->phase < (pdat->period / 2))
	{
		brightness = 1000 * pdat->phase / (pdat->period / 2);
	}
	else if(pdat->phase < pdat->period)
	{
		brightness = 1000 * (pdat->period - pdat->phase) / (pdat->period / 2);
	}
	else
	{
		brightness = 0;
		pdat->phase = 0;
	}

	led_set_brightness(pdat->led, brightness);
	timer_forward(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static void ledtrigger_breathing_activity(struct ledtrigger_t * trigger)
{
}

static struct device_t * ledtrigger_breathing_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledtrigger_breathing_pdata_t * pdat;
	struct ledtrigger_t * trigger;
	struct device_t * dev;
	struct led_t * led;

	led = search_led(dt_read_string(n, "led-name", NULL));
	if(!led)
		return NULL;

	pdat = malloc(sizeof(struct ledtrigger_breathing_pdata_t));
	if(!pdat)
		return NULL;

	trigger = malloc(sizeof(struct ledtrigger_t));
	if(!trigger)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, ledtrigger_breathing_timer_function, trigger);
	pdat->led = led;
	pdat->interval = dt_read_int(n, "interval-ms", 20);
	pdat->period = dt_read_int(n, "period-ms", 3000);
	pdat->phase = 0;

	trigger->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	trigger->activity = ledtrigger_breathing_activity;
	trigger->priv = pdat;

	timer_start(&pdat->timer, ms_to_ktime(pdat->interval));

	if(!(dev = register_ledtrigger(trigger, drv)))
	{
		timer_cancel(&pdat->timer);
		free_device_name(trigger->name);
		free(trigger->priv);
		free(trigger);
		return NULL;
	}
	return dev;
}

static void ledtrigger_breathing_remove(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_breathing_pdata_t * pdat = (struct ledtrigger_breathing_pdata_t *)trigger->priv;

	if(trigger)
	{
		unregister_ledtrigger(trigger);
		timer_cancel(&pdat->timer);
		free_device_name(trigger->name);
		free(trigger->priv);
		free(trigger);
	}
}

static void ledtrigger_breathing_suspend(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_breathing_pdata_t * pdat = (struct ledtrigger_breathing_pdata_t *)trigger->priv;

	timer_cancel(&pdat->timer);
}

static void ledtrigger_breathing_resume(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_breathing_pdata_t * pdat = (struct ledtrigger_breathing_pdata_t *)trigger->priv;

	timer_start(&pdat->timer, ms_to_ktime(pdat->interval));
}

static struct driver_t ledtrigger_breathing = {
	.name		= "ledtrigger-breathing",
	.probe		= ledtrigger_breathing_probe,
	.remove		= ledtrigger_breathing_remove,
	.suspend	= ledtrigger_breathing_suspend,
	.resume		= ledtrigger_breathing_resume,
};

static __init void ledtrigger_breathing_driver_init(void)
{
	register_driver(&ledtrigger_breathing);
}

static __exit void ledtrigger_breathing_driver_exit(void)
{
	unregister_driver(&ledtrigger_breathing);
}

driver_initcall(ledtrigger_breathing_driver_init);
driver_exitcall(ledtrigger_breathing_driver_exit);
