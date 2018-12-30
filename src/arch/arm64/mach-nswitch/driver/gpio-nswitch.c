/*
 * driver/gpio-nswitch.c
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
#include <gpio/gpio.h>

#define GPIO_BANK(x)			((x) >> 5)
#define GPIO_PORT(x)			(((x) >> 3) & 0x3)
#define GPIO_BIT(x)				((x) & 0x7)
#define GPIO_REG(x)				(GPIO_BANK(x) * 0x100 + GPIO_PORT(x) * 4)

#define GPIO_CNF(x)				(GPIO_REG(x) + 0x00)
#define GPIO_OE(x)				(GPIO_REG(x) + 0x10)
#define GPIO_OUT(x)				(GPIO_REG(x) + 0x20)
#define GPIO_IN(x)				(GPIO_REG(x) + 0x30)
#define GPIO_INT_STA(x)			(GPIO_REG(x) + 0x40)
#define GPIO_INT_ENB(x)			(GPIO_REG(x) + 0x50)
#define GPIO_INT_LVL(x)			(GPIO_REG(x) + 0x60)
#define GPIO_INT_CLR(x)			(GPIO_REG(x) + 0x70)
#define GPIO_DBC_CNT(x)			(GPIO_REG(x) + 0xf0)

#define GPIO_MSK_CNF(x)			(GPIO_REG(x) + 0x80 + 0x00)
#define GPIO_MSK_OE(x)			(GPIO_REG(x) + 0x80 + 0x10)
#define GPIO_MSK_OUT(x)			(GPIO_REG(x) + 0x80 + 0x20)
#define GPIO_MSK_DBC_EN(x)		(GPIO_REG(x) + 0x80 + 0x30)
#define GPIO_MSK_INT_STA(x)		(GPIO_REG(x) + 0x80 + 0x40)
#define GPIO_MSK_INT_ENB(x)		(GPIO_REG(x) + 0x80 + 0x50)
#define GPIO_MSK_INT_LVL(x)		(GPIO_REG(x) + 0x80 + 0x60)

struct gpio_nswitch_pdata_t
{
	virtual_addr_t virt;
	virtual_addr_t pinmux;
	int base;
	int ngpio;
	int oirq;
};

struct gpio_mux_desc_t {
	uint32_t reg;
	uint8_t mux_bit;
	uint8_t mux_width;
	uint8_t pupd_bit;
	uint8_t pupd_width;
	uint8_t drv_bit;
	uint8_t drv_width;
};

#define GPIOMUX(r, mux_b, mux_w, pupd_b, pupd_w, drv_b, drv_w)	\
	{															\
		.reg = r,												\
		.mux_bit = mux_b,										\
		.mux_width = mux_w,										\
		.pupd_bit = pupd_b,										\
		.pupd_width = pupd_w,									\
		.drv_bit = drv_b,										\
		.drv_width = drv_w,										\
	}

#define GPIOMUX_NULL()											\
	{															\
		.reg = 0x0000,											\
		.mux_bit = 0,											\
		.mux_width = 0,											\
		.pupd_bit = 0,											\
		.pupd_width = 0,										\
		.drv_bit = 0,											\
		.drv_width = 0,											\
	}

static const struct gpio_mux_desc_t descs[] = {
					/*      reg,    mux_b, mux_w, pupd_b, pupd_w, drv_b, drv_w */
	/* GPIOA0 */	GPIOMUX(0x0038, 0,     2,     2,      2,      0,    0),
	/* GPIOA1 */	GPIOMUX(0x003c, 0,     2,     2,      2,      0,    0),
	/* GPIOA2 */	GPIOMUX(0x0040, 0,     2,     2,      2,      0,    0),
	/* GPIOA3 */	GPIOMUX(0x0044, 0,     2,     2,      2,      0,    0),
	/* GPIOA4 */	GPIOMUX(0x0048, 0,     2,     2,      2,      0,    0),
	/* GPIOA5 */	GPIOMUX(0x004c, 0,     2,     2,      2,      0,    0),
	/* GPIOA6 */	GPIOMUX(0x0244, 0,     2,     2,      2,      0,    0),
	/* GPIOA7 */	GPIOMUX_NULL(),
	/* GPIOB0 */	GPIOMUX(0x0124, 0,     2,     2,      2,      13,   2),
	/* GPIOB1 */	GPIOMUX(0x0128, 0,     2,     2,      2,      13,   2),
	/* GPIOB2 */	GPIOMUX(0x012c, 0,     2,     2,      2,      13,   2),
	/* GPIOB3 */	GPIOMUX(0x0130, 0,     2,     2,      2,      13,   2),
	/* GPIOB4 */	GPIOMUX(0x0064, 0,     2,     2,      2,      13,   2),
	/* GPIOB5 */	GPIOMUX(0x0068, 0,     2,     2,      2,      13,   2),
	/* GPIOB6 */	GPIOMUX(0x006c, 0,     2,     2,      2,      13,   2),
	/* GPIOB7 */	GPIOMUX(0x0070, 0,     2,     2,      2,      13,   2),
	/* GPIOC0 */	GPIOMUX(0x0050, 0,     2,     2,      2,      13,   2),
	/* GPIOC1 */	GPIOMUX(0x0054, 0,     2,     2,      2,      13,   2),
	/* GPIOC2 */	GPIOMUX(0x0058, 0,     2,     2,      2,      13,   2),
	/* GPIOC3 */	GPIOMUX(0x005c, 0,     2,     2,      2,      13,   2),
	/* GPIOC4 */	GPIOMUX(0x0060, 0,     2,     2,      2,      13,   2),
	/* GPIOC5 */	GPIOMUX(0x0080, 0,     2,     2,      2,      13,   2),
	/* GPIOC6 */	GPIOMUX(0x0084, 0,     2,     2,      2,      13,   2),
	/* GPIOC7 */	GPIOMUX(0x0078, 0,     2,     2,      2,      13,   2),
	/* GPIOD0 */	GPIOMUX(0x007c, 0,     2,     2,      2,      13,   2),
	/* GPIOD1 */	GPIOMUX(0x0104, 0,     2,     2,      2,      0,    0),
	/* GPIOD2 */	GPIOMUX(0x0108, 0,     2,     2,      2,      0,    0),
	/* GPIOD3 */	GPIOMUX(0x010c, 0,     2,     2,      2,      0,    0),
	/* GPIOD4 */	GPIOMUX(0x0110, 0,     2,     2,      2,      0,    0),
	/* GPIOD5 */	GPIOMUX_NULL(),
	/* GPIOD6 */	GPIOMUX_NULL(),
	/* GPIOD7 */	GPIOMUX_NULL(),

	/* GPIOE0 */	GPIOMUX(0x00a4, 0,     2,     2,      2,      0,    0),
	/* GPIOE1 */	GPIOMUX(0x00a8, 0,     2,     2,      2,      0,    0),
	/* GPIOE2 */	GPIOMUX(0x00ac, 0,     2,     2,      2,      0,    0),
	/* GPIOE3 */	GPIOMUX(0x00b0, 0,     2,     2,      2,      0,    0),
	/* GPIOE4 */	GPIOMUX(0x00b4, 0,     2,     2,      2,      0,    0),
	/* GPIOE5 */	GPIOMUX(0x00b8, 0,     2,     2,      2,      0,    0),
	/* GPIOE6 */	GPIOMUX(0x0248, 0,     2,     2,      2,      0,    0),
	/* GPIOE7 */	GPIOMUX(0x024c, 0,     2,     2,      2,      0,    0),
	/* GPIOF0 */	GPIOMUX(0x00cc, 0,     2,     2,      2,      0,    0),
	/* GPIOF1 */	GPIOMUX(0x00d0, 0,     2,     2,      2,      0,    0),
	/* GPIOF2 */	GPIOMUX_NULL(),
	/* GPIOF3 */	GPIOMUX_NULL(),
	/* GPIOF4 */	GPIOMUX_NULL(),
	/* GPIOF5 */	GPIOMUX_NULL(),
	/* GPIOF6 */	GPIOMUX_NULL(),
	/* GPIOF7 */	GPIOMUX_NULL(),
	/* GPIOG0 */	GPIOMUX(0x00f4, 0,     2,     2,      2,      0,    0),
	/* GPIOG1 */	GPIOMUX(0x00f8, 0,     2,     2,      2,      0,    0),
	/* GPIOG2 */	GPIOMUX(0x00fc, 0,     2,     2,      2,      0,    0),
	/* GPIOG3 */	GPIOMUX(0x0100, 0,     2,     2,      2,      0,    0),
	/* GPIOG4 */	GPIOMUX_NULL(),
	/* GPIOG5 */	GPIOMUX_NULL(),
	/* GPIOG6 */	GPIOMUX_NULL(),
	/* GPIOG7 */	GPIOMUX_NULL(),
	/* GPIOH0 */	GPIOMUX(0x01b4, 0,     2,     2,      2,      0,    0),
	/* GPIOH1 */	GPIOMUX(0x01b8, 0,     2,     2,      2,      0,    0),
	/* GPIOH2 */	GPIOMUX(0x01bc, 0,     2,     2,      2,      0,    0),
	/* GPIOH3 */	GPIOMUX(0x01c0, 0,     2,     2,      2,      0,    0),
	/* GPIOH4 */	GPIOMUX(0x01c4, 0,     2,     2,      2,      0,    0),
	/* GPIOH5 */	GPIOMUX(0x01c8, 0,     2,     2,      2,      0,    0),
	/* GPIOH6 */	GPIOMUX(0x0250, 0,     2,     2,      2,      0,    0),
	/* GPIOH7 */	GPIOMUX(0x01cc, 0,     2,     2,      2,      0,    0),

	/* GPIOI0 */	GPIOMUX(0x01d0, 0,     2,     2,      2,      0,    0),
	/* GPIOI1 */	GPIOMUX(0x01d4, 0,     2,     2,      2,      0,    0),
	/* GPIOI2 */	GPIOMUX(0x01d8, 0,     2,     2,      2,      0,    0),
	/* GPIOI3 */	GPIOMUX(0x01dc, 0,     2,     2,      2,      0,    0),
	/* GPIOI4 */	GPIOMUX(0x0114, 0,     2,     2,      2,      0,    0),
	/* GPIOI5 */	GPIOMUX(0x0118, 0,     2,     2,      2,      0,    0),
	/* GPIOI6 */	GPIOMUX(0x011c, 0,     2,     2,      2,      0,    0),
	/* GPIOI7 */	GPIOMUX(0x0120, 0,     2,     2,      2,      0,    0),
	/* GPIOJ0 */	GPIOMUX(0x00bc, 0,     2,     2,      2,      0,    0),
	/* GPIOJ1 */	GPIOMUX(0x00c0, 0,     2,     2,      2,      0,    0),
	/* GPIOJ2 */	GPIOMUX(0x00c4, 0,     2,     2,      2,      0,    0),
	/* GPIOJ3 */	GPIOMUX(0x00c8, 0,     2,     2,      2,      0,    0),
	/* GPIOJ4 */	GPIOMUX(0x0144, 0,     2,     2,      2,      0,    0),
	/* GPIOJ5 */	GPIOMUX(0x0148, 0,     2,     2,      2,      0,    0),
	/* GPIOJ6 */	GPIOMUX(0x014c, 0,     2,     2,      2,      0,    0),
	/* GPIOJ7 */	GPIOMUX(0x0150, 0,     2,     2,      2,      0,    0),
	/* GPIOK0 */	GPIOMUX(0x0254, 0,     2,     2,      2,      13,   2),
	/* GPIOK1 */	GPIOMUX(0x0258, 0,     2,     2,      2,      13,   2),
	/* GPIOK2 */	GPIOMUX(0x025c, 0,     2,     2,      2,      13,   2),
	/* GPIOK3 */	GPIOMUX(0x0260, 0,     2,     2,      2,      13,   2),
	/* GPIOK4 */	GPIOMUX(0x0264, 0,     2,     2,      2,      13,   2),
	/* GPIOK5 */	GPIOMUX(0x0268, 0,     2,     2,      2,      13,   2),
	/* GPIOK6 */	GPIOMUX(0x026c, 0,     2,     2,      2,      13,   2),
	/* GPIOK7 */	GPIOMUX(0x0270, 0,     2,     2,      2,      13,   2),
	/* GPIOL0 */	GPIOMUX(0x0274, 0,     2,     2,      2,      13,   2),
	/* GPIOL1 */	GPIOMUX(0x0278, 0,     2,     2,      2,      13,   2),
	/* GPIOL2 */	GPIOMUX_NULL(),
	/* GPIOL3 */	GPIOMUX_NULL(),
	/* GPIOL4 */	GPIOMUX_NULL(),
	/* GPIOL5 */	GPIOMUX_NULL(),
	/* GPIOL6 */	GPIOMUX_NULL(),
	/* GPIOL7 */	GPIOMUX_NULL(),

	/* GPIOM0 */	GPIOMUX(0x0000, 0,     2,     2,      2,      13,   2),
	/* GPIOM1 */	GPIOMUX(0x0004, 0,     2,     2,      2,      13,   2),
	/* GPIOM2 */	GPIOMUX(0x0008, 0,     2,     2,      2,      13,   2),
	/* GPIOM3 */	GPIOMUX(0x000c, 0,     2,     2,      2,      13,   2),
	/* GPIOM4 */	GPIOMUX(0x0010, 0,     2,     2,      2,      13,   2),
	/* GPIOM5 */	GPIOMUX(0x0014, 0,     2,     2,      2,      13,   2),
	/* GPIOM6 */	GPIOMUX_NULL(),
	/* GPIOM7 */	GPIOMUX_NULL(),
	/* GPION0 */	GPIOMUX_NULL(),
	/* GPION1 */	GPIOMUX_NULL(),
	/* GPION2 */	GPIOMUX_NULL(),
	/* GPION3 */	GPIOMUX_NULL(),
	/* GPION4 */	GPIOMUX_NULL(),
	/* GPION5 */	GPIOMUX_NULL(),
	/* GPION6 */	GPIOMUX_NULL(),
	/* GPION7 */	GPIOMUX_NULL(),
	/* GPIOO0 */	GPIOMUX_NULL(),
	/* GPIOO1 */	GPIOMUX_NULL(),
	/* GPIOO2 */	GPIOMUX_NULL(),
	/* GPIOO3 */	GPIOMUX_NULL(),
	/* GPIOO4 */	GPIOMUX_NULL(),
	/* GPIOO5 */	GPIOMUX_NULL(),
	/* GPIOO6 */	GPIOMUX_NULL(),
	/* GPIOO7 */	GPIOMUX_NULL(),
	/* GPIOP0 */	GPIOMUX(0x001c, 0,     2,     2,      2,      13,   2),
	/* GPIOP1 */	GPIOMUX(0x0020, 0,     2,     2,      2,      13,   2),
	/* GPIOP2 */	GPIOMUX(0x0030, 0,     2,     2,      2,      13,   2),
	/* GPIOP3 */	GPIOMUX(0x002c, 0,     2,     2,      2,      13,   2),
	/* GPIOP4 */	GPIOMUX(0x0028, 0,     2,     2,      2,      13,   2),
	/* GPIOP5 */	GPIOMUX(0x0024, 0,     2,     2,      2,      13,   2),
	/* GPIOP6 */	GPIOMUX_NULL(),
	/* GPIOP7 */	GPIOMUX_NULL(),

	/* GPIOQ0 */	GPIOMUX_NULL(),
	/* GPIOQ1 */	GPIOMUX_NULL(),
	/* GPIOQ2 */	GPIOMUX_NULL(),
	/* GPIOQ3 */	GPIOMUX_NULL(),
	/* GPIOQ4 */	GPIOMUX_NULL(),
	/* GPIOQ5 */	GPIOMUX_NULL(),
	/* GPIOQ6 */	GPIOMUX_NULL(),
	/* GPIOQ7 */	GPIOMUX_NULL(),
	/* GPIOR0 */	GPIOMUX_NULL(),
	/* GPIOR1 */	GPIOMUX_NULL(),
	/* GPIOR2 */	GPIOMUX_NULL(),
	/* GPIOR3 */	GPIOMUX_NULL(),
	/* GPIOR4 */	GPIOMUX_NULL(),
	/* GPIOR5 */	GPIOMUX_NULL(),
	/* GPIOR6 */	GPIOMUX_NULL(),
	/* GPIOR7 */	GPIOMUX_NULL(),
	/* GPIOS0 */	GPIOMUX(0x0154, 0,     2,     2,      2,      0,    0),
	/* GPIOS1 */	GPIOMUX(0x0158, 0,     2,     2,      2,      0,    0),
	/* GPIOS2 */	GPIOMUX(0x00d4, 0,     2,     2,      2,      0,    0),
	/* GPIOS3 */	GPIOMUX(0x00d8, 0,     2,     2,      2,      0,    0),
	/* GPIOS4 */	GPIOMUX(0x01e0, 0,     2,     2,      2,      0,    0),
	/* GPIOS5 */	GPIOMUX(0x01e4, 0,     2,     2,      2,      0,    0),
	/* GPIOS6 */	GPIOMUX(0x01e8, 0,     2,     2,      2,      0,    0),
	/* GPIOS7 */	GPIOMUX(0x01ec, 0,     2,     2,      2,      0,    0),
	/* GPIOT0 */	GPIOMUX(0x01f0, 0,     2,     2,      2,      0,    0),
	/* GPIOT1 */	GPIOMUX(0x01f4, 0,     2,     2,      2,      0,    0),
	/* GPIOT2 */	GPIOMUX_NULL(),
	/* GPIOT3 */	GPIOMUX_NULL(),
	/* GPIOT4 */	GPIOMUX_NULL(),
	/* GPIOT5 */	GPIOMUX_NULL(),
	/* GPIOT6 */	GPIOMUX_NULL(),
	/* GPIOT7 */	GPIOMUX_NULL(),

	/* GPIOU0 */	GPIOMUX(0x00e4, 0,     2,     2,      2,      0,    0),
	/* GPIOU1 */	GPIOMUX(0x00e8, 0,     2,     2,      2,      0,    0),
	/* GPIOU2 */	GPIOMUX(0x00ec, 0,     2,     2,      2,      0,    0),
	/* GPIOU3 */	GPIOMUX(0x00f0, 0,     2,     2,      2,      0,    0),
	/* GPIOU4 */	GPIOMUX_NULL(),
	/* GPIOU5 */	GPIOMUX_NULL(),
	/* GPIOU6 */	GPIOMUX_NULL(),
	/* GPIOU7 */	GPIOMUX_NULL(),
	/* GPIOV0 */	GPIOMUX(0x01fc, 0,     2,     2,      2,      0,    0),
	/* GPIOV1 */	GPIOMUX(0x0200, 0,     2,     2,      2,      0,    0),
	/* GPIOV2 */	GPIOMUX(0x0204, 0,     2,     2,      2,      0,    0),
	/* GPIOV3 */	GPIOMUX(0x0208, 0,     2,     2,      2,      0,    0),
	/* GPIOV4 */	GPIOMUX(0x020c, 0,     2,     2,      2,      0,    0),
	/* GPIOV5 */	GPIOMUX(0x0210, 0,     2,     2,      2,      0,    0),
	/* GPIOV6 */	GPIOMUX(0x0214, 0,     2,     2,      2,      0,    0),
	/* GPIOV7 */	GPIOMUX(0x0218, 0,     2,     2,      2,      0,    0),
	/* GPIOW0 */	GPIOMUX_NULL(),
	/* GPIOW1 */	GPIOMUX_NULL(),
	/* GPIOW2 */	GPIOMUX_NULL(),
	/* GPIOW3 */	GPIOMUX_NULL(),
	/* GPIOW4 */	GPIOMUX_NULL(),
	/* GPIOW5 */	GPIOMUX_NULL(),
	/* GPIOW6 */	GPIOMUX_NULL(),
	/* GPIOW7 */	GPIOMUX_NULL(),
	/* GPIOX0 */	GPIOMUX(0x021c, 0,     2,     2,      2,      0,    0),
	/* GPIOX1 */	GPIOMUX(0x0220, 0,     2,     2,      2,      0,    0),
	/* GPIOX2 */	GPIOMUX(0x0224, 0,     2,     2,      2,      0,    0),
	/* GPIOX3 */	GPIOMUX(0x0228, 0,     2,     2,      2,      0,    0),
	/* GPIOX4 */	GPIOMUX(0x022c, 0,     2,     2,      2,      0,    0),
	/* GPIOX5 */	GPIOMUX(0x0230, 0,     2,     2,      2,      0,    0),
	/* GPIOX6 */	GPIOMUX(0x0234, 0,     2,     2,      2,      0,    0),
	/* GPIOX7 */	GPIOMUX(0x0238, 0,     2,     2,      2,      0,    0),

	/* GPIOY0 */	GPIOMUX(0x023c, 0,     2,     2,      2,      0,    0),
	/* GPIOY1 */	GPIOMUX(0x0240, 0,     2,     2,      2,      0,    0),
	/* GPIOY2 */	GPIOMUX(0x01f8, 0,     2,     2,      2,      0,    0),
	/* GPIOY3 */	GPIOMUX(0x00dc, 0,     2,     2,      2,      0,    0),
	/* GPIOY4 */	GPIOMUX(0x00e0, 0,     2,     2,      2,      0,    0),
	/* GPIOY5 */	GPIOMUX(0x0164, 0,     2,     2,      2,      0,    0),
	/* GPIOY6 */	GPIOMUX_NULL(),
	/* GPIOY7 */	GPIOMUX_NULL(),
	/* GPIOZ0 */	GPIOMUX(0x027c, 0,     2,     2,      2,      0,    0),
	/* GPIOZ1 */	GPIOMUX(0x0280, 0,     2,     2,      2,      0,    0),
	/* GPIOZ2 */	GPIOMUX(0x0284, 0,     2,     2,      2,      0,    0),
	/* GPIOZ3 */	GPIOMUX(0x0288, 0,     2,     2,      2,      0,    0),
	/* GPIOZ4 */	GPIOMUX(0x028c, 0,     2,     2,      2,      0,    0),
	/* GPIOZ5 */	GPIOMUX(0x0290, 0,     2,     2,      2,      0,    0),
	/* GPIOZ6 */	GPIOMUX_NULL(),
	/* GPIOZ7 */	GPIOMUX_NULL(),
	/* GPIOAA0 */	GPIOMUX(0x0134, 0,     2,     2,      2,      13,   2),
	/* GPIOAA1 */	GPIOMUX(0x0140, 0,     2,     2,      2,      13,   2),
	/* GPIOAA2 */	GPIOMUX(0x0138, 0,     2,     2,      2,      13,   2),
	/* GPIOAA3 */	GPIOMUX(0x013c, 0,     2,     2,      2,      13,   2),
	/* GPIOAA4 */	GPIOMUX_NULL(),
	/* GPIOAA5 */	GPIOMUX_NULL(),
	/* GPIOAA6 */	GPIOMUX_NULL(),
	/* GPIOAA7 */	GPIOMUX_NULL(),
	/* GPIOBB0 */	GPIOMUX(0x0180, 0,     2,     2,      2,      0,    0),
	/* GPIOBB1 */	GPIOMUX(0x0184, 0,     2,     2,      2,      0,    0),
	/* GPIOBB2 */	GPIOMUX(0x0188, 0,     2,     2,      2,      0,    0),
	/* GPIOBB3 */	GPIOMUX(0x018c, 0,     2,     2,      2,      0,    0),
	/* GPIOBB4 */	GPIOMUX(0x0190, 0,     2,     2,      2,      0,    0),
	/* GPIOBB5 */	GPIOMUX_NULL(),
	/* GPIOBB6 */	GPIOMUX_NULL(),
	/* GPIOBB7 */	GPIOMUX_NULL(),

	/* GPIOCC0 */	GPIOMUX(0x0198, 0,     2,     2,      2,      0,    0),
	/* GPIOCC1 */	GPIOMUX(0x019c, 0,     2,     2,      2,      0,    0),
	/* GPIOCC2 */	GPIOMUX(0x01a0, 0,     2,     2,      2,      0,    0),
	/* GPIOCC3 */	GPIOMUX(0x01a4, 0,     2,     2,      2,      0,    0),
	/* GPIOCC4 */	GPIOMUX(0x01a8, 0,     2,     2,      2,      0,    0),
	/* GPIOCC5 */	GPIOMUX(0x01ac, 0,     2,     2,      2,      0,    0),
	/* GPIOCC6 */	GPIOMUX(0x01b0, 0,     2,     2,      2,      0,    0),
	/* GPIOCC7 */	GPIOMUX(0x0194, 0,     2,     2,      2,      0,    0),
	/* GPIODD0 */	GPIOMUX(0x0074, 0,     2,     2,      2,      13,   2),
	/* GPIODD1 */	GPIOMUX_NULL(),
	/* GPIODD2 */	GPIOMUX_NULL(),
	/* GPIODD3 */	GPIOMUX_NULL(),
	/* GPIODD4 */	GPIOMUX_NULL(),
	/* GPIODD5 */	GPIOMUX_NULL(),
	/* GPIODD6 */	GPIOMUX_NULL(),
	/* GPIODD7 */	GPIOMUX_NULL(),
	/* GPIOEE0 */	GPIOMUX(0x0088, 0,     2,     2,      2,      13,   2),
	/* GPIOEE1 */	GPIOMUX(0x008c, 0,     2,     2,      2,      13,   2),
	/* GPIOEE2 */	GPIOMUX(0x0090, 0,     2,     2,      2,      13,   2),
	/* GPIOEE3 */	GPIOMUX(0x0094, 0,     2,     2,      2,      13,   2),
	/* GPIOEE4 */	GPIOMUX(0x0098, 0,     2,     2,      2,      13,   2),
	/* GPIOEE5 */	GPIOMUX(0x009c, 0,     2,     2,      2,      13,   2),
	/* GPIOEE6 */	GPIOMUX_NULL(),
	/* GPIOEE7 */	GPIOMUX_NULL(),
};

static void gpio_nswitch_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(cfg & (1 << 8))
	{
		val = 0x101 << GPIO_BIT(offset);
		write32(pdat->virt + GPIO_MSK_CNF(offset), val);
	}
	else
	{
		d = &descs[offset];
		val = read32(pdat->pinmux + d->reg);
		val &= ~(((1 << d->mux_width) - 1) << d->mux_bit);
		val |= (cfg & ((1 << d->mux_width) - 1)) << d->mux_bit;
		write32(pdat->pinmux + d->reg, val);

		val = 0x100 << GPIO_BIT(offset);
		write32(pdat->virt + GPIO_MSK_CNF(offset), val);
	}
}

static int gpio_nswitch_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t val, v = (1 << 8);

	if(offset >= chip->ngpio)
		return v;

	val = 1 << GPIO_BIT(offset);
	if(!(read32(pdat->virt + GPIO_CNF(offset)) & val))
	{
		d = &descs[offset];
		if(d->mux_width > 0)
			v = (read32(pdat->pinmux + d->reg) >> d->mux_bit) & ((1 << d->mux_width) - 1);
	}
	return v;
}

static void gpio_nswitch_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	d = &descs[offset];
	if(d->pupd_width > 0)
	{
		switch(pull)
		{
		case GPIO_PULL_UP:
			v = 0x2;
			break;
		case GPIO_PULL_DOWN:
			v = 0x1;
			break;
		case GPIO_PULL_NONE:
			v = 0x0;
			break;
		default:
			v = 0x0;
			break;
		}
		val = read32(pdat->pinmux + d->reg);
		val &= ~(((1 << d->pupd_width) - 1) << d->pupd_bit);
		val |= v << d->pupd_bit;
		write32(pdat->pinmux + d->reg, val);
	}
}

static enum gpio_pull_t gpio_nswitch_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t v;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	d = &descs[offset];
	if(d->pupd_width > 0)
	{
		v = (read32(pdat->pinmux + d->reg) >> d->pupd_bit) & ((1 << d->pupd_width) - 1);
		switch(v)
		{
		case 0:
			return GPIO_PULL_NONE;
		case 1:
			return GPIO_PULL_DOWN;
		case 2:
			return GPIO_PULL_UP;
		default:
			break;
		}
	}
	return GPIO_PULL_NONE;
}

static void gpio_nswitch_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	d = &descs[offset];
	if(d->drv_width > 0)
	{
		switch(drv)
		{
		case GPIO_DRV_WEAK:
			v = 0x0;
			break;
		case GPIO_DRV_WEAKER:
			v = 0x1;
			break;
		case GPIO_DRV_STRONGER:
			v = 0x2;
			break;
		case GPIO_DRV_STRONG:
			v = 0x3;
			break;
		default:
			v = 0x0;
			break;
		}
		val = read32(pdat->pinmux + d->reg);
		val &= ~(((1 << d->drv_width) - 1) << d->drv_bit);
		val |= v << d->drv_bit;
		write32(pdat->pinmux + d->reg, val);
	}
}

static enum gpio_drv_t gpio_nswitch_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	const struct gpio_mux_desc_t * d;
	u32_t v;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	d = &descs[offset];
	if(d->drv_width > 0)
	{
		v = (read32(pdat->pinmux + d->reg) >> d->drv_bit) & ((1 << d->drv_width) - 1);
		switch(v)
		{
		case 0:
			return GPIO_DRV_WEAK;
		case 1:
			return GPIO_DRV_WEAKER;
		case 2:
			return GPIO_DRV_STRONGER;
		case 3:
			return GPIO_DRV_STRONG;
		default:
			break;
		}
	}
	return GPIO_DRV_WEAK;
}

static void gpio_nswitch_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_nswitch_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_nswitch_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = 0x100 << GPIO_BIT(offset);
		write32(pdat->virt + GPIO_MSK_OE(offset), val);
		gpio_nswitch_set_cfg(chip, offset, (1 << 8));
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = 0x101 << GPIO_BIT(offset);
		write32(pdat->virt + GPIO_MSK_OE(offset), val);
		gpio_nswitch_set_cfg(chip, offset, (1 << 8));
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_nswitch_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	if(gpio_nswitch_get_cfg(chip, offset) & (1 << 8))
	{
		val = 1 << GPIO_BIT(offset);
		if(read32(pdat->virt + GPIO_OE(offset)) & val)
			return GPIO_DIRECTION_OUTPUT;
		else
			return GPIO_DIRECTION_INPUT;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_nswitch_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = 0x100 << GPIO_BIT(offset);
	if(value)
		val |= 1 << GPIO_BIT(offset);
	write32(pdat->virt + GPIO_MSK_OUT(offset), val);
}

static int gpio_nswitch_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = 1 << GPIO_BIT(offset);
	if(read32(pdat->virt + GPIO_OE(offset)) & val)
		return !!(read32(pdat->virt + GPIO_OUT(offset)) & val);
	return !!(read32(pdat->virt + GPIO_IN(offset)) & val);
}

static int gpio_nswitch_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_nswitch_pdata_t * pdat = (struct gpio_nswitch_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_nswitch_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_nswitch_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_nswitch_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->pinmux = phys_to_virt(0x70003000);
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_nswitch_set_cfg;
	chip->get_cfg = gpio_nswitch_get_cfg;
	chip->set_pull = gpio_nswitch_set_pull;
	chip->get_pull = gpio_nswitch_get_pull;
	chip->set_drv = gpio_nswitch_set_drv;
	chip->get_drv = gpio_nswitch_get_drv;
	chip->set_rate = gpio_nswitch_set_rate;
	chip->get_rate = gpio_nswitch_get_rate;
	chip->set_dir = gpio_nswitch_set_dir;
	chip->get_dir = gpio_nswitch_get_dir;
	chip->set_value = gpio_nswitch_set_value;
	chip->get_value = gpio_nswitch_get_value;
	chip->to_irq = gpio_nswitch_to_irq;
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

static void gpio_nswitch_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_nswitch_suspend(struct device_t * dev)
{
}

static void gpio_nswitch_resume(struct device_t * dev)
{
}

static struct driver_t gpio_nswitch = {
	.name		= "gpio-nswitch",
	.probe		= gpio_nswitch_probe,
	.remove		= gpio_nswitch_remove,
	.suspend	= gpio_nswitch_suspend,
	.resume		= gpio_nswitch_resume,
};

static __init void gpio_nswitch_driver_init(void)
{
	register_driver(&gpio_nswitch);
}

static __exit void gpio_nswitch_driver_exit(void)
{
	unregister_driver(&gpio_nswitch);
}

driver_initcall(gpio_nswitch_driver_init);
driver_exitcall(gpio_nswitch_driver_exit);
