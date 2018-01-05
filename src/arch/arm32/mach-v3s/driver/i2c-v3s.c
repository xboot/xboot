/*
 * driver/i2c-v3s.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <i2c/i2c.h>

#if 0
enum {
	I2C_ADDR	= 0x000,
	I2C_XADDR	= 0x004,
	I2C_DATA 	= 0x008,
	I2C_CNTR	= 0x00c,
	I2C_STAT	= 0x010,
	I2C_CCR		= 0x014,
	I2C_SRST	= 0x018,
	I2C_EFR		= 0x01c,
	I2C_LCR		= 0x020,
};

struct i2c_v3s_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int sda;
	int sdacfg;
	int scl;
	int sclcfg;
};

static void v3s_i2c_set_rate(struct i2c_v3s_pdata_t * pdat, u64_t rate)
{
	u64_t pclk = clk_get_rate(pdat->clk);
	u32_t m, n;

	n = 3;
	m = (pclk >> n) / rate;
	write32(pdat->virt + I2C_CCR, (((m - 1) & 0xf) << 3) | ((n & 0x7) << 0));
}

static void v3s_i2c_start(struct i2c_v3s_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + I2C_CNTR);
	val &= ~(1 << 3);
	val |= (1 << 5);
	write32(pdat->virt + I2C_CNTR, val);

	ktime_t timeout = ktime_add_us(ktime_get(), 10);
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 5)))
			break;
	} while(ktime_before(ktime_get(), timeout));
}

static void v3s_i2c_stop(struct i2c_v3s_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + I2C_CNTR);
	val &= ~(1 << 3);
	val |= (1 << 4);
	write32(pdat->virt + I2C_CNTR, val);

	ktime_t timeout = ktime_add_us(ktime_get(), 10);
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 4)))
			break;
	} while(ktime_before(ktime_get(), timeout));
}

static int v3s_i2c_read(struct i2c_v3s_pdata_t * pdat, struct i2c_msg_t * msg)
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

static int v3s_i2c_write(struct i2c_v3s_pdata_t * pdat, struct i2c_msg_t * msg)
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

static int i2c_v3s_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_v3s_pdata_t * pdat = (struct i2c_v3s_pdata_t *) i2c->priv;
	struct i2c_msg_t * pmsg = msgs;
	int i, res;

	if(!msgs || num <= 0)
		return 0;

	v3s_i2c_start(pdat);
	for(i = 0; i < num; i++, pmsg++)
	{
		if(i != 0)
			v3s_i2c_start(pdat);
		if(pmsg->flags & I2C_M_RD)
			res = v3s_i2c_read(pdat, pmsg);
		else
			res = v3s_i2c_write(pdat, pmsg);
		if(res < 0)
			break;
	}
	v3s_i2c_stop(pdat);

	return i;
}

static struct device_t * i2c_v3s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_v3s_pdata_t * pdat;
	struct i2c_t * i2c;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct i2c_v3s_pdata_t));
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
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->sda = dt_read_int(n, "sda-gpio", -1);
	pdat->sdacfg = dt_read_int(n, "sda-gpio-config", -1);
	pdat->scl = dt_read_int(n, "scl-gpio", -1);
	pdat->sclcfg = dt_read_int(n, "scl-gpio-config", -1);

	i2c->name = alloc_device_name(dt_read_name(n), -1);
	i2c->xfer = i2c_v3s_xfer,
	i2c->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
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
	v3s_i2c_set_rate(pdat, (u64_t)dt_read_long(n, "clock-frequency", 400000));
	write32(pdat->virt + I2C_CNTR, 1 << 6);
	write32(pdat->virt + I2C_SRST, 1 << 0);

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

static void i2c_v3s_remove(struct device_t * dev)
{
	struct i2c_t * i2c = (struct i2c_t *)dev->priv;
	struct i2c_v3s_pdata_t * pdat = (struct i2c_v3s_pdata_t *)i2c->priv;

	if(i2c && unregister_i2c(i2c))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
	}
}

static void i2c_v3s_suspend(struct device_t * dev)
{
}

static void i2c_v3s_resume(struct device_t * dev)
{
}

static struct driver_t i2c_v3s = {
	.name		= "i2c-v3s",
	.probe		= i2c_v3s_probe,
	.remove		= i2c_v3s_remove,
	.suspend	= i2c_v3s_suspend,
	.resume		= i2c_v3s_resume,
};

static __init void i2c_v3s_driver_init(void)
{
	register_driver(&i2c_v3s);
}

static __exit void i2c_v3s_driver_exit(void)
{
	unregister_driver(&i2c_v3s);
}

driver_initcall(i2c_v3s_driver_init);
driver_exitcall(i2c_v3s_driver_exit);
#endif
