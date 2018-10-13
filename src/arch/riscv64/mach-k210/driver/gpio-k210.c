/*
 * driver/gpio-k210.c
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
#include <gpio/gpio.h>
#include <k210/reg-gpiohs.h>
#include <k210/reg-gpio.h>
#include <k210-gpio.h>

enum {
	FPIOA_IO_CFG	= 0x00,
	FPIOA_TIE_EN	= 0xc0,
	FPIOA_TIE_VAL	= 0xe0,
};

struct gpio_k210_pdata_t
{
	virtual_addr_t virtfpioa;
	virtual_addr_t virtgpiohs;
	virtual_addr_t virtgpio;
	int base;
	int ngpio;
	int oirq;
};

#define FPIOA(ch_sel, ds, oe_en, oe_inv, do_sel, do_inv, pu, pd, spu, sl, ie_en, ie_inv, di_inv, st, tie_en, tie_val, pad_di) \
	(uint32_t)( (ch_sel << 0) | (ds << 8) | (oe_en << 12) | (oe_inv << 13) | (do_sel << 14) | (do_inv << 15) | (pu << 16) \
	| (pd << 17) | (spu << 18) | (sl << 19) | (ie_en << 20) | (ie_inv << 21) | (di_inv << 22) | (st << 23) | (tie_en << 24) \
	| (tie_val << 25) | (0 << 26) | (pad_di << 31) )

static const uint32_t fpioa_cfg[256] = {
	/*    CH_SEL,             DS,  OE_EN, OE_INV, DO_SEL, DO_INV, PU, PD, SPU, SL, IE_EN, IE_INV, DI_INV, ST, TIE_EN, TIE_VAL, PAD_DI */
	FPIOA(CFG_JTAG_TCLK,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_JTAG_TDI,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_JTAG_TMS,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_JTAG_TDO,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI0_D0,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D1,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D2,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D3,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D4,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D5,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D6,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_D7,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI0_SS0,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI0_SS1,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI0_SS2,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI0_SS3,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI0_ARB,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  1,      1,       0),
	FPIOA(CFG_SPI0_SCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UARTHS_RX,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UARTHS_TX,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV6,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_RESV7,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CLK_SPI1,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CLK_I2C1,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_GPIOHS0,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS1,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS2,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS3,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS4,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS5,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS6,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS7,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS8,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS9,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS10,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS11,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS12,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS13,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS14,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS15,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS16,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS17,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS18,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS19,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS20,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS21,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS22,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS23,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS24,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS25,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS26,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS27,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS28,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS29,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS30,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIOHS31,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO0,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO1,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO2,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO3,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO4,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO5,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO6,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_GPIO7,          0xf, 1,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_RX,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_TX,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_RX,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_TX,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_RX,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_TX,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI1_D0,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D1,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D2,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D3,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D4,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D5,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D6,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_D7,        0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI1_SS0,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI1_SS1,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI1_SS2,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI1_SS3,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI1_ARB,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  1,      1,       0),
	FPIOA(CFG_SPI1_SCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_SPI_SLAVE_D0,   0xf, 1,     1,      0,      0,      0,  0,  0,   0,  1,     1,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI_SLAVE_SS,   0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_SPI_SLAVE_SCLK, 0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S0_MCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_SCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_WS,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_IN_D0,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S0_IN_D1,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S0_IN_D2,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S0_IN_D3,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S0_OUT_D0,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_OUT_D1,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_OUT_D2,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S0_OUT_D3,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_MCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_SCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_WS,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_IN_D0,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S1_IN_D1,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S1_IN_D2,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S1_IN_D3,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S1_OUT_D0,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_OUT_D1,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_OUT_D2,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S1_OUT_D3,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_MCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_SCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_WS,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_IN_D0,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S2_IN_D1,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S2_IN_D2,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S2_IN_D3,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2S2_OUT_D0,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_OUT_D1,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_OUT_D2,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2S2_OUT_D3,    0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV0,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV1,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV2,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV3,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV4,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_RESV5,          0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_I2C0_SCLK,      0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2C0_SDA,       0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2C1_SCLK,      0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2C1_SDA,       0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2C2_SCLK,      0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_I2C2_SDA,       0x0, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_XCLK,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CMOS_RST,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CMOS_PWDN,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CMOS_VSYNC,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_HREF,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_PCLK,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D0,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D1,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D2,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D3,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D4,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D5,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D6,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CMOS_D7,        0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_SCCB_SCLK,      0x0, 1,     1,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_SCCB_SDA,       0x0, 1,     1,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_CTS,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_DSR,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_DCD,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_RI,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_SIR_IN,   0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART1_DTR,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_RTS,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_OUT2,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_OUT1,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_SIR_OUT,  0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_BAUD,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_RE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_DE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART1_RS485_EN, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_CTS,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_DSR,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_DCD,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_RI,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_SIR_IN,   0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART2_DTR,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_RTS,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_OUT2,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_OUT1,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_SIR_OUT,  0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_BAUD,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_RE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_DE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART2_RS485_EN, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_CTS,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_DSR,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_DCD,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_RI,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_SIR_IN,   0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_UART3_DTR,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_RTS,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_OUT2,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_OUT1,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_SIR_OUT,  0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_BAUD,     0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_RE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_DE,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_UART3_RS485_EN, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER0_TOGGLE1, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER0_TOGGLE2, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER0_TOGGLE3, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER0_TOGGLE4, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER1_TOGGLE1, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER1_TOGGLE2, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER1_TOGGLE3, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER1_TOGGLE4, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER2_TOGGLE1, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER2_TOGGLE2, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER2_TOGGLE3, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_TIMER2_TOGGLE4, 0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CLK_SPI2,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_CLK_I2C2,       0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL0,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL1,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL2,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL3,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL4,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL5,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL6,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL7,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL8,      0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL9,      0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL10,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL11,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL12,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL13,     0x0, 0,     0,      0,      0,      1,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL14,     0xf, 1,     0,      0,      0,      1,  0,  0,   1,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL15,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL16,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_INTERNAL17,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_CONSTANT,       0x0, 0,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_INTERNAL18,     0x0, 0,     0,      0,      0,      0,  0,  0,   0,  1,     0,      0,      1,  0,      0,       0),
	FPIOA(CFG_DEBUG0,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG1,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG2,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG3,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG4,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG5,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG6,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG7,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG8,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG9,         0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG10,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG11,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG12,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG13,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG14,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG15,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG16,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG17,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG18,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG19,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG20,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG21,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG22,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG23,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG24,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG25,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG26,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG27,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG28,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG29,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG30,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
	FPIOA(CFG_DEBUG31,        0xf, 1,     0,      0,      0,      0,  0,  0,   0,  0,     0,      0,      0,  0,      0,       0),
};

static void fpioa_set_cfg(struct gpio_k210_pdata_t * pdat, int offset, int cfg)
{
	virtual_addr_t addr;
	u32_t val;

	addr = pdat->virtfpioa + FPIOA_TIE_VAL + ((cfg >> 5) << 2);
	val = read32(addr);
	if(fpioa_cfg[cfg] & (1 << 25))
		val |= (0x1 << (cfg & 0x1f));
	else
		val &= ~(0x1 << (cfg & 0x1f));
	write32(addr, val);

	addr = pdat->virtfpioa + FPIOA_TIE_EN + ((cfg >> 5) << 2);
	val = read32(addr);
	if(fpioa_cfg[cfg] & (1 << 24))
		val |= (0x1 << (cfg & 0x1f));
	else
		val &= ~(0x1 << (cfg & 0x1f));
	write32(addr, val);

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	val = fpioa_cfg[cfg] & ~((0x3f << 24) | (0xff << 0));
	val |= (cfg << 0) & 0xff;
	write32(addr, val);
}

static void gpio_k210_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;
	int i;

	if(offset >= chip->ngpio)
		return;

	if((cfg < 0) || (cfg > 0xff))
		return;

	if(cfg != CFG_RESV0)
	{
		for(i = 0; i < chip->ngpio; i++)
		{
			addr = pdat->virtfpioa + FPIOA_IO_CFG + (i << 2);
			val = (read32(addr) >> 0) & 0xff;
			if((val == cfg) && (i != offset))
				fpioa_set_cfg(pdat, i, CFG_RESV0);
		}
	}
	fpioa_set_cfg(pdat, offset, cfg);
}

static int gpio_k210_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	val = (read32(addr) >> 0) & 0xff;
	return val;
}

static void gpio_k210_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		v = 0x1;
		break;
	case GPIO_PULL_DOWN:
		v = 0x2;
		break;
	case GPIO_PULL_NONE:
		v = 0x0;
		break;
	default:
		v = 0x0;
		break;
	}
	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	val = read32(addr);
	val &= ~(0x3 << 16);
	val |= (v << 16);
	write32(addr, val);
}

static enum gpio_pull_t gpio_k210_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	v = (read32(addr) >> 16) & 0x3;
	switch(v)
	{
	case 0:
		return GPIO_PULL_NONE;
	case 1:
		return GPIO_PULL_UP;
	case 2:
		return GPIO_PULL_DOWN;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void gpio_k210_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_WEAK:
		v = 0x0;
		break;
	case GPIO_DRV_WEAKER:
		v = 0x5;
		break;
	case GPIO_DRV_STRONGER:
		v = 0xa;
		break;
	case GPIO_DRV_STRONG:
		v = 0xf;
		break;
	default:
		v = 0x0;
		break;
	}
	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	val = read32(addr);
	val &= ~(0xf << 8);
	val |= (v << 8);
	write32(addr, val);
}

static enum gpio_drv_t gpio_k210_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	v = (read32(addr) >> 8) & 0xf;
	switch(v)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return GPIO_DRV_WEAK;
	case 4:
	case 5:
	case 6:
	case 7:
		return GPIO_DRV_WEAKER;
	case 8:
	case 9:
	case 10:
	case 11:
		return GPIO_DRV_STRONGER;
	case 12:
	case 13:
	case 14:
	case 15:
		return GPIO_DRV_STRONG;
	default:
		break;
	}
	return GPIO_DRV_WEAK;
}

static void gpio_k210_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	val = read32(addr);
	if(rate == GPIO_RATE_FAST)
		val |= (0x1 << 19);
	else
		val &= ~(0x1 << 19);
	write32(addr, val);
}

static enum gpio_rate_t gpio_k210_get_rate(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;

	if(offset >= chip->ngpio)
		return GPIO_RATE_SLOW;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	if((read32(addr) >> 19) & 0x1)
		return GPIO_RATE_FAST;
	return GPIO_RATE_SLOW;
}

static void gpio_k210_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t cfg, val;

	if(offset >= chip->ngpio)
		return;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	cfg = (read32(addr) >> 0) & 0xff;

	if((cfg >= CFG_GPIOHS0) && (cfg <= CFG_GPIOHS31))
	{
		switch(dir)
		{
		case GPIO_DIRECTION_INPUT:
			val = read32(pdat->virtgpiohs + GPIOHS_OUTPUT_EN);
			val &= ~(1 << (cfg - CFG_GPIOHS0));
			write32(pdat->virtgpiohs + GPIOHS_OUTPUT_EN, val);
			val = read32(pdat->virtgpiohs + GPIOHS_INPUT_EN);
			val |= (1 << (cfg - CFG_GPIOHS0));
			write32(pdat->virtgpiohs + GPIOHS_INPUT_EN, val);
			break;
		case GPIO_DIRECTION_OUTPUT:
			val = read32(pdat->virtgpiohs + GPIOHS_INPUT_EN);
			val &= ~(1 << (cfg - CFG_GPIOHS0));
			write32(pdat->virtgpiohs + GPIOHS_INPUT_EN, val);
			val = read32(pdat->virtgpiohs + GPIOHS_OUTPUT_EN);
			val |= (1 << (cfg - CFG_GPIOHS0));
			write32(pdat->virtgpiohs + GPIOHS_OUTPUT_EN, val);
			break;
		default:
			break;
		}
	}
	else if((cfg >= CFG_GPIO0) && (cfg <= CFG_GPIO7))
	{
		switch(dir)
		{
		case GPIO_DIRECTION_INPUT:
			val = read32(pdat->virtgpio + GPIO_DIRECTION);
			val &= ~(1 << (cfg - CFG_GPIO0));
			write32(pdat->virtgpio + GPIO_DIRECTION, val);
			break;
		case GPIO_DIRECTION_OUTPUT:
			val = read32(pdat->virtgpio + GPIO_DIRECTION);
			val |= (1 << (cfg - CFG_GPIO0));
			write32(pdat->virtgpio + GPIO_DIRECTION, val);
			break;
		default:
			break;
		}
	}
}

static enum gpio_direction_t gpio_k210_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t cfg, val;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	cfg = (read32(addr) >> 0) & 0xff;

	if((cfg >= CFG_GPIOHS0) && (cfg <= CFG_GPIOHS31))
	{
		val = read32(pdat->virtgpiohs + GPIOHS_OUTPUT_EN);
		if(val & (1 << (cfg - CFG_GPIOHS0)))
			return GPIO_DIRECTION_OUTPUT;
		val = read32(pdat->virtgpiohs + GPIOHS_INPUT_EN);
		if(val & (1 << (cfg - CFG_GPIOHS0)))
			return GPIO_DIRECTION_INPUT;
	}
	else if((cfg >= CFG_GPIO0) && (cfg <= CFG_GPIO7))
	{
		val = read32(pdat->virtgpio + GPIO_DIRECTION);
		return (val & (1 << (cfg - CFG_GPIO0))) ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_k210_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t cfg, val;

	if(offset >= chip->ngpio)
		return;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	cfg = (read32(addr) >> 0) & 0xff;

	if((cfg >= CFG_GPIOHS0) && (cfg <= CFG_GPIOHS31))
	{
		val = read32(pdat->virtgpiohs + GPIOHS_OUTPUT_VAL);
		if(value)
			val |= (1 << (cfg - CFG_GPIOHS0));
		else
			val &= ~(1 << (cfg - CFG_GPIOHS0));
		write32(pdat->virtgpiohs + GPIOHS_OUTPUT_VAL, val);
	}
	else if((cfg >= CFG_GPIO0) && (cfg <= CFG_GPIO7))
	{
		val = read32(pdat->virtgpio + GPIO_DATA_OUTPUT);
		if(value)
			val |= (1 << (cfg - CFG_GPIO0));
		else
			val &= ~(1 << (cfg - CFG_GPIO0));
		write32(pdat->virtgpio + GPIO_DATA_OUTPUT, val);
	}
}

static int gpio_k210_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t cfg, val;

	if(offset >= chip->ngpio)
		return 0;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	cfg = (read32(addr) >> 0) & 0xff;

	if((cfg >= CFG_GPIOHS0) && (cfg <= CFG_GPIOHS31))
	{
		val = read32(pdat->virtgpiohs + GPIOHS_INPUT_VAL);
		return (val & (1 << (cfg - CFG_GPIOHS0))) ? 1 : 0;
	}
	else if((cfg >= CFG_GPIO0) && (cfg <= CFG_GPIO7))
	{
		val = read32(pdat->virtgpio + GPIO_DATA_INPUT);
		return (val & (1 << (cfg - CFG_GPIO0))) ? 1 : 0;
	}
	return 0;
}

static int gpio_k210_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_k210_pdata_t * pdat = (struct gpio_k210_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t cfg;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;

	addr = pdat->virtfpioa + FPIOA_IO_CFG + (offset << 2);
	cfg = (read32(addr) >> 0) & 0xff;

	if((cfg >= CFG_GPIOHS0) && (cfg <= CFG_GPIO7))
		return pdat->oirq + (cfg - CFG_GPIOHS0);
	return -1;
}

static struct device_t * gpio_k210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_k210_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_k210_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtfpioa = virt;
	pdat->virtgpiohs = phys_to_virt(K210_GPIOHS_BASE);
	pdat->virtgpio = phys_to_virt(K210_GPIO_BASE);
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_k210_set_cfg;
	chip->get_cfg = gpio_k210_get_cfg;
	chip->set_pull = gpio_k210_set_pull;
	chip->get_pull = gpio_k210_get_pull;
	chip->set_drv = gpio_k210_set_drv;
	chip->get_drv = gpio_k210_get_drv;
	chip->set_rate = gpio_k210_set_rate;
	chip->get_rate = gpio_k210_get_rate;
	chip->set_dir = gpio_k210_set_dir;
	chip->get_dir = gpio_k210_get_dir;
	chip->set_value = gpio_k210_set_value;
	chip->get_value = gpio_k210_get_value;
	chip->to_irq = gpio_k210_to_irq;
	chip->priv = pdat;

	if(!register_gpiochip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void gpio_k210_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_k210_suspend(struct device_t * dev)
{
}

static void gpio_k210_resume(struct device_t * dev)
{
}

static struct driver_t gpio_k210 = {
	.name		= "gpio-k210",
	.probe		= gpio_k210_probe,
	.remove		= gpio_k210_remove,
	.suspend	= gpio_k210_suspend,
	.resume		= gpio_k210_resume,
};

static __init void gpio_k210_driver_init(void)
{
	register_driver(&gpio_k210);
}

static __exit void gpio_k210_driver_exit(void)
{
	unregister_driver(&gpio_k210);
}

driver_initcall(gpio_k210_driver_init);
driver_exitcall(gpio_k210_driver_exit);
