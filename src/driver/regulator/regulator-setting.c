/*
 * driver/regulator/regulator-setting.c
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

#include <xboot.h>
#include <regulator/regulator.h>

static void regulator_setting_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_setting_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_setting_set_enable(struct regulator_t * supply, bool_t enable)
{
}

static bool_t regulator_setting_get_enable(struct regulator_t * supply)
{
	return TRUE;
}

static void regulator_setting_set_voltage(struct regulator_t * supply, int voltage)
{
}

static int regulator_setting_get_voltage(struct regulator_t * supply)
{
	return 0;
}

static struct device_t * regulator_setting_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dtnode_t o;
	struct regulator_t * supply;
	struct device_t * dev;
	char * name;
	char * parent;
	int voltage;
	int nregulators = dt_read_array_length(n, "regulators");
	int i;

	for(i = 0; i < nregulators; i++)
	{
		dt_read_array_object(n, "regulators", i, &o);
		name = dt_read_string(&o, "name", NULL);

		if(name && search_regulator(name))
		{
			if((parent = dt_read_string(&o, "parent", NULL)) && search_regulator(parent))
				regulator_set_parent(name, parent);

			if((voltage = dt_read_int(&o, "voltage", -1)) >= 0)
				regulator_set_voltage(name, voltage);

			if(dt_read_bool(&o, "enable", 0))
				regulator_enable(name);
			else
				regulator_disable(name);
		}
	}

	supply = malloc(sizeof(struct regulator_t));
	if(!supply)
		return NULL;

	supply->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	supply->count = 0;
	supply->set_parent = regulator_setting_set_parent;
	supply->get_parent = regulator_setting_get_parent;
	supply->set_enable = regulator_setting_set_enable;
	supply->get_enable = regulator_setting_get_enable;
	supply->set_voltage = regulator_setting_set_voltage;
	supply->get_voltage = regulator_setting_get_voltage;
	supply->priv = 0;

	if(!register_regulator(&dev, supply))
	{
		free_device_name(supply->name);
		free(supply->priv);
		free(supply);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void regulator_setting_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;

	if(supply && unregister_regulator(supply))
	{
		free_device_name(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_setting_suspend(struct device_t * dev)
{
}

static void regulator_setting_resume(struct device_t * dev)
{
}

static struct driver_t regulator_setting = {
	.name		= "regulator-setting",
	.probe		= regulator_setting_probe,
	.remove		= regulator_setting_remove,
	.suspend	= regulator_setting_suspend,
	.resume		= regulator_setting_resume,
};

static __init void regulator_setting_driver_init(void)
{
	register_driver(&regulator_setting);
}

static __exit void regulator_setting_driver_exit(void)
{
	unregister_driver(&regulator_setting);
}

driver_initcall(regulator_setting_driver_init);
driver_exitcall(regulator_setting_driver_exit);
