/*
 * licheepi-zero.c
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
#include <mmu.h>

typedef unsigned int u32_t;

#define set_wbit(addr, v)	(*((volatile unsigned long  *)(addr)) |= (unsigned long)(v))
#define readl(addr)			(*((volatile unsigned long  *)(addr)))
#define writel(v, addr)		(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define SUNXI_UART0_BASE	0x01C28000
#define SUNXI_PIO_BASE		0x01C20800
#define AW_CCM_BASE			0x01c20000
#define AW_SRAMCTRL_BASE	0x01c00000

#define SUNXI_GPIO_A    0
#define SUNXI_GPIO_B    1
#define SUNXI_GPIO_C    2
#define SUNXI_GPIO_D    3
#define SUNXI_GPIO_E    4
#define SUNXI_GPIO_F    5
#define SUNXI_GPIO_G    6
#define SUNXI_GPIO_H    7
#define SUNXI_GPIO_I    8

struct sunxi_gpio {
	u32_t cfg[4];
	u32_t dat;
	u32_t drv[2];
	u32_t pull[2];
};

struct sunxi_gpio_reg {
	struct sunxi_gpio gpio_bank[10];
};

#define GPIO_BANK(pin)		((pin) >> 5)
#define GPIO_NUM(pin)		((pin) & 0x1F)

#define GPIO_CFG_INDEX(pin)	(((pin) & 0x1F) >> 3)
#define GPIO_CFG_OFFSET(pin)	((((pin) & 0x1F) & 0x7) << 2)

#define GPIO_PULL_INDEX(pin)	(((pin) & 0x1f) >> 4)
#define GPIO_PULL_OFFSET(pin)	((((pin) & 0x1f) & 0xf) << 1)

/* GPIO bank sizes */
#define SUNXI_GPIO_A_NR    (32)
#define SUNXI_GPIO_B_NR    (32)
#define SUNXI_GPIO_C_NR    (32)
#define SUNXI_GPIO_D_NR    (32)
#define SUNXI_GPIO_E_NR    (32)
#define SUNXI_GPIO_F_NR    (32)
#define SUNXI_GPIO_G_NR    (32)
#define SUNXI_GPIO_H_NR    (32)
#define SUNXI_GPIO_I_NR    (32)

#define SUNXI_GPIO_NEXT(__gpio) ((__gpio##_START) + (__gpio##_NR) + 0)

enum sunxi_gpio_number {
	SUNXI_GPIO_A_START = 0,
	SUNXI_GPIO_B_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_A),
	SUNXI_GPIO_C_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_B),
	SUNXI_GPIO_D_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_C),
	SUNXI_GPIO_E_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_D),
	SUNXI_GPIO_F_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_E),
	SUNXI_GPIO_G_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_F),
	SUNXI_GPIO_H_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_G),
	SUNXI_GPIO_I_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_H),
};

#define SUNXI_GPA(_nr)          (SUNXI_GPIO_A_START + (_nr))
#define SUNXI_GPB(_nr)          (SUNXI_GPIO_B_START + (_nr))
#define SUNXI_GPC(_nr)          (SUNXI_GPIO_C_START + (_nr))
#define SUNXI_GPD(_nr)          (SUNXI_GPIO_D_START + (_nr))
#define SUNXI_GPE(_nr)          (SUNXI_GPIO_E_START + (_nr))
#define SUNXI_GPF(_nr)          (SUNXI_GPIO_F_START + (_nr))
#define SUNXI_GPG(_nr)          (SUNXI_GPIO_G_START + (_nr))
#define SUNXI_GPH(_nr)          (SUNXI_GPIO_H_START + (_nr))
#define SUNXI_GPI(_nr)          (SUNXI_GPIO_I_START + (_nr))

#define SUNXI_GPIO_PULL_DISABLE (0)
#define SUNXI_GPIO_PULL_UP      (1)
#define SUNXI_GPIO_PULL_DOWN    (2)

static int sunxi_gpio_set_cfgpin(u32_t pin, u32_t val)
{
	u32_t cfg;
	u32_t bank = GPIO_BANK(pin);
	u32_t index = GPIO_CFG_INDEX(pin);
	u32_t offset = GPIO_CFG_OFFSET(pin);
	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];
	cfg = readl(&pio->cfg[0] + index);
	cfg &= ~(0xf << offset);
	cfg |= val << offset;
	writel(cfg, &pio->cfg[0] + index);
	return 0;
}

static int sunxi_gpio_set_pull(u32_t pin, u32_t val)
{
	u32_t cfg;
	u32_t bank = GPIO_BANK(pin);
	u32_t index = GPIO_PULL_INDEX(pin);
	u32_t offset = GPIO_PULL_OFFSET(pin);
	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];
	cfg = readl(&pio->pull[0] + index);
	cfg &= ~(0x3 << offset);
	cfg |= val << offset;
	writel(cfg, &pio->pull[0] + index);
	return 0;
}

#define CONFIG_CONS_INDEX	1
#define APB2_CFG		(AW_CCM_BASE + 0x058)
#define APB2_GATE		(AW_CCM_BASE + 0x06C)
#define APB2_RESET		(AW_CCM_BASE + 0x2D8)
#define APB2_GATE_UART_SHIFT	(16)
#define APB2_RESET_UART_SHIFT	(16)

#define UART0_RBR (SUNXI_UART0_BASE + 0x0)    /* receive buffer register */
#define UART0_THR (SUNXI_UART0_BASE + 0x0)    /* transmit holding register */
#define UART0_DLL (SUNXI_UART0_BASE + 0x0)    /* divisor latch low register */

#define UART0_DLH (SUNXI_UART0_BASE + 0x4)    /* divisor latch high register */
#define UART0_IER (SUNXI_UART0_BASE + 0x4)    /* interrupt enable reigster */

#define UART0_IIR (SUNXI_UART0_BASE + 0x8)    /* interrupt identity register */
#define UART0_FCR (SUNXI_UART0_BASE + 0x8)    /* fifo control register */

#define UART0_LCR (SUNXI_UART0_BASE + 0xc)    /* line control register */

#define UART0_LSR (SUNXI_UART0_BASE + 0x14)   /* line status register */

#define BAUD_115200    (0xD) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1       (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

void debug_init(void)
{
#if 0
	virtual_addr_t addr;
	u32_t val;

	/* Config GPIOB8 and GPIOB9 to txd0 and rxd0 */
	addr = 0x01c20824 + 0x04;
	val = read32(addr);
	val &= ~(0xf << ((8 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((8 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((9 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((9 & 0x7) << 2));
	write32(addr, val);

	/* Open the clock gate for uart0 */
	addr = 0x01c2006c;
	val = read32(addr);
	val |= 1 << 16;
	write32(addr, val);

	/* Deassert uart0 reset */
	addr = 0x01c202d8;
	val = read32(addr);
	val |= 1 << 16;
	write32(addr, val);

	/* Config uart0 to 115200-8-1-0 */
	addr = 0x01c28000;
	val = read32(addr + 0x0c);
	val |= (1 << 7);
	write32(addr + 0x0c, val);
	write32(addr + 0x00, 0xd & 0xff);
	write32(addr + 0x04, (0xd >> 8) & 0xff);

	val = read32(addr + 0x0c);
	val &= ~(1 << 7);
	write32(addr + 0x0c, val);
	val = read32(addr + 0x0c);
	val &= ~0x1f;
	val |= (0x3 << 0) | (0 << 2) | (0x0 << 3);
	write32(addr + 0x0c, val);


	/* select dll dlh */
//	writel(0x80, UART0_LCR);
	/* set baudrate */

//	writel(BAUD_115200, UART0_DLL);
//	writel(0, UART0_DLH);
	/* set line control */
//	writel(LC_8_N_1, UART0_LCR);
#endif
	virtual_addr_t addr;
	u32_t val;

	/* Config GPIOB8 and GPIOB9 to txd0 and rxd0 */
	addr = 0x01c20824 + 0x04;
	val = read32(addr);
	val &= ~(0xf << ((8 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((8 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((9 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((9 & 0x7) << 2));
	write32(addr, val);

	/* Open the clock gate for uart0 */
	addr = 0x01c2006c;
	val = read32(addr);
	val |= 1 << 16;
	write32(addr, val);

	/* Deassert uart0 reset */
	addr = 0x01c202d8;
	val = read32(addr);
	val |= 1 << 16;
	write32(addr, val);

	/* Config uart0 to 115200-8-1-0 */
	addr = 0x01c28000;
	write32(addr + 0x88, (1 << 0) | (1 << 1) | (1 << 2));
	write32(addr + 0x04, 0x0);
	write32(addr + 0x10, 0x0);
	write32(addr + 0x98, 0x1);
	write32(addr + 0x9c, 0x3);
	write32(addr + 0xa0, 0x1);
	val = read32(addr + 0x0c);
	val |= (1 << 7);
	write32(addr + 0x0c, val);
	write32(addr + 0x00, 0xd & 0xff);
	write32(addr + 0x04, (0xd >> 8) & 0xff);
	val = read32(addr + 0x0c);
	val &= ~(1 << 7);
	write32(addr + 0x0c, val);
	val = read32(addr + 0x0c);
	val &= ~0x1f;
	val |= (0x3 << 0) | (0 << 2) | (0x0 << 3);
	write32(addr + 0x0c, val);
}

void debug_putc(char c)
{
	while (!(readl(UART0_LSR) & (1 << 6)));
	writel(c, UART0_THR);
}

static const struct mmap_t mach_map[] = {
	{"ram",  0x40000000, 0x40000000, SZ_16M, MAP_TYPE_CB},
	{"dma",  0x41000000, 0x41000000, SZ_16M, MAP_TYPE_NCNB},
	{"heap", 0x42000000, 0x42000000, SZ_32M, MAP_TYPE_CB},
	{ 0 },
};

static u32_t sram_read_id(virtual_addr_t virt)
{
	u32_t id;

	write32(virt, read32(virt) | (1 << 15));
	id = read32(virt) >> 16;
	write32(virt, read32(virt) & ~(1 << 15));
	return id;
}

static int mach_detect(struct machine_t * mach)
{
	u32_t id = sram_read_id(phys_to_virt(0x01c00024));

	if(id == 0x1681)
		return 1;
	return 0;
}

static void mach_memmap(struct machine_t * mach)
{
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x01c28000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[32 + 3 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(0x01c23800);
	u32_t sid0, sid1, sid2, sid3;

	sid0 = read32(virt + 0 * 4);
	sid1 = read32(virt + 1 * 4);
	sid2 = read32(virt + 2 * 4);
	sid3 = read32(virt + 3 * 4);
	snprintf(uniqueid, sizeof(uniqueid), "%08x:%08x:%08x:%08x",sid0, sid1, sid2, sid3);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t licheepi_zero = {
	.name 		= "licheepi-zero",
	.desc 		= "Lichee Pi Zero Based On Allwinner V3S SOC",
	.map		= mach_map,
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void licheepi_zero_machine_init(void)
{
	register_machine(&licheepi_zero);
}

static __exit void licheepi_zero_machine_exit(void)
{
	unregister_machine(&licheepi_zero);
}

machine_initcall(licheepi_zero_machine_init);
machine_exitcall(licheepi_zero_machine_exit);
