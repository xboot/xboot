/*
 * driver/i2c-rk3399.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <i2c/i2c.h>

enum {
	I2C_CON			= 0x000,
	I2C_CLKDIV		= 0x004,
	I2C_MRXADDR 	= 0x008,
	I2C_MRXRADDR	= 0x00c,
	I2C_MTXCNT		= 0x010,
	I2C_MRXCNT		= 0x014,
	I2C_IEN			= 0x018,
	I2C_IPD			= 0x01c,
	I2C_FCNT		= 0x020,
	I2C_TXDATA_BASE	= 0x100,
	I2C_RXDATA_BASE	= 0x200,
};

struct i2c_rk3399_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int sda;
	int sdacfg;
	int scl;
	int sclcfg;
};

static void rk3399_i2c_set_rate(struct i2c_rk3399_pdata_t * pdat, u64_t rate)
{
	u64_t pclk = clk_get_rate(pdat->clk);
	u32_t divl = 0, divh = 0;
	u32_t div;

	div = ((pclk + (rate * 8) - 1) / (rate * 8)) - 2;
	if(div >= 0)
	{
		divl = div / 2;
		if(div % 2 == 0)
			divh = div / 2;
		else
			divh = (div + 2 - 1) / 2;
	}
	write32(pdat->virt + I2C_CLKDIV, ((divl & 0xffff) << 0) | ((divh & 0xffff) << 16));
}

static void rk3399_i2c_start(struct i2c_rk3399_pdata_t * pdat)
{
	write32(pdat->virt + I2C_IPD, 0x7f);
	write32(pdat->virt + I2C_CON, (1 << 0) | (1 << 3));

	ktime_t timeout = ktime_add_us(ktime_get(), 10);
	do {
		if(read32(pdat->virt + I2C_IPD) & (1 << 4))
		{
			write32(pdat->virt + I2C_IPD, (1 << 4));
			break;
		}
	} while(ktime_before(ktime_get(), timeout));
}

static void rk3399_i2c_stop(struct i2c_rk3399_pdata_t * pdat)
{
	write32(pdat->virt + I2C_IPD, 0x7f);
	write32(pdat->virt + I2C_CON, (1 << 0) | (1 << 4));

	ktime_t timeout = ktime_add_us(ktime_get(), 10);
	do {
		if(read32(pdat->virt + I2C_IPD) & (1 << 5))
		{
			write32(pdat->virt + I2C_IPD, (1 << 5));
			break;
		}
	} while(ktime_before(ktime_get(), timeout));
	write32(pdat->virt + I2C_CON, 0);
}

static int rk3399_i2c_read(struct i2c_rk3399_pdata_t * pdat, struct i2c_msg_t * msg)
{
	u32_t data = 0;
	u32_t con = 0;
	u8_t * p = msg->buf;
	int len = msg->len;
	int bytes = 0;
	int words = 0;
	int i, j;

	write32(pdat->virt + I2C_MRXADDR, (1 << 24) | (msg->addr << 1) | 1);
	write32(pdat->virt + I2C_MRXRADDR, 0);
	con = (1 << 6) | (1 << 1) | (1 << 0);

	while(len)
	{
		bytes = len < 32 ? len : 32;
		len -= bytes;
		if(!len)
			con |= (1 << 5) | (1 << 0);
		words = (bytes + 4 - 1) / 4;

		write32(pdat->virt + I2C_IPD, 0x7f);
		write32(pdat->virt + I2C_CON, con);
		write32(pdat->virt + I2C_MRXCNT, bytes);

		ktime_t timeout = ktime_add_ms(ktime_get(), 100);
		while(1)
		{
			if(read32(pdat->virt + I2C_IPD) & (1 << 3))
			{
				write32(pdat->virt + I2C_IPD, (1 << 3));
				break;
			}
			if(read32(pdat->virt + I2C_IPD) & (1 << 6))
			{
				write32(pdat->virt + I2C_IPD, (1 << 6));
				write32(pdat->virt + I2C_MTXCNT, 0);
				write32(pdat->virt + I2C_CON, 0);
				return -1;
			}
			if(ktime_after(ktime_get(), timeout))
			{
				write32(pdat->virt + I2C_MTXCNT, 0);
				write32(pdat->virt + I2C_CON, 0);
				return -1;
			}
		}

		for(i = 0; i < words; i++)
		{
			data = read32(pdat->virt + I2C_RXDATA_BASE + 0x4 * i);
			for(j = 0; j < 4; j++)
			{
				if((i * 4 + j) == bytes)
					break;
				*p++ = (data >> (j * 8)) & 0xff;
			}
		}
		con = (1 << 6) | (2 << 1) | (1 << 0);
	}
	return 0;
}

static int rk3399_i2c_write(struct i2c_rk3399_pdata_t * pdat, struct i2c_msg_t * msg)
{
	u32_t data = 0;
	u8_t * p = msg->buf;
	int len = msg->len + 1;
	int bytes = 0;
	int words = 0;
	int i, j = 1;

	data |= (msg->addr << 1);
	while(len)
	{
		bytes = len < 32 ? len : 32;
		words = (bytes + 4 - 1) / 4;
		for(i = 0; i < words; i++)
		{
			do {
				if((i * 4 + j) == bytes)
					break;
				data |= (*p++) << (j * 8);
			} while(++j < 4);

			write32(pdat->virt + I2C_TXDATA_BASE + 0x4 * i, data);
			j = 0;
			data = 0;
		}

		write32(pdat->virt + I2C_IPD, 0x7f);
		write32(pdat->virt + I2C_CON, (1 << 6) | (0 << 1) | (1 << 0));
		write32(pdat->virt + I2C_MTXCNT, bytes);

		ktime_t timeout = ktime_add_ms(ktime_get(), 100);
		while(1)
		{
			if(read32(pdat->virt + I2C_IPD) & (1 << 2))
			{
				write32(pdat->virt + I2C_IPD, (1 << 2));
				break;
			}
			if(read32(pdat->virt + I2C_IPD) & (1 << 6))
			{
				write32(pdat->virt + I2C_IPD, (1 << 6));
				write32(pdat->virt + I2C_MTXCNT, 0);
				write32(pdat->virt + I2C_CON, 0);
				return -1;
			}
			if(ktime_after(ktime_get(), timeout))
			{
				write32(pdat->virt + I2C_MTXCNT, 0);
				write32(pdat->virt + I2C_CON, 0);
				return -1;
			}
		}
		len -= bytes;
	}
	return 0;
}

static int i2c_rk3399_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_rk3399_pdata_t * pdat = (struct i2c_rk3399_pdata_t *) i2c->priv;
	struct i2c_msg_t * pmsg = msgs;
	int i, res;

	if(!msgs || num <= 0)
		return 0;

	rk3399_i2c_start(pdat);
	for(i = 0; i < num; i++, pmsg++)
	{
		if(i != 0)
			rk3399_i2c_start(pdat);
		if(pmsg->flags & I2C_M_RD)
			res = rk3399_i2c_read(pdat, pmsg);
		else
			res = rk3399_i2c_write(pdat, pmsg);
		if(res < 0)
			break;
	}
	rk3399_i2c_stop(pdat);

	return i;
}

static struct device_t * i2c_rk3399_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_rk3399_pdata_t * pdat;
	struct i2c_t * i2c;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct i2c_rk3399_pdata_t));
	if(!pdat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->sda = dt_read_int(n, "sda-gpio", -1);
	pdat->sdacfg = dt_read_int(n, "sda-gpio-config", -1);
	pdat->scl = dt_read_int(n, "scl-gpio", -1);
	pdat->sclcfg = dt_read_int(n, "scl-gpio-config", -1);

	i2c->name = alloc_device_name(dt_read_name(n), -1);
	i2c->xfer = i2c_rk3399_xfer;
	i2c->priv = pdat;

	clk_enable(pdat->clk);
	rk3399_i2c_set_rate(pdat, (u64_t)dt_read_long(n, "clock-frequency", 400000));
	if(pdat->sda >= 0)
	{
		if(pdat->sdacfg >= 0)
			gpio_set_cfg(pdat->sda, pdat->sdacfg);
		gpio_set_pull(pdat->sda, GPIO_PULL_UP);
	}
	if(pdat->scl >= 0)
	{
		if(pdat->sclcfg >= 0)
			gpio_set_cfg(pdat->scl, pdat->sclcfg);
		gpio_set_pull(pdat->scl, GPIO_PULL_UP);
	}

	if(!register_i2c(&dev, i2c))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void i2c_rk3399_remove(struct device_t * dev)
{
	struct i2c_t * i2c = (struct i2c_t *)dev->priv;
	struct i2c_rk3399_pdata_t * pdat = (struct i2c_rk3399_pdata_t *)i2c->priv;

	if(i2c && unregister_i2c(i2c))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
	}
}

static void i2c_rk3399_suspend(struct device_t * dev)
{
}

static void i2c_rk3399_resume(struct device_t * dev)
{
}

static struct driver_t i2c_rk3399 = {
	.name		= "i2c-rk3399",
	.probe		= i2c_rk3399_probe,
	.remove		= i2c_rk3399_remove,
	.suspend	= i2c_rk3399_suspend,
	.resume		= i2c_rk3399_resume,
};

static __init void i2c_rk3399_driver_init(void)
{
	register_driver(&i2c_rk3399);
}

static __exit void i2c_rk3399_driver_exit(void)
{
	unregister_driver(&i2c_rk3399);
}

driver_initcall(i2c_rk3399_driver_init);
driver_exitcall(i2c_rk3399_driver_exit);
