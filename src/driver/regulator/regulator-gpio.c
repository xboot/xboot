/*
 * driver/regulator/regulator-gpio.c
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
#include <gpio/gpio.h>
#include <regulator/regulator.h>

/*
 * GPIO Regulator - Regulator Driver Using Generic Purpose Input Output
 *
 * Required properties:
 * - gpio: regulator control pin
 *
 * Optional properties:
 * - parent: regulator's parent, null means top level
 * - voltage: regulator's voltage in uV
 * - active-low: low level for active regulator
 * - default-enable: regulator default enable or disable
 *
 * Example:
 *   "regulator-gpio@0": {
 *       "parent": null,
 *       "name": "power-bl",
 *       "voltage": 12000000,
 *       "gpio": 74,
 *       "gpiocfg": -1,
 *       "active-low": false,
 *       "default-enable": false
 *   }
 */

struct regulator_gpio_pdata_t {
	char * parent;
	int voltage;
	int gpio;
	int gpiocfg;
	int active_low;
	int enable;
};

static void regulator_gpio_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_gpio_get_parent(struct regulator_t * supply)
{
	struct regulator_gpio_pdata_t * pdat = (struct regulator_gpio_pdata_t *)supply->priv;
	return pdat->parent;
}

static void regulator_gpio_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_gpio_pdata_t * pdat = (struct regulator_gpio_pdata_t *)supply->priv;

	if(pdat->enable != enable)
	{
		if(enable)
		{
			gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_DOWN: GPIO_PULL_UP);
			gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
		}
		else
		{
			gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
			gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
		}
		pdat->enable = enable;
	}
}

static bool_t regulator_gpio_get_enable(struct regulator_t * supply)
{
	struct regulator_gpio_pdata_t * pdat = (struct regulator_gpio_pdata_t *)supply->priv;
	return pdat->enable;
}

static void regulator_gpio_set_voltage(struct regulator_t * supply, int voltage)
{
}

static int regulator_gpio_get_voltage(struct regulator_t * supply)
{
	struct regulator_gpio_pdata_t * pdat = (struct regulator_gpio_pdata_t *)supply->priv;
	return pdat->voltage;
}

static struct device_t * regulator_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_gpio_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct dtnode_t o;
	char * name = dt_read_string(n, "name", NULL);

	if(!gpio_is_valid(dt_read_int(n, "gpio", -1)))
		return NULL;

	if(!name || search_regulator(name))
		return NULL;

	pdat = malloc(sizeof(struct regulator_gpio_pdata_t));
	if(!pdat)
		return NULL;

	supply = malloc(sizeof(struct regulator_t));
	if(!supply)
	{
		free(pdat);
		return NULL;
	}

	pdat->parent = strdup(dt_read_string(n, "parent", NULL));
	pdat->voltage = dt_read_int(n, "voltage", 0);
	pdat->gpio = dt_read_int(n, "gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->active_low = dt_read_bool(n, "active-low", 0);
	pdat->enable = -1;

	supply->name = strdup(name);
	supply->count = 0;
	supply->set_parent = regulator_gpio_set_parent;
	supply->get_parent = regulator_gpio_get_parent;
	supply->set_enable = regulator_gpio_set_enable;
	supply->get_enable = regulator_gpio_get_enable;
	supply->set_voltage = regulator_gpio_set_voltage;
	supply->get_voltage = regulator_gpio_get_voltage;
	supply->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);

	if(!register_regulator(&dev, supply))
	{
		if(pdat->parent)
			free(pdat->parent);

		free(supply->name);
		free(supply->priv);
		free(supply);
		return NULL;
	}
	dev->driver = drv;

	if(dt_read_object(n, "default", &o))
	{
		char * s = supply->name;
		char * p;
		int v;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_regulator(p))
			regulator_set_parent(s, p);
		if((v = dt_read_int(&o, "voltage", -1)) >= 0)
			regulator_set_voltage(s, v);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				regulator_enable(s);
			else
				regulator_disable(s);
		}
	}
	return dev;
}

static void regulator_gpio_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_gpio_pdata_t * pdat = (struct regulator_gpio_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		if(pdat->parent)
			free(pdat->parent);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_gpio_suspend(struct device_t * dev)
{
}

static void regulator_gpio_resume(struct device_t * dev)
{
}

static struct driver_t regulator_gpio = {
	.name		= "regulator-gpio",
	.probe		= regulator_gpio_probe,
	.remove		= regulator_gpio_remove,
	.suspend	= regulator_gpio_suspend,
	.resume		= regulator_gpio_resume,
};

static __init void regulator_gpio_driver_init(void)
{
	register_driver(&regulator_gpio);
}

static __exit void regulator_gpio_driver_exit(void)
{
	unregister_driver(&regulator_gpio);
}

driver_initcall(regulator_gpio_driver_init);
driver_exitcall(regulator_gpio_driver_exit);
