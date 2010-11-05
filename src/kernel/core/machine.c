/*
 * kernel/core/machine.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <macros.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <vsprintf.h>
#include <mode.h>
#include <time/tick.h>
#include <time/timer.h>
#include <xboot/log.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>

/*
 * must define the __machine struct for portable
 */
static struct machine * __machine = 0;

/*
 * get machine
 */
struct machine * get_machine(void)
{
	return __machine;
}

/*
 * standby function
 */
x_bool standby(void)
{
	if(__machine && __machine->pm.standby)
		return __machine->pm.standby();
	return FALSE;
}

/*
 * resume function
 */
x_bool resume(void)
{
	if(__machine && __machine->pm.resume)
		return __machine->pm.resume();
	return FALSE;
}

/*
 * halt function
 */
x_bool halt(void)
{
	if(__machine && __machine->pm.halt)
		return __machine->pm.halt();

	return FALSE;
}

/*
 * reset function
 */
x_bool reset(void)
{
	if(__machine && __machine->pm.reset)
		return __machine->pm.reset();

	return FALSE;
}

/*
 * clean up system before running os
 */
x_bool cleanup(void)
{
	if(__machine && __machine->misc.cleanup)
		return __machine->misc.cleanup();

	return FALSE;
}

/*
 * register machine.
 */
x_bool machine_register(struct machine * mach)
{
	if(mach)
	{
		__machine = mach;

		/*
		 * set xboot's default running mode.
		 */
		if(__machine->cfg.mode)
			xboot_set_mode(__machine->cfg.mode);

		/* machine initialize */
		if(__machine->pm.init)
			__machine->pm.init();

		/* will be changed to menu mode */
		if(__machine->misc.menumode)
		{
			if(__machine->misc.menumode())
				xboot_set_mode(MODE_MENU);
		}

		return TRUE;
	}
	else
	{
		__machine = 0;
		return FALSE;
	}
}

/*
 * anti piracy timer.
 */
static struct timer_list anti_piracy_timer;

/*
 * xtime's timer function.
 */
static void anti_piracy_timer_function(x_u32 data)
{
	if(!__machine || !__machine->misc.genuine)
		return;

	if(!__machine->misc.genuine())
	{
		/* system halt */
		halt();

		/* system reset */
		reset();

		/* dead loop */
		while(1);
	}

	/* mod timer for four second */
	mod_timer(&anti_piracy_timer, jiffies + get_system_hz() * 4);
}

/*
 * do anti piracy.
 */
void do_anti_piracy(void)
{
	if(!__machine || !__machine->misc.genuine)
		return;

	if(__machine->misc.genuine())
	{
		LOG_I("start anti piracy");

		/* setup timer for anti piracy */
		setup_timer(&anti_piracy_timer, anti_piracy_timer_function, (x_u32)(__machine));

		/* mod timer for four second */
		mod_timer(&anti_piracy_timer, jiffies + get_system_hz() * 4);
	}
	else
	{
		/* system halt */
		halt();

		/* system reset */
		reset();

		/* dead loop */
		while(1);
	}
}

/*
 * machine proc interface
 */
static x_s32 machine_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 size[16];
	x_s8 * p;
	x_s32 len = 0;

	if(__machine == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" board name   : %s\r\n", __machine->info.board_name);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" board desc   : %s\r\n", __machine->info.board_desc);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" board id     : %s\r\n", __machine->info.board_id);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" cpu name     : %s\r\n", __machine->info.cpu_name);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" cpu desc     : %s\r\n", __machine->info.cpu_desc);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" cpu id       : %s\r\n", __machine->info.cpu_id);

#if defined(__SYS_32BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory start : 0x%08lx\r\n", (x_u32)__machine->res.mem_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory end   : 0x%08lx\r\n", (x_u32)__machine->res.mem_end);
	ssize(size, (x_u64)(__machine->res.mem_end - __machine->res.mem_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory size  : %s\r\n", size);
#elif defined(__SYS_64BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory start : 0x%016Lx\r\n", (x_u64)__machine->res.mem_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory end   : 0x%016Lx\r\n", (x_u64)__machine->res.mem_end);
	ssize(size, (x_u64)(__machine->res.mem_end - __machine->res.mem_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" memory size  : %s\r\n", size);
#endif

#if defined(__LITTLE_ENDIAN)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" endian format: little endian\r\n");
#elif defined(__BIG_ENDIAN)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" endian format: big endian\r\n");
#endif
#if defined(__SYS_32BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" system width : 32bits");
#elif defined(__SYS_64BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" system width : 64bits");
#endif

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc machine_proc = {
	.name	= "machine",
	.read	= machine_proc_read,
};

/*
 * link proc interface
 */
static x_s32 link_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 size[16];
	x_s8 * p;
	x_s32 len = 0;

	if(__machine == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

#if defined(__SYS_32BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text start   : 0x%08lx\r\n", (x_u32)__machine->link.text_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text end     : 0x%08lx\r\n", (x_u32)__machine->link.text_end);
	ssize(size, (x_u64)(__machine->link.text_end - __machine->link.text_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk start: 0x%08lx\r\n", (x_u32)__machine->link.ramdisk_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk end  : 0x%08lx\r\n", (x_u32)__machine->link.ramdisk_end);
	ssize(size, (x_u64)(__machine->link.ramdisk_end - __machine->link.ramdisk_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk size : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' start  : 0x%08lx\r\n", (x_u32)__machine->link.data_shadow_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' end    : 0x%08lx\r\n", (x_u32)__machine->link.data_shadow_end);
	ssize(size, (x_u64)(__machine->link.data_shadow_end - __machine->link.data_shadow_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' size   : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data start   : 0x%08lx\r\n", (x_u32)__machine->link.data_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data end     : 0x%08lx\r\n", (x_u32)__machine->link.data_end);
	ssize(size, (x_u64)(__machine->link.data_end - __machine->link.data_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss start    : 0x%08lx\r\n", (x_u32)__machine->link.bss_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss end      : 0x%08lx\r\n", (x_u32)__machine->link.bss_end);
	ssize(size, (x_u64)(__machine->link.bss_end - __machine->link.bss_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss size     : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap start   : 0x%08lx\r\n", (x_u32)__machine->link.heap_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap end     : 0x%08lx\r\n", (x_u32)__machine->link.heap_end);
	ssize(size, (x_u64)(__machine->link.heap_end - __machine->link.heap_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack start  : 0x%08lx\r\n", (x_u32)__machine->link.stack_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack end    : 0x%08lx\r\n", (x_u32)__machine->link.stack_end);
	ssize(size, (x_u64)(__machine->link.stack_end - __machine->link.stack_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack size   : %s", size);
#elif defined(__SYS_64BIT)
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text start   : 0x%016Lx\r\n", (x_u64)__machine->link.text_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text end     : 0x%016Lx\r\n", (x_u64)__machine->link.text_end);
	ssize(size, (x_u64)(__machine->link.text_end - __machine->link.text_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" text size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk start: 0x%016Lx\r\n", (x_u64)__machine->link.ramdisk_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk end  : 0x%016Lx\r\n", (x_u64)__machine->link.ramdisk_end);
	ssize(size, (x_u64)(__machine->link.ramdisk_end - __machine->link.ramdisk_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" ramdisk size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' start  : 0x%016Lx\r\n", (x_u64)__machine->link.data_shadow_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' end    : 0x%016Lx\r\n", (x_u64)__machine->link.data_shadow_end);
	ssize(size, (x_u64)(__machine->link.data_shadow_end - __machine->link.data_shadow_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data' size   : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data start   : 0x%016Lx\r\n", (x_u64)__machine->link.data_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data end     : 0x%016Lx\r\n", (x_u64)__machine->link.data_end);
	ssize(size, (x_u64)(__machine->link.data_end - __machine->link.data_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" data size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss start    : 0x%016Lx\r\n", (x_u64)__machine->link.bss_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss end      : 0x%016Lx\r\n", (x_u64)__machine->link.bss_end);
	ssize(size, (x_u64)(__machine->link.bss_end - __machine->link.bss_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bss size     : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap start   : 0x%016Lx\r\n", (x_u64)__machine->link.heap_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap end     : 0x%016Lx\r\n", (x_u64)__machine->link.heap_end);
	ssize(size, (x_u64)(__machine->link.heap_end - __machine->link.heap_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" heap size    : %s\r\n", size);

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack start  : 0x%016Lx\r\n", (x_u64)__machine->link.stack_start);
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack end    : 0x%016Lx\r\n", (x_u64)__machine->link.stack_end);
	ssize(size, (x_u64)(__machine->link.stack_end - __machine->link.stack_start + 1));
	len += sprintf((x_s8 *)(p + len), (const x_s8 *)" stack size   : %s", size);
#endif

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc link_proc = {
	.name	= "link",
	.read	= link_proc_read,
};

/*
 * machine pure sync init
 */
static __init void machine_pure_sync_init(void)
{
	__machine = 0;

	/* register proc interface */
	proc_register(&machine_proc);
	proc_register(&link_proc);
}

static __exit void machine_pure_sync_exit(void)
{
	/* unregister proc interface */
	proc_unregister(&machine_proc);
	proc_unregister(&link_proc);
}

module_init(machine_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(machine_pure_sync_exit, LEVEL_PURE_SYNC);
