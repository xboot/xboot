/*
 * driver/i2c-versatile.c
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
#include <i2c/i2c-algo-bit.h>

/*
 * Versatile i2c - I2C Controller On ARM Versatile Platform
 *
 * Optional properties:
 * - delay-us: half clock cycle time in us
 *      2 : fast-mode i2c - 400khz
 *      5 : standard-mode i2c and smbus - 100khz
 *     50 : slow-mode for smbus - 10khz
 *
 * Example:
 *   "i2c-versatile@0x10002000": {
 *       "delay-us": 5
 *   }
 */

#define I2C_CONTROL		(0x00)
#define I2C_CONTROLS	(0x00)
#define I2C_CONTROLC	(0x04)
#define SCL				(1 << 0)
#define SDA				(1 << 1)

struct i2c_versatile_pdata_t {
	virtual_addr_t virt;
	struct i2c_algo_bit_data_t bdat;
};

static void i2c_versatile_setsda(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_versatile_pdata_t * pdat = (struct i2c_versatile_pdata_t *)bdat->priv;
	write32(pdat->virt + (state ? I2C_CONTROLS : I2C_CONTROLC), SDA);
}

static void i2c_versatile_setscl(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_versatile_pdata_t * pdat = (struct i2c_versatile_pdata_t *)bdat->priv;
	write32(pdat->virt + (state ? I2C_CONTROLS : I2C_CONTROLC), SCL);
}

static int i2c_versatile_getsda(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_versatile_pdata_t * pdat = (struct i2c_versatile_pdata_t *)bdat->priv;
	return !!(read32(pdat->virt + I2C_CONTROL) & SDA);
}

static int i2c_versatile_getscl(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_versatile_pdata_t * pdat = (struct i2c_versatile_pdata_t *)bdat->priv;
	return !!(read32(pdat->virt + I2C_CONTROL) & SCL);
}

static int i2c_versatile_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_versatile_pdata_t * pdat = (struct i2c_versatile_pdata_t *)i2c->priv;
	struct i2c_algo_bit_data_t * bdat = (struct i2c_algo_bit_data_t *)(&pdat->bdat);
	return i2c_algo_bit_xfer(bdat, msgs, num);
}

static struct device_t * i2c_versatile_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_versatile_pdata_t * pdat;
	struct i2c_t * i2c;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct i2c_versatile_pdata_t));
	if(!pdat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;
	pdat->bdat.setsda = i2c_versatile_setsda;
	pdat->bdat.setscl = i2c_versatile_setscl;
	pdat->bdat.getsda = i2c_versatile_getsda;
	pdat->bdat.getscl = i2c_versatile_getscl;
	pdat->bdat.udelay = dt_read_int(n, "delay-us", 5);
	pdat->bdat.priv = pdat;

	i2c->name = alloc_device_name(dt_read_name(n), -1);
	i2c->xfer = i2c_versatile_xfer;
	i2c->priv = pdat;

	i2c_versatile_setsda(&pdat->bdat, 1);
	i2c_versatile_setscl(&pdat->bdat, 1);

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

static void i2c_versatile_remove(struct device_t * dev)
{
	struct i2c_t * i2c = (struct i2c_t *)dev->priv;

	if(i2c && unregister_i2c(i2c))
	{
		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
	}
}

static void i2c_versatile_suspend(struct device_t * dev)
{
}

static void i2c_versatile_resume(struct device_t * dev)
{
}

static struct driver_t i2c_versatile = {
	.name		= "i2c-versatile",
	.probe		= i2c_versatile_probe,
	.remove		= i2c_versatile_remove,
	.suspend	= i2c_versatile_suspend,
	.resume		= i2c_versatile_resume,
};

static __init void i2c_versatile_driver_init(void)
{
	register_driver(&i2c_versatile);
}

static __exit void i2c_versatile_driver_exit(void)
{
	unregister_driver(&i2c_versatile);
}

driver_initcall(i2c_versatile_driver_init);
driver_exitcall(i2c_versatile_driver_exit);
