/*
 * mach-x6410.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <s3c6410-cp15.h>
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-wdg.h>


extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __romdisk_start[];
extern u8_t __romdisk_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __heap_start[];
extern u8_t __heap_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

static void mach_init(void)
{
	/* gpk0 high level for power lock */
	writel(S3C6410_GPKCON0, (readl(S3C6410_GPKCON0) & ~(0xf<<0)) | (0x1<<0));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<0)) | (0x1<<0));
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_halt(void)
{
	/* gpk0 low level for power down */
	writel(S3C6410_GPKCON0, (readl(S3C6410_GPKCON0) & ~(0xf<<0)) | (0x1<<0));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<0)) | (0x0<<0));

	return TRUE;
}

static bool_t mach_reset(void)
{
	/* disable watchdog */
	writel(S3C6410_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S3C6410_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S3C6410_WTCON, 0x0021);

	return TRUE;
}

static enum mode_t mach_getmode(void)
{
	u32_t gpn;

	/* set gpn5 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<10)) | (0x0<<10));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<10)) | (0x2<<10));

	/* gpn5 with key down */
	gpn = readl(S3C6410_GPNDAT) & 0x20;
	if(gpn != 0x20)
		return MODE_MENU;

	return MODE_MENU;
}

static bool_t mach_batinfo(struct battery_info * info)
{
	if(!info)
		return FALSE;

	info->charging = FALSE;
	info->voltage = 3700;
	info->charge_current = 0;
	info->discharge_current = 300;
	info->temperature = 200;
	info->capacity = 3600;
	info->internal_resistance = 100;
	info->level = 100;

	return TRUE;
}

static bool_t mach_cleanup(void)
{
	/* disable irq */
	irq_disable();

	/* disable fiq */
	fiq_disable();

	/* disable icache */
	icache_disable();

	/* disable dcache */
	dcache_disable();

	/* disable mmu */
	mmu_disable();

	/* disable vic */
	vic_disable();

	return TRUE;
}

static bool_t mach_authentication(void)
{
	return TRUE;
}

/*
 * A portable machine interface.
 */
static struct machine x6410 = {
	.info = {
		.board_name 		= "x6410",
		.board_desc 		= "the 9tripod's development board",
		.board_id			= "0",

		.cpu_name			= "s3c6410x",
		.cpu_desc			= "based on arm11 by samsung",
		.cpu_id				= "0x410fb760",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x50000000,
				.end		= 0x50000000 + SZ_128M - 1,
			},

			[1] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 12*1000*1000,
	},

	.link = {
		.text_start			= (const ptrdiff_t)__text_start,
		.text_end			= (const ptrdiff_t)__text_end,

		.romdisk_start		= (const ptrdiff_t)__romdisk_start,
		.romdisk_end		= (const ptrdiff_t)__romdisk_end,

		.data_shadow_start	= (const ptrdiff_t)__data_shadow_start,
		.data_shadow_end	= (const ptrdiff_t)__data_shadow_end,

		.data_start			= (const ptrdiff_t)__data_start,
		.data_end			= (const ptrdiff_t)__data_end,

		.bss_start			= (const ptrdiff_t)__bss_start,
		.bss_end			= (const ptrdiff_t)__bss_end,

		.heap_start			= (const ptrdiff_t)__heap_start,
		.heap_end			= (const ptrdiff_t)__heap_end,

		.stack_start		= (const ptrdiff_t)__stack_start,
		.stack_end			= (const ptrdiff_t)__stack_end,
	},

	.pm = {
		.init 				= mach_init,
		.sleep				= mach_sleep,
		.halt				= mach_halt,
		.reset				= mach_reset,
	},

	.misc = {
		.getmode			= mach_getmode,
		.batinfo			= mach_batinfo,
		.cleanup			= mach_cleanup,
		.authentication		= mach_authentication,
	},

	.priv					= NULL,
};

static __init void mach_x6410_init(void)
{
	if(!register_machine(&x6410))
		LOG_E("failed to register machine 'x6410'");
}
arch_initcall(mach_x6410_init);
