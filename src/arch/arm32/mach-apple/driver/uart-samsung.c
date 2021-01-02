/*
 * driver/uart-samsung.c
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <uart/uart.h>

/*
 * Samsung uart - Universal Asynchronous Receiver Transmitter
 *
 * Required properties:
 * - clock-name: uart parant clock name
 *
 * Optional properties:
 * - txd-gpio: uart txd gpio
 * - txd-gpio-config: uart txd gpio config
 * - rxd-gpio: uart rxd gpio
 * - rxd-gpio-config: uart rxd gpio config
 * - baud-rates: uart baud rates, default is 115200
 * - data-bits: uart data bits, default is 8
 * - parity-bits: uart parity bits, default is 0
 * - stop-bits: uart stop bits, default is 1
 *
 * Example:
 *   "uart-samsung@0x10009000": {
 *       "clock-name": "uclk",
 *       "txd-gpio": -1,
 *       "txd-gpio-config": -1,
 *       "rxd-gpio": -1,
 *       "rxd-gpio-config": -1,
 *       "baud-rates": 115200,
 *       "data-bits": 8,
 *       "parity-bits": 0,
 *       "stop-bits": 1
 *   }
 */

enum {
	UART_ULCON		= 0x00,
	UART_UCON		= 0x04,
	UART_UFCON		= 0x08,
	UART_UMCON		= 0x0C,
	UART_UTRSTAT	= 0x10,
	UART_UERSTAT	= 0x14,
	UART_UFSTAT		= 0x18,
	UART_UMSTAT		= 0x1C,
	UART_UTXH		= 0x20,
	UART_URXH		= 0x24,
	UART_UBRDIV		= 0x28,
	UART_UFRACVAL	= 0x2C,
	UART_UINTP		= 0x30,
	UART_UINTSP		= 0x34,
	UART_UINTM		= 0x38,
};

struct uart_samsung_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int txd;
	int txdcfg;
	int rxd;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
};

static bool_t uart_samsung_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_samsung_pdata_t * pdat = (struct uart_samsung_pdata_t *)uart->priv;
	const u32_t udivslot_code[16] = {0x0000, 0x0080, 0x0808, 0x0888,
									 0x2222, 0x4924, 0x4a52, 0x54aa,
									 0x5555, 0xd555, 0xd5d5, 0xddd5,
									 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
	u32_t baud_div_reg, baud_divslot_reg;
	u8_t dreg, preg, sreg;
	u64_t rate;

	if(baud < 0)
		return FALSE;
	if((data < 5) || (data > 8))
		return FALSE;
	if((parity < 0) || (parity > 2))
		return FALSE;
	if((stop < 0) || (stop > 2))
		return FALSE;

	switch(data)
	{
	case 5:	/* Data bits = 5 */
		dreg = 0x0;
		break;
	case 6:	/* Data bits = 6 */
		dreg = 0x1;
		break;
	case 7:	/* Data bits = 7 */
		dreg = 0x2;
		break;
	case 8:	/* Data bits = 8 */
		dreg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case 0:	/* Parity none */
		preg = 0x0;
		break;
	case 1:	/* Parity odd */
		preg = 0x4;
		break;
	case 2:	/* Parity even */
		preg = 0x5;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case 1:	/* Stop bits = 1 */
		sreg = 0;
		break;
	case 2:	/* Stop bits = 2 */
		sreg = 1;
		break;
	case 0:	/* Stop bits = 1.5 */
	default:
		return FALSE;
	}

	pdat->baud = baud;
	pdat->data = data;
	pdat->parity = parity;
	pdat->stop = stop;

	rate = clk_get_rate(pdat->clk);
	baud_div_reg = (u32_t)((rate / (baud * 16))) - 1;
	baud_divslot_reg = udivslot_code[( (u32_t)((rate % (baud*16)) / baud) ) & 0xf];

	write32(pdat->virt + UART_UBRDIV, baud_div_reg);
	write32(pdat->virt + UART_UFRACVAL, baud_divslot_reg);
	write32(pdat->virt + UART_ULCON, (dreg<<0 | sreg<<2 | preg<<3));
	return TRUE;
}

static bool_t uart_samsung_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_samsung_pdata_t * pdat = (struct uart_samsung_pdata_t *)uart->priv;

	if(baud)
		*baud = pdat->baud;
	if(data)
		*data = pdat->data;
	if(parity)
		*parity = pdat->parity;
	if(stop)
		*stop = pdat->stop;
	return TRUE;
}

static ssize_t uart_samsung_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_samsung_pdata_t * pdat = (struct uart_samsung_pdata_t *)uart->priv;
	ssize_t i;

	if((read32(pdat->virt + UART_UFCON) & (1 << 0)))
	{
		for(i = 0; i < count; i++)
		{
			if((read32(pdat->virt + UART_UFSTAT) & (0x1ff << 0)))
				buf[i] = read8(pdat->virt + UART_URXH);
			else
				break;
		}
	}
	else
	{
		for(i = 0; i < count; i++)
		{
			if((read32(pdat->virt + UART_UTRSTAT) & (1 << 0)))
				buf[i] = read8(pdat->virt + UART_URXH);
			else
				break;
		}
	}
	return i;
}

static ssize_t uart_samsung_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_samsung_pdata_t * pdat = (struct uart_samsung_pdata_t *)uart->priv;
	ssize_t i;

	if((read32(pdat->virt + UART_UFCON) & (1 << 0)))
	{
		for(i = 0; i < count; i++)
		{
			while((read32(pdat->virt + UART_UFSTAT) & (1 << 24)));
			write8(pdat->virt + UART_UTXH, buf[i]);
		}
	}
	else
	{
		for(i = 0; i < count; i++)
		{
			while(!(read32(pdat->virt + UART_UTRSTAT) & (1 << 1)));
			write8(pdat->virt + UART_UTXH, buf[i]);
		}
	}
	return i;
}

static struct device_t * uart_samsung_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_samsung_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct uart_samsung_pdata_t));
	if(!pdat)
		return NULL;

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->txd = dt_read_int(n, "txd-gpio", -1);
	pdat->txdcfg = dt_read_int(n, "txd-gpio-config", -1);
	pdat->rxd = dt_read_int(n, "rxd-gpio", -1);
	pdat->rxdcfg = dt_read_int(n, "rxd-gpio-config", -1);
	pdat->baud = dt_read_int(n, "baud-rates", 115200);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);

	uart->name = alloc_device_name(dt_read_name(n), -1);
	uart->set = uart_samsung_set;
	uart->get = uart_samsung_get;
	uart->read = uart_samsung_read;
	uart->write = uart_samsung_write;
	uart->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->txd >= 0)
	{
		if(pdat->txdcfg >= 0)
			gpio_set_cfg(pdat->txd, pdat->txdcfg);
		gpio_set_pull(pdat->txd, GPIO_PULL_UP);
	}
	if(pdat->rxd >= 0)
	{
		if(pdat->rxdcfg >= 0)
			gpio_set_cfg(pdat->rxd, pdat->rxdcfg);
		gpio_set_pull(pdat->rxd, GPIO_PULL_UP);
	}
/*	write32(pdat->virt + UART_UCON, 0x00000005);
	write32(pdat->virt + UART_UFCON, 0x00000777);
	write32(pdat->virt + UART_UMCON, 0x00000000);
	uart_samsung_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);
*/
	if(!(dev = register_uart(uart, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
		return NULL;
	}
	return dev;
}

static void uart_samsung_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_samsung_pdata_t * pdat = (struct uart_samsung_pdata_t *)uart->priv;

	if(uart)
	{
		unregister_uart(uart);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_samsung_suspend(struct device_t * dev)
{
}

static void uart_samsung_resume(struct device_t * dev)
{
}

static struct driver_t uart_samsung = {
	.name		= "uart-samsung",
	.probe		= uart_samsung_probe,
	.remove		= uart_samsung_remove,
	.suspend	= uart_samsung_suspend,
	.resume		= uart_samsung_resume,
};

static __init void uart_samsung_driver_init(void)
{
	register_driver(&uart_samsung);
}

static __exit void uart_samsung_driver_exit(void)
{
	unregister_driver(&uart_samsung);
}

driver_initcall(uart_samsung_driver_init);
driver_exitcall(uart_samsung_driver_exit);
