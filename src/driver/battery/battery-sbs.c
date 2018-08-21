/*
 * driver/battery-sbs.c
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
#include <i2c/i2c.h>
#include <battery/battery.h>

/*
 * SBS - Smart Battery System
 *
 * Required properties:
 * - i2c-bus: i2c bus name which device attached
 *
 * Optional properties:
 * - slave-address: sbs battery slave address
 *
 * Example:
 *   "battery-sbs": {
 *       "i2c-bus": "i2c-gpio.0"
 *   }
 */

enum {
	SBS_TEMPERATURE 		= 0x08,
	SBS_VOLTAGE				= 0x09,
	SBS_CURRENT 			= 0x0a,
	SBS_AVG_CURRENT 		= 0x0b,
	SBS_RSOC 				= 0x0d,
	SBS_ASOC 				= 0x0e,
	SBS_RCAP 				= 0x0f,
	SBS_FCAP 				= 0x10,
	SBS_RTIME_TO_EMPTY 		= 0x11,
	SBS_AVGTIME_TO_EMPTY 	= 0x12,
	SBS_AVGTIME_TO_FULL 	= 0x13,
	SBS_BATTERY_STATUS		= 0x16,
	SBS_CYCLE_COUNT			= 0x17,
	SBS_DESIGN_CAPACITY		= 0x18,
	SBS_DESIGN_VOLTAGE 		= 0x19,
};

struct battery_sbs_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t sbs_read_byte(struct i2c_device_t * dev, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
    u8_t buf;

    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = &buf;

    if(i2c_transfer(dev->i2c, msgs, 2) != 2)
    	return FALSE;

    if(val)
    	*val = buf;
    return TRUE;
}

static bool_t sbs_read_word(struct i2c_device_t * dev, u8_t reg, u16_t * val)
{
	struct i2c_msg_t msgs[2];
    u8_t buf[2];

    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 2;
    msgs[1].buf = &buf[0];

    if(i2c_transfer(dev->i2c, msgs, 2) != 2)
    	return FALSE;

    if(val)
    	*val = ((u16_t)buf[1] << 8) | buf[0];
    return TRUE;
}

static bool_t battery_sbs_update(struct battery_t * bat, struct battery_info_t * info)
{
	struct battery_sbs_pdata_t * pdat = (struct battery_sbs_pdata_t *)bat->priv;
	u16_t val;
	u8_t soc;

	if(!sbs_read_word(pdat->dev, SBS_BATTERY_STATUS, &val))
		return FALSE;
	if(val & (0x1 << 5))
		info->status = BATTERY_STATUS_FULL;
	else if(val & (0x1 << 4))
		info->status = BATTERY_STATUS_EMPTY;
	else if(val & (0x1 << 6))
		info->status = BATTERY_STATUS_DISCHARGING;
	else
		info->status = BATTERY_STATUS_CHARGING;

	if(val & (0x1 << 12))
		info->health = BATTERY_HEALTH_OVERHEAT;
	else
		info->health = BATTERY_HEALTH_GOOD;

	if(info->status == BATTERY_STATUS_CHARGING)
		info->supply = POWER_SUPPLAY_AC;
	else
		info->supply = POWER_SUPPLAY_BATTERY;

	if(!sbs_read_word(pdat->dev, SBS_DESIGN_CAPACITY, &val))
		return FALSE;
	info->design_capacity = val;

	if(!sbs_read_word(pdat->dev, SBS_DESIGN_VOLTAGE, &val))
		return FALSE;
	info->design_voltage = val;

	if(!sbs_read_word(pdat->dev, SBS_VOLTAGE, &val))
		return 0;
	info->voltage = val;

	if(!sbs_read_word(pdat->dev, SBS_CURRENT, &val))
		return 0;
	info->current = (int)((s16_t)val);

	if(!sbs_read_word(pdat->dev, SBS_TEMPERATURE, &val))
		return FALSE;
	info->temperature = (val - 2732) * 100;

	if(!sbs_read_word(pdat->dev, SBS_CYCLE_COUNT, &val))
		return FALSE;
	info->cycle = val;

	if(!sbs_read_byte(pdat->dev, SBS_RSOC, &soc))
		return FALSE;
	info->level = soc;

	return TRUE;
}

static struct device_t * battery_sbs_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct battery_sbs_pdata_t * pdat;
	struct battery_t * bat;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u16_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x0b), 0);
	if(!i2cdev)
		return NULL;

	if(sbs_read_word(i2cdev, SBS_BATTERY_STATUS, &val))
	{
		if(val & (0x1 << 7))
		{
		}
		else
		{
		}
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct battery_sbs_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	bat = malloc(sizeof(struct battery_t));
	if(!bat)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;

	bat->name = alloc_device_name(dt_read_name(n), -1);
	bat->update = battery_sbs_update;
	bat->priv = pdat;

	if(!register_battery(&dev, bat))
	{
		i2c_device_free(pdat->dev);

		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void battery_sbs_remove(struct device_t * dev)
{
	struct battery_t * bat = (struct battery_t *)dev->priv;
	struct battery_sbs_pdata_t * pdat = (struct battery_sbs_pdata_t *)bat->priv;

	if(bat && unregister_battery(bat))
	{
		i2c_device_free(pdat->dev);

		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
	}
}

static void battery_sbs_suspend(struct device_t * dev)
{
}

static void battery_sbs_resume(struct device_t * dev)
{
}

static struct driver_t battery_sbs = {
	.name		= "battery-sbs",
	.probe		= battery_sbs_probe,
	.remove		= battery_sbs_remove,
	.suspend	= battery_sbs_suspend,
	.resume		= battery_sbs_resume,
};

static __init void battery_sbs_driver_init(void)
{
	register_driver(&battery_sbs);
}

static __exit void battery_sbs_driver_exit(void)
{
	unregister_driver(&battery_sbs);
}

driver_initcall(battery_sbs_driver_init);
driver_exitcall(battery_sbs_driver_exit);
