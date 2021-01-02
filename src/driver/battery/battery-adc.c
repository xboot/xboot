/*
 * driver/battery-adc.c
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
#include <median.h>
#include <mean.h>
#include <adc/adc.h>
#include <gpio/gpio.h>
#include <battery/battery.h>

struct curve_param_t {
	int voltage;
	int level;
};

struct battery_adc_pdata_t {
	struct adc_t * adc;
	int channel;
	int mult;
	int div;
	struct median_filter_t * mf;
	struct mean_filter_t * nf;
	int design_capacity;
	int design_voltage;
	int gpio;
	int gpiocfg;
	int active_low;
	struct curve_param_t * cc;
	int ncc;
	struct curve_param_t * dc;
	int ndc;
};

static inline int battery_adc_get_voltage(struct battery_adc_pdata_t * pdat)
{
	int voltage = adc_read_voltage(pdat->adc, pdat->channel) * pdat->mult / pdat->div / 1000;
	return mean_update(pdat->nf, median_update(pdat->mf, voltage));
}

static inline int battery_adc_get_level(struct battery_adc_pdata_t * pdat, int voltage, int charging)
{
	struct curve_param_t * c;
	int level = 100;
	int nc, i;

	if(charging)
	{
		c = pdat->cc;
		nc = pdat->ncc;
	}
	else
	{
		c = pdat->dc;
		nc = pdat->ndc;
	}

	if(voltage < c[0].voltage)
	{
		voltage = c[0].voltage;
		level = c[0].level;
	}
	else if(voltage > c[pdat->ncc - 1].voltage)
	{
		voltage = c[pdat->ncc - 1].voltage;
		level = c[pdat->ncc - 1].level;
	}

	for(i = 1; i < nc; i++)
	{
		if(voltage < c[i].voltage)
		{
			level = c[i - 1].level + (c[i].level - c[i - 1].level) * (voltage - c[i - 1].voltage) / (c[i].voltage - c[i - 1].voltage);
			break;
		}
		else
		{
			level = c[i].level;
		}
	}

	if(level < 0)
		level = 0;
	else if(level > 100)
		level = 100;
	return level;
}

static bool_t battery_adc_update(struct battery_t * bat, struct battery_info_t * info)
{
	struct battery_adc_pdata_t * pdat = (struct battery_adc_pdata_t *)bat->priv;
	int voltage = battery_adc_get_voltage(pdat);
	int charging = gpio_get_value(pdat->gpio) ? !pdat->active_low : pdat->active_low;

	if(charging)
	{
		info->supply = POWER_SUPPLAY_AC;
		info->status = BATTERY_STATUS_CHARGING;
	}
	else
	{
		info->supply = POWER_SUPPLAY_BATTERY;
		info->status = BATTERY_STATUS_DISCHARGING;
	}
	info->health = BATTERY_HEALTH_GOOD;
	info->design_capacity = pdat->design_capacity;
	info->design_voltage = pdat->design_voltage;
	info->voltage = voltage;
	info->current = 0;
	info->temperature = 20 * 1000;
	info->cycle = 0;
	info->level = battery_adc_get_level(pdat, voltage, charging);

	return TRUE;
}

static struct device_t * battery_adc_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct battery_adc_pdata_t * pdat;
	struct battery_t * bat;
	struct device_t * dev;
	struct adc_t * adc;
	struct curve_param_t * cc;
	struct curve_param_t * dc;
	struct dtnode_t o;
	int ncc, ndc, i;

	if(!(adc = search_adc(dt_read_string(n, "adc-name", NULL))))
		return NULL;

	if((ncc = dt_read_array_length(n, "charging-curve")) <= 0)
		return NULL;

	if((ndc = dt_read_array_length(n, "discharging-curve")) <= 0)
		return NULL;

	pdat = malloc(sizeof(struct battery_adc_pdata_t));
	if(!pdat)
		return NULL;

	cc = malloc(sizeof(struct curve_param_t) * ncc);
	if(!cc)
	{
		free(pdat);
		return NULL;
	}

	dc = malloc(sizeof(struct curve_param_t) * ndc);
	if(!dc)
	{
		free(cc);
		free(pdat);
		return NULL;
	}

	bat = malloc(sizeof(struct battery_t));
	if(!bat)
	{
		free(cc);
		free(dc);
		free(pdat);
		return NULL;
	}

	for(i = 0; i < ncc; i++)
	{
		dt_read_array_object(n, "charging-curve", i, &o);
		cc[i].voltage = dt_read_int(&o, "voltage", -1);
		cc[i].level = dt_read_int(&o, "level", -1);
	}

	for(i = 0; i < ndc; i++)
	{
		dt_read_array_object(n, "discharging-curve", i, &o);
		dc[i].voltage = dt_read_int(&o, "voltage", -1);
		dc[i].level = dt_read_int(&o, "level", -1);
	}

	pdat->adc = adc;
	pdat->channel = dt_read_int(n, "adc-channel", 0);
	pdat->mult = dt_read_int(n, "adc-voltage-multiply", 1);
	pdat->div = dt_read_int(n, "adc-voltage-divide", 1);
	pdat->mf = median_alloc(dt_read_int(n, "median-filter-length", 7));
	pdat->nf = mean_alloc(dt_read_int(n, "mean-filter-length", 10));
	pdat->design_capacity = dt_read_int(n, "design-capacity", 3000);
	pdat->design_voltage = dt_read_int(n, "design-voltage", 3700);
	pdat->gpio = dt_read_int(n, "charging-detect-gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "charging-detect-gpio-config", -1);
	pdat->active_low = dt_read_bool(n, "charging-detect-active-low", 1);
	pdat->cc = cc;
	pdat->ncc = ncc;
	pdat->dc = dc;
	pdat->ndc = ndc;

	bat->name = alloc_device_name(dt_read_name(n), -1);
	bat->update = battery_adc_update;
	bat->priv = pdat;

	if(gpio_is_valid(pdat->gpio))
	{
		if(pdat->gpiocfg >= 0)
			gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
		gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP : GPIO_PULL_DOWN);
		gpio_set_direction(pdat->gpio, GPIO_DIRECTION_INPUT);
	}

	if(!(dev = register_battery(bat, drv)))
	{
		median_free(pdat->mf);
		mean_free(pdat->nf);
		free(pdat->cc);
		free(pdat->dc);
		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
		return NULL;
	}
	return dev;
}

static void battery_adc_remove(struct device_t * dev)
{
	struct battery_t * bat = (struct battery_t *)dev->priv;
	struct battery_adc_pdata_t * pdat = (struct battery_adc_pdata_t *)bat->priv;

	if(bat)
	{
		unregister_battery(bat);
		median_free(pdat->mf);
		mean_free(pdat->nf);
		free(pdat->cc);
		free(pdat->dc);
		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
	}
}

static void battery_adc_suspend(struct device_t * dev)
{
}

static void battery_adc_resume(struct device_t * dev)
{
}

static struct driver_t battery_adc = {
	.name		= "battery-adc",
	.probe		= battery_adc_probe,
	.remove		= battery_adc_remove,
	.suspend	= battery_adc_suspend,
	.resume		= battery_adc_resume,
};

static __init void battery_adc_driver_init(void)
{
	register_driver(&battery_adc);
}

static __exit void battery_adc_driver_exit(void)
{
	unregister_driver(&battery_adc);
}

driver_initcall(battery_adc_driver_init);
driver_exitcall(battery_adc_driver_exit);
