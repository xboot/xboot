/*
 * driver/led/ledtrigger-heartbeat.c
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

struct ledtrigger_heartbeat_pdata_t {
	struct timer_t timer;
	struct led_t * led;
	int period;
	int phase;
};

static int ledtrigger_heartbeat_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)(data);
	struct ledtrigger_heartbeat_pdata_t * pdat = (struct ledtrigger_heartbeat_pdata_t *)trigger->priv;
	int brightness;
	int delay;

	/*
	 * Acts like an actual heart beat -- thump-thump-pause ...
	 */
	switch(pdat->phase)
	{
	case 0:
		delay = 70;
		pdat->phase++;
		brightness = CONFIG_MAX_BRIGHTNESS;
		break;

	case 1:
		delay = pdat->period / 4 - 70;
		pdat->phase++;
		brightness = 0;
		break;

	case 2:
		delay = 70;
		pdat->phase++;
		brightness = CONFIG_MAX_BRIGHTNESS;
		break;

	default:
		delay = pdat->period - pdat->period / 4 - 70;
		pdat->phase = 0;
		brightness = 0;
		break;
	}

	led_set_brightness(pdat->led, brightness);
	timer_forward_now(timer, ms_to_ktime(delay));
	return 1;
}

static void ledtrigger_heartbeat_activity(struct ledtrigger_t * trigger)
{
}

static struct device_t * ledtrigger_heartbeat_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledtrigger_heartbeat_pdata_t * pdat;
	struct ledtrigger_t * trigger;
	struct device_t * dev;
	struct led_t * led;

	led = search_led(dt_read_string(n, "led-name", NULL));
	if(!led)
		return NULL;

	pdat = malloc(sizeof(struct ledtrigger_heartbeat_pdata_t));
	if(!pdat)
		return NULL;

	trigger = malloc(sizeof(struct ledtrigger_t));
	if(!trigger)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, ledtrigger_heartbeat_timer_function, trigger);
	pdat->led = led;
	pdat->period = dt_read_int(n, "period-ms", 1260);
	pdat->phase = 0;

	trigger->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	trigger->activity = ledtrigger_heartbeat_activity;
	trigger->priv = pdat;

	timer_start_now(&pdat->timer, ms_to_ktime(50));

	if(!register_ledtrigger(&dev, trigger))
	{
		timer_cancel(&pdat->timer);

		free_device_name(trigger->name);
		free(trigger->priv);
		free(trigger);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ledtrigger_heartbeat_remove(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_heartbeat_pdata_t * pdat = (struct ledtrigger_heartbeat_pdata_t *)trigger->priv;

	if(trigger && unregister_ledtrigger(trigger))
	{
		timer_cancel(&pdat->timer);

		free_device_name(trigger->name);
		free(trigger->priv);
		free(trigger);
	}
}

static void ledtrigger_heartbeat_suspend(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_heartbeat_pdata_t * pdat = (struct ledtrigger_heartbeat_pdata_t *)trigger->priv;

	timer_cancel(&pdat->timer);
}

static void ledtrigger_heartbeat_resume(struct device_t * dev)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)dev->priv;
	struct ledtrigger_heartbeat_pdata_t * pdat = (struct ledtrigger_heartbeat_pdata_t *)trigger->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(50));
}

static struct driver_t ledtrigger_heartbeat = {
	.name		= "ledtrigger-heartbeat",
	.probe		= ledtrigger_heartbeat_probe,
	.remove		= ledtrigger_heartbeat_remove,
	.suspend	= ledtrigger_heartbeat_suspend,
	.resume		= ledtrigger_heartbeat_resume,
};

static __init void ledtrigger_heartbeat_driver_init(void)
{
	register_driver(&ledtrigger_heartbeat);
}

static __exit void ledtrigger_heartbeat_driver_exit(void)
{
	unregister_driver(&ledtrigger_heartbeat);
}

driver_initcall(ledtrigger_heartbeat_driver_init);
driver_exitcall(ledtrigger_heartbeat_driver_exit);
