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

#include <xboot.h>
#include <types.h>
#include <stddef.h>
#include <macros.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <mode/mode.h>
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
static struct machine * __machine = NULL;

/*
 * get machine
 */
inline struct machine * get_machine(void)
{
	return __machine;
}

/*
 * suspend function
 */
bool_t suspend(void)
{
	if(__machine && __machine->pm.suspend)
		return __machine->pm.suspend();
	return FALSE;
}

/*
 * resume function
 */
bool_t resume(void)
{
	if(__machine && __machine->pm.resume)
		return __machine->pm.resume();
	return FALSE;
}

/*
 * halt function
 */
bool_t halt(void)
{
	if(__machine && __machine->pm.halt)
		return __machine->pm.halt();

	return FALSE;
}

/*
 * reset function
 */
bool_t reset(void)
{
	if(__machine && __machine->pm.reset)
		return __machine->pm.reset();

	return FALSE;
}

/*
 * clean up system before running os
 */
bool_t cleanup(void)
{
	if(__machine && __machine->misc.cleanup)
		return __machine->misc.cleanup();

	return FALSE;
}

/*
 * register machine.
 */
bool_t machine_register(struct machine * mach)
{
	if(mach)
	{
		__machine = mach;

		if(__machine->pm.init)
			__machine->pm.init();

		if(__machine->misc.getmode)
			xboot_set_mode(__machine->misc.getmode());

		return TRUE;
	}
	else
	{
		__machine = NULL;
		return FALSE;
	}
}

/*
 * anti piracy timer.
 */
static struct timer_list anti_piracy_timer;

/*
 * anti piracy timer function.
 */
static void anti_piracy_timer_function(u32_t data)
{
	if(!__machine || !__machine->misc.genuine)
		return;

	if(!__machine->misc.genuine())
	{
		while(1)
		{
			halt();
			reset();
		}
	}

	mod_timer(&anti_piracy_timer, jiffies + get_system_hz() * 15);
}

/*
 * do anti piracy.
 */
void do_system_antipiracy(void)
{
	if(!__machine || !__machine->misc.genuine)
		return;

	if(__machine->misc.genuine())
	{
		LOG_I("start anti piracy");

		setup_timer(&anti_piracy_timer, anti_piracy_timer_function, (u32_t)(__machine));
		mod_timer(&anti_piracy_timer, jiffies + get_system_hz() * 15);
	}
	else
	{
		while(1)
		{
			halt();
			reset();
		}
	}
}

/*
 * machine proc interface
 */
static s32_t machine_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char size[16];
	char * p;
	s32_t len = 0;
	s32_t i;

	if(__machine == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)" board name         : %s\r\n", __machine->info.board_name);
	len += sprintf((char *)(p + len), (const char *)" board desc         : %s\r\n", __machine->info.board_desc);
	len += sprintf((char *)(p + len), (const char *)" board id           : %s\r\n", __machine->info.board_id);

	len += sprintf((char *)(p + len), (const char *)" cpu name           : %s\r\n", __machine->info.cpu_name);
	len += sprintf((char *)(p + len), (const char *)" cpu desc           : %s\r\n", __machine->info.cpu_desc);
	len += sprintf((char *)(p + len), (const char *)" cpu id             : %s\r\n", __machine->info.cpu_id);

#if defined(__SYS_32BIT)
	for(i = 0; i < ARRAY_SIZE(get_machine()->res.mem_banks); i++)
	{
		if( (__machine->res.mem_banks[i].start == 0) && (__machine->res.mem_banks[i].end == 0) )
			break;

		len += sprintf((char *)(p + len), (const char *)" memory bank%ld start : 0x%08lx\r\n", i, (u32_t)__machine->res.mem_banks[i].start);
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld end   : 0x%08lx\r\n", i, (u32_t)__machine->res.mem_banks[i].end);
		ssize(size, (u64_t)(__machine->res.mem_banks[i].end - __machine->res.mem_banks[i].start + 1));
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld size  : %s\r\n", i, size);
	}
#elif defined(__SYS_64BIT)
	for(i = 0; i < ARRAY_SIZE(get_machine()->res.mem_banks); i++)
	{
		if( (__machine->res.mem_banks[i].start == 0) && (__machine->res.mem_banks[i].end == 0) )
			break;

		len += sprintf((char *)(p + len), (const char *)" memory bank%ld start : 0x%016Lx\r\n", i, (u64_t)__machine->res.mem_banks[i].start);
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld end   : 0x%016Lx\r\n", i, (u64_t)__machine->res.mem_banks[i].end);
		ssize(size, (u64_t)(__machine->res.mem_banks[i].end - __machine->res.mem_banks[i].start + 1));
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld size  : %s\r\n", i, size);
	}
#endif

#if defined(__LITTLE_ENDIAN)
	len += sprintf((char *)(p + len), (const char *)" endian format      : little endian\r\n");
#elif defined(__BIG_ENDIAN)
	len += sprintf((char *)(p + len), (const char *)" endian format      : big endian\r\n");
#endif
#if defined(__SYS_32BIT)
	len += sprintf((char *)(p + len), (const char *)" system width       : 32bits");
#elif defined(__SYS_64BIT)
	len += sprintf((char *)(p + len), (const char *)" system width       : 64bits");
#endif

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
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
static s32_t link_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char size[16];
	char * p;
	s32_t len = 0;

	if(__machine == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

#if defined(__SYS_32BIT)
	len += sprintf((char *)(p + len), (const char *)" text start   : 0x%08lx\r\n", (u32_t)__machine->link.text_start);
	len += sprintf((char *)(p + len), (const char *)" text end     : 0x%08lx\r\n", (u32_t)__machine->link.text_end);
	ssize(size, (u64_t)(__machine->link.text_end - __machine->link.text_start + 1));
	len += sprintf((char *)(p + len), (const char *)" text size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" ramdisk start: 0x%08lx\r\n", (u32_t)__machine->link.ramdisk_start);
	len += sprintf((char *)(p + len), (const char *)" ramdisk end  : 0x%08lx\r\n", (u32_t)__machine->link.ramdisk_end);
	ssize(size, (u64_t)(__machine->link.ramdisk_end - __machine->link.ramdisk_start + 1));
	len += sprintf((char *)(p + len), (const char *)" ramdisk size : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data' start  : 0x%08lx\r\n", (u32_t)__machine->link.data_shadow_start);
	len += sprintf((char *)(p + len), (const char *)" data' end    : 0x%08lx\r\n", (u32_t)__machine->link.data_shadow_end);
	ssize(size, (u64_t)(__machine->link.data_shadow_end - __machine->link.data_shadow_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data' size   : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data start   : 0x%08lx\r\n", (u32_t)__machine->link.data_start);
	len += sprintf((char *)(p + len), (const char *)" data end     : 0x%08lx\r\n", (u32_t)__machine->link.data_end);
	ssize(size, (u64_t)(__machine->link.data_end - __machine->link.data_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" bss start    : 0x%08lx\r\n", (u32_t)__machine->link.bss_start);
	len += sprintf((char *)(p + len), (const char *)" bss end      : 0x%08lx\r\n", (u32_t)__machine->link.bss_end);
	ssize(size, (u64_t)(__machine->link.bss_end - __machine->link.bss_start + 1));
	len += sprintf((char *)(p + len), (const char *)" bss size     : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" heap start   : 0x%08lx\r\n", (u32_t)__machine->link.heap_start);
	len += sprintf((char *)(p + len), (const char *)" heap end     : 0x%08lx\r\n", (u32_t)__machine->link.heap_end);
	ssize(size, (u64_t)(__machine->link.heap_end - __machine->link.heap_start + 1));
	len += sprintf((char *)(p + len), (const char *)" heap size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" stack start  : 0x%08lx\r\n", (u32_t)__machine->link.stack_start);
	len += sprintf((char *)(p + len), (const char *)" stack end    : 0x%08lx\r\n", (u32_t)__machine->link.stack_end);
	ssize(size, (u64_t)(__machine->link.stack_end - __machine->link.stack_start + 1));
	len += sprintf((char *)(p + len), (const char *)" stack size   : %s", size);
#elif defined(__SYS_64BIT)
	len += sprintf((char *)(p + len), (const char *)" text start   : 0x%016Lx\r\n", (u64_t)__machine->link.text_start);
	len += sprintf((char *)(p + len), (const char *)" text end     : 0x%016Lx\r\n", (u64_t)__machine->link.text_end);
	ssize(size, (u64_t)(__machine->link.text_end - __machine->link.text_start + 1));
	len += sprintf((char *)(p + len), (const char *)" text size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" ramdisk start: 0x%016Lx\r\n", (u64_t)__machine->link.ramdisk_start);
	len += sprintf((char *)(p + len), (const char *)" ramdisk end  : 0x%016Lx\r\n", (u64_t)__machine->link.ramdisk_end);
	ssize(size, (u64_t)(__machine->link.ramdisk_end - __machine->link.ramdisk_start + 1));
	len += sprintf((char *)(p + len), (const char *)" ramdisk size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data' start  : 0x%016Lx\r\n", (u64_t)__machine->link.data_shadow_start);
	len += sprintf((char *)(p + len), (const char *)" data' end    : 0x%016Lx\r\n", (u64_t)__machine->link.data_shadow_end);
	ssize(size, (u64_t)(__machine->link.data_shadow_end - __machine->link.data_shadow_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data' size   : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data start   : 0x%016Lx\r\n", (u64_t)__machine->link.data_start);
	len += sprintf((char *)(p + len), (const char *)" data end     : 0x%016Lx\r\n", (u64_t)__machine->link.data_end);
	ssize(size, (u64_t)(__machine->link.data_end - __machine->link.data_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" bss start    : 0x%016Lx\r\n", (u64_t)__machine->link.bss_start);
	len += sprintf((char *)(p + len), (const char *)" bss end      : 0x%016Lx\r\n", (u64_t)__machine->link.bss_end);
	ssize(size, (u64_t)(__machine->link.bss_end - __machine->link.bss_start + 1));
	len += sprintf((char *)(p + len), (const char *)" bss size     : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" heap start   : 0x%016Lx\r\n", (u64_t)__machine->link.heap_start);
	len += sprintf((char *)(p + len), (const char *)" heap end     : 0x%016Lx\r\n", (u64_t)__machine->link.heap_end);
	ssize(size, (u64_t)(__machine->link.heap_end - __machine->link.heap_start + 1));
	len += sprintf((char *)(p + len), (const char *)" heap size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" stack start  : 0x%016Lx\r\n", (u64_t)__machine->link.stack_start);
	len += sprintf((char *)(p + len), (const char *)" stack end    : 0x%016Lx\r\n", (u64_t)__machine->link.stack_end);
	ssize(size, (u64_t)(__machine->link.stack_end - __machine->link.stack_start + 1));
	len += sprintf((char *)(p + len), (const char *)" stack size   : %s", size);
#endif

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
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
