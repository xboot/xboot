/*
 * driver/led/ledtrigger-general.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

struct ledtrigger_general_pdata_t {
	struct timer_t timer;
	struct led_t * led;
	int activity;
	int last_activity;
};

static int ledtrigger_general_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)(data);
	struct ledtrigger_general_pdata_t * pdat = (struct ledtrigger_general_pdata_t *)trigger->priv;

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

static void ledtrigger_general_activity(struct ledtrigger_t * trigger)
{
	struct ledtrigger_general_pdata_t * pdat = (struct ledtrigger_general_pdata_t *)trigger->priv;

	pdat->activity++;
	timer_start_now(&pdat->timer, ms_to_ktime(20));
}

static struct device_t * ledtrigger_general_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledtrigger_general_pdata_t * pdat;
	struct ledtrigger_t * ledtrigger;
	struct device_t * dev;
	struct led_t * led;

	led = search_led(dt_read_string(n, "led-name", NULL));
	if(!led)
		return NULL;

	pdat = malloc(sizeof(struct ledtrigger_general_pdata_t));
	if(!pdat)
		return NULL;

	ledtrigger = malloc(sizeof(struct ledtrigger_t));
	if(!ledtrigger)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, ledtrigger_general_timer_function, ledtrigger);
	pdat->led = led;
	pdat->activity = 0;
	pdat->last_activity = 0;

	ledtrigger->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	ledtrigger->activity = ledtrigger_general_activity;
	ledtrigger->priv = pdat;

	if(!register_ledtrigger(&dev, ledtrigger))
	{
		timer_cancel(&pdat->timer);

		free_device_name(ledtrigger->name);
		free(ledtrigger->priv);
		free(ledtrigger);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ledtrigger_general_remove(struct device_t * dev)
{
	struct ledtrigger_t * ledtrigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_general_pdata_t * pdat = (struct ledtrigger_general_pdata_t *)ledtrigger->priv;

	if(ledtrigger && unregister_ledtrigger(ledtrigger))
	{
		timer_cancel(&pdat->timer);

		free_device_name(ledtrigger->name);
		free(ledtrigger->priv);
		free(ledtrigger);
	}
}

static void ledtrigger_general_suspend(struct device_t * dev)
{
	struct ledtrigger_t * ledtrigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_general_pdata_t * pdat = (struct ledtrigger_general_pdata_t *)ledtrigger->priv;

	timer_cancel(&pdat->timer);
}

static void ledtrigger_general_resume(struct device_t * dev)
{
	struct ledtrigger_t * ledtrigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_general_pdata_t * pdat = (struct ledtrigger_general_pdata_t *)ledtrigger->priv;

	timer_cancel(&pdat->timer);
}

static struct driver_t ledtrigger_general = {
	.name		= "ledtrigger-general",
	.probe		= ledtrigger_general_probe,
	.remove		= ledtrigger_general_remove,
	.suspend	= ledtrigger_general_suspend,
	.resume		= ledtrigger_general_resume,
};

static __init void ledtrigger_general_driver_init(void)
{
	register_driver(&ledtrigger_general);
}

static __exit void ledtrigger_general_driver_exit(void)
{
	unregister_driver(&ledtrigger_general);
}

driver_initcall(ledtrigger_general_driver_init);
driver_exitcall(ledtrigger_general_driver_exit);
