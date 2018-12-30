/*
 * driver/i2c-f1c100s.c
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
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <i2c/i2c.h>

enum {
	I2C_ADDR			= 0x000,
	I2C_XADDR			= 0x004,
	I2C_DATA 			= 0x008,
	I2C_CNTR			= 0x00c,
	I2C_STAT			= 0x010,
	I2C_CCR				= 0x014,
	I2C_SRST			= 0x018,
	I2C_EFR				= 0x01c,
	I2C_LCR				= 0x020,
};

enum {
	I2C_STAT_BUS_ERROR	= 0x00,
	I2C_STAT_TX_START	= 0x08,
	I2C_STAT_TX_RSTART	= 0x10,
	I2C_STAT_TX_AW_ACK	= 0x18,
	I2C_STAT_TX_AW_NAK	= 0x20,
	I2C_STAT_TXD_ACK	= 0x28,
	I2C_STAT_TXD_NAK	= 0x30,
	I2C_STAT_LOST_ARB	= 0x38,
	I2C_STAT_TX_AR_ACK	= 0x40,
	I2C_STAT_TX_AR_NAK	= 0x48,
	I2C_STAT_RXD_ACK	= 0x50,
	I2C_STAT_RXD_NAK	= 0x58,
	I2C_STAT_IDLE		= 0xf8,
};

struct i2c_f1c100s_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int sda;
	int sdacfg;
	int scl;
	int sclcfg;
};

static void f1c100s_i2c_set_rate(struct i2c_f1c100s_pdata_t * pdat, u64_t rate)
{
	u64_t pclk = clk_get_rate(pdat->clk);
	s64_t freq, delta, best = 0x7fffffffffffffffLL;
	int tm = 5, tn = 0;
	int m, n;

	for(n = 0; n <= 7; n++)
	{
		for(m = 0; m <= 15; m++)
		{
			freq = pclk / (10 * (m + 1) * (1 << n));
			delta = rate - freq;
			if(delta >= 0 && delta < best)
			{
				tm = m;
				tn = n;
				best = delta;
			}
			if(best == 0)
				break;
		}
	}
	write32(pdat->virt + I2C_CCR, ((tm & 0xf) << 3) | ((tn & 0x7) << 0));
}

static int f1c100s_i2c_wait_status(struct i2c_f1c100s_pdata_t * pdat)
{
	ktime_t timeout = ktime_add_ms(ktime_get(), 1);
	do {
		if((read32(pdat->virt + I2C_CNTR) & (1 << 3)))
			return read32(pdat->virt + I2C_STAT);
	} while(ktime_before(ktime_get(), timeout));
	return I2C_STAT_BUS_ERROR;
}

static int f1c100s_i2c_start(struct i2c_f1c100s_pdata_t * pdat)
{
	write32(pdat->virt + I2C_CNTR, (1 << 7) | (1 << 6) | (1 << 5) | (1 << 2));

	ktime_t timeout = ktime_add_ms(ktime_get(), 1);
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 5)))
			break;
	} while(ktime_before(ktime_get(), timeout));
	return f1c100s_i2c_wait_status(pdat);
}

static int f1c100s_i2c_stop(struct i2c_f1c100s_pdata_t * pdat)
{
	write32(pdat->virt + I2C_CNTR, (read32(pdat->virt + I2C_CNTR) | (1 << 4)) & ~((1 << 7) | (1 << 3)));
	ktime_t timeout = ktime_add_ms(ktime_get(), 1);
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 4)))
			return 0;
	} while(ktime_before(ktime_get(), timeout));
	return -1;
}

static int f1c100s_i2c_send_data(struct i2c_f1c100s_pdata_t * pdat, u8_t dat)
{
	write32(pdat->virt + I2C_DATA, dat);
	write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) & (~(1 << 3)));
	return f1c100s_i2c_wait_status(pdat);
}

static int f1c100s_i2c_read(struct i2c_f1c100s_pdata_t * pdat, struct i2c_msg_t * msg)
{
	u8_t * p = msg->buf;
	int len = msg->len;

	if(f1c100s_i2c_send_data(pdat, (u8_t)(msg->addr << 1 | 1)) != I2C_STAT_TX_AR_ACK)
		return -1;

	while(len > 0)
	{
		switch(f1c100s_i2c_wait_status(pdat))
		{
			case I2C_STAT_TX_AR_ACK:
				if(len == 1)
					write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) & ~((1 << 3) | (1 << 2)));
				else
					write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) & ~(1 << 3));
				break;
			case I2C_STAT_RXD_ACK:
				*p++ = read32(pdat->virt + I2C_DATA);
				len--;
				if(len == 1)
					write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) & ~((1 << 3) | (1 << 2)));
				else
					write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) & ~(1 << 3));
				break;
			case I2C_STAT_RXD_NAK:
				*p++ = read32(pdat->virt + I2C_DATA);
				len--;
				break;
			default:
				return -1;
		}
	}
	return 0;
}

static int f1c100s_i2c_write(struct i2c_f1c100s_pdata_t * pdat, struct i2c_msg_t * msg)
{
	u8_t * p = msg->buf;
	int len = msg->len;

	if(f1c100s_i2c_send_data(pdat, (u8_t)(msg->addr << 1)) != I2C_STAT_TX_AW_ACK)
		return -1;
	while(len > 0)
	{
		if(f1c100s_i2c_send_data(pdat, *p++) != I2C_STAT_TXD_ACK)
			return -1;
		len--;
	}
	return 0;
}

static int i2c_f1c100s_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_f1c100s_pdata_t * pdat = (struct i2c_f1c100s_pdata_t *) i2c->priv;
	struct i2c_msg_t * pmsg = msgs;
	int i, res;

	if(!msgs || num <= 0)
		return 0;

	if(f1c100s_i2c_start(pdat) != I2C_STAT_TX_START)
		return 0;

	for(i = 0; i < num; i++, pmsg++)
	{
		if(i != 0)
		{
			if(f1c100s_i2c_start(pdat) != I2C_STAT_TX_RSTART)
				break;
		}
		if(pmsg->flags & I2C_M_RD)
			res = f1c100s_i2c_read(pdat, pmsg);
		else
			res = f1c100s_i2c_write(pdat, pmsg);
		if(res < 0)
			break;
	}
	f1c100s_i2c_stop(pdat);

	return i;
}

static struct device_t * i2c_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_f1c100s_pdata_t * pdat;
	struct i2c_t * i2c;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct i2c_f1c100s_pdata_t));
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
	i2c->xfer = i2c_f1c100s_xfer;
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
	write32(pdat->virt + I2C_SRST, 1 << 0);
	write32(pdat->virt + I2C_SRST, 0 << 0);
	f1c100s_i2c_set_rate(pdat, (u64_t)dt_read_long(n, "clock-frequency", 400000));
	write32(pdat->virt + I2C_ADDR, 0);
	write32(pdat->virt + I2C_XADDR, 0);
	write32(pdat->virt + I2C_CNTR, (1 << 6) | (1 << 4));

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

static void i2c_f1c100s_remove(struct device_t * dev)
{
	struct i2c_t * i2c = (struct i2c_t *)dev->priv;
	struct i2c_f1c100s_pdata_t * pdat = (struct i2c_f1c100s_pdata_t *)i2c->priv;

	if(i2c && unregister_i2c(i2c))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(i2c->name);
		free(i2c->priv);
		free(i2c);
	}
}

static void i2c_f1c100s_suspend(struct device_t * dev)
{
}

static void i2c_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t i2c_f1c100s = {
	.name		= "i2c-f1c100s",
	.probe		= i2c_f1c100s_probe,
	.remove		= i2c_f1c100s_remove,
	.suspend	= i2c_f1c100s_suspend,
	.resume		= i2c_f1c100s_resume,
};

static __init void i2c_f1c100s_driver_init(void)
{
	register_driver(&i2c_f1c100s);
}

static __exit void i2c_f1c100s_driver_exit(void)
{
	unregister_driver(&i2c_f1c100s);
}

driver_initcall(i2c_f1c100s_driver_init);
driver_exitcall(i2c_f1c100s_driver_exit);
