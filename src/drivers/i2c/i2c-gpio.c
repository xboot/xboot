/*
 * drivers/i2c/i2c-gpio.c
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
#include <gpio/gpio.h>
#include <i2c/i2c.h>
#include <i2c/i2c-algo-bit.h>

/*
 * GPIO I2C - I2C Master Driver Using Generic Bitbanged GPIO
 *
 * Required properties:
 * - sda-gpio: i2c data pin
 * - scl-gpio: i2c clock pin
 *
 * Optional properties:
 * - sda-open-drain: sda as open drain
 * - scl-open-drain: scl as open drain
 * - scl-output-only: scl as output only
 * - delay-us: delay between GPIO operations (half clock cycle time in us)
 *      2 : fast-mode i2c - 400khz
 *      5 : standard-mode i2c and smbus - 100khz
 *     50 : slow-mode for smbus - 10khz
 *
 * Example:
 *   "i2c-gpio": {
 *       "sda-gpio": 11,
 *       "scl-gpio": 12,
 *       "sda-open-drain": false,
 *       "scl-open-drain": false,
 *       "scl-output-only": false,
 *       "delay-us": 5
 *   }
 */

struct i2c_gpio_pdata_t {
	struct i2c_algo_bit_data_t bdat;
	int sda;
	int scl;
	int sda_open_drain;
	int scl_open_drain;
	int scl_output_only;
	int udelay;
};

static void i2c_gpio_setsda_dir(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	if(state)
		gpio_direction_input(pdat->sda);
	else
		gpio_direction_output(pdat->sda, 0);
}

static void i2c_gpio_setsda_val(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	gpio_set_value(pdat->sda, state);
}

static void i2c_gpio_setscl_dir(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	if(state)
		gpio_direction_input(pdat->scl);
	else
		gpio_direction_output(pdat->scl, 0);
}

static void i2c_gpio_setscl_val(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	gpio_set_value(pdat->scl, state);
}

static int i2c_gpio_getsda(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	return gpio_get_value(pdat->sda);
}

static int i2c_gpio_getscl(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)bdat->priv;
	return gpio_get_value(pdat->scl);
}

static int i2c_gpio_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_gpio_pdata_t * pdat = (struct i2c_gpio_pdata_t *)i2c->priv;
	struct i2c_algo_bit_data_t * bdat = (struct i2c_algo_bit_data_t *)&(pdat->bdat);
	return i2c_algo_bit_xfer(bdat, msgs, num);
}

static struct device_t * i2c_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_gpio_pdata_t * pdat;
	struct i2c_t * i2c;
	struct device_t * dev;
	int sda = dt_read_int(n, "sda-gpio", -1);
	int scl = dt_read_int(n, "scl-gpio", -1);

	if(!gpio_is_valid(sda) || !gpio_is_valid(scl))
		return NULL;

	pdat = malloc(sizeof(struct i2c_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(pdat);
		return FALSE;
	}

	pdat->sda = sda;
	pdat->scl = scl;
	pdat->sda_open_drain = dt_read_bool(n, "sda-open-drain", 0);
	pdat->scl_open_drain = dt_read_bool(n, "scl-open-drain", 0);
	pdat->scl_output_only = dt_read_bool(n, "sda-output-only", 0);
	pdat->udelay = dt_read_int(n, "delay-us", 5);
	pdat->bdat.priv = pdat;

	if(pdat->sda_open_drain)
	{
		gpio_direction_output(pdat->sda, 1);
		pdat->bdat.setsda = i2c_gpio_setsda_val;
	}
	else
	{
		gpio_direction_input(pdat->sda);
		pdat->bdat.setsda = i2c_gpio_setsda_dir;
	}

	if(pdat->scl_open_drain || pdat->scl_output_only)
	{
		gpio_direction_output(pdat->scl, 1);
		pdat->bdat.setscl = i2c_gpio_setscl_val;
	}
	else
	{
		gpio_direction_input(pdat->scl);
		pdat->bdat.setscl = i2c_gpio_setscl_dir;
	}

	pdat->bdat.getsda = i2c_gpio_getsda;
	if(pdat->scl_output_only)
		pdat->bdat.getscl = 0;
	else
		pdat->bdat.getscl = i2c_gpio_getscl;

	if(pdat->udelay > 0)
		pdat->bdat.udelay = pdat->udelay;
	else if(pdat->scl_output_only)
		pdat->bdat.udelay = 50;
	else
		pdat->bdat.udelay = 5;

	i2c->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	i2c->xfer = i2c_gpio_xfer,
	i2c->priv = pdat;

	if(!register_i2c(&dev, i2c))
	{
		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void i2c_gpio_remove(struct device_t * dev)
{
	struct i2c_t * i2c = (struct i2c_t *)dev->priv;

	if(i2c && unregister_i2c(i2c))
	{
		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
	}
}

static void i2c_gpio_suspend(struct device_t * dev)
{
}

static void i2c_gpio_resume(struct device_t * dev)
{
}

struct driver_t i2c_gpio = {
	.name		= "i2c-gpio",
	.probe		= i2c_gpio_probe,
	.remove		= i2c_gpio_remove,
	.suspend	= i2c_gpio_suspend,
	.resume		= i2c_gpio_resume,
};

static __init void i2c_gpio_driver_init(void)
{
	register_driver(&i2c_gpio);
}

static __exit void i2c_gpio_driver_exit(void)
{
	unregister_driver(&i2c_gpio);
}

driver_initcall(i2c_gpio_driver_init);
driver_exitcall(i2c_gpio_driver_exit);
