/*
 * mach-sbc2410x.c
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
#include <s3c2410/reg-wdg.h>

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

}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_halt(void)
{
	return TRUE;
}

static bool_t mach_reset(void)
{
	/* disable watchdog */
	writel(S3C2410_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S3C2410_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S3C2410_WTCON, 0x0021);

	return TRUE;
}

static enum mode_t mach_getmode(void)
{
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
	u32_t old, tmp;

	/* disable all interrupt */
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (tmp)
			     :
			     : "memory");

	/* disable I-D cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
	tmp &= ~(1<<12 | 1<<2);
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));

	/* flush I/D cache */
	tmp = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (tmp));

	return TRUE;
}

static bool_t mach_authentication(void)
{
	return TRUE;
}

/*
 * A portable machine interface.
 */
static struct machine sbc2410x = {
	.info = {
		.board_name 		= "sbc2410x",
		.board_desc 		= "frendly arm's single board computer",
		.board_id			= "0",

		.cpu_name			= "s3c2410a",
		.cpu_desc			= "based on arm920t by samsung",
		.cpu_id				= "0x32410002",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x30000000,
				.end		= 0x30000000 + SZ_64M - 1,
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

static __init void mach_sbc2410x_init(void)
{
	if(!register_machine(&sbc2410x))
		LOG_E("failed to register machine 'sbc2410x'");
}
arch_initcall(mach_sbc2410x_init);
