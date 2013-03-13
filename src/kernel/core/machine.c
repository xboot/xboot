/*
 * kernel/core/machine.c
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
#include <xboot/machine.h>

static struct machine * __machine = NULL;

inline struct machine * get_machine(void)
{
	return __machine;
}

bool_t machine_sleep(void)
{
	if(get_machine() && get_machine()->pm.sleep)
		return get_machine()->pm.sleep();
	return FALSE;
}

bool_t machine_halt(void)
{
	if(get_machine() && get_machine()->pm.halt)
		return get_machine()->pm.halt();

	return FALSE;
}

bool_t machine_reset(void)
{
	if(get_machine() && get_machine()->pm.reset)
		return get_machine()->pm.reset();

	return FALSE;
}

bool_t machine_batinfo(struct battery_info * info)
{
	if(get_machine() && get_machine()->misc.batinfo)
		return get_machine()->misc.batinfo(info);

	return FALSE;
}

/*
 * clean up system before running os
 */
bool_t machine_cleanup(void)
{
	if(get_machine() && get_machine()->misc.cleanup)
		return get_machine()->misc.cleanup();

	return FALSE;
}

/*
 * machine authentication for anti-piracy
 */
bool_t machine_authentication(void)
{
	if(get_machine() && get_machine()->misc.authentication)
		return get_machine()->misc.authentication();

	return FALSE;
}

bool_t register_machine(struct machine * mach)
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

static s32_t machine_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct battery_info info;
	char size[16];
	char * p;
	s32_t len = 0;
	s32_t i;

	if(get_machine() == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)" board name         : %s\r\n", get_machine()->info.board_name);
	len += sprintf((char *)(p + len), (const char *)" board desc         : %s\r\n", get_machine()->info.board_desc);
	len += sprintf((char *)(p + len), (const char *)" board id           : %s\r\n", get_machine()->info.board_id);

	len += sprintf((char *)(p + len), (const char *)" cpu name           : %s\r\n", get_machine()->info.cpu_name);
	len += sprintf((char *)(p + len), (const char *)" cpu desc           : %s\r\n", get_machine()->info.cpu_desc);
	len += sprintf((char *)(p + len), (const char *)" cpu id             : %s\r\n", get_machine()->info.cpu_id);

	if(machine_batinfo(&info))
	{
		len += sprintf((char *)(p + len), (const char *)" battery charging   : %s\r\n", info.charging ? "yes" : "no");
		len += sprintf((char *)(p + len), (const char *)" battery voltage    : %ldmV\r\n", info.voltage);
		len += sprintf((char *)(p + len), (const char *)" charge current     : %ldmA\r\n", info.charge_current);
		len += sprintf((char *)(p + len), (const char *)" discharge current  : %ldmA\r\n", info.discharge_current);
		len += sprintf((char *)(p + len), (const char *)" battery temperature: %ld.%ldC\r\n", info.temperature/10, info.temperature%10);
		len += sprintf((char *)(p + len), (const char *)" battery capacity   : %ldmAh\r\n", info.capacity);
		len += sprintf((char *)(p + len), (const char *)" internal resistance: %ldmohm\r\n", info.internal_resistance);
		len += sprintf((char *)(p + len), (const char *)" battery level      : %ld%%\r\n", info.level);
	}

	for(i = 0; i < ARRAY_SIZE(get_machine()->res.mem_banks); i++)
	{
		if( (get_machine()->res.mem_banks[i].start == 0) && (get_machine()->res.mem_banks[i].end == 0) )
			break;

		len += sprintf((char *)(p + len), (const char *)" memory bank%ld start : %p\r\n", i, (void *)get_machine()->res.mem_banks[i].start);
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld end   : %p\r\n", i, (void *)get_machine()->res.mem_banks[i].end);
		ssize(size, (u64_t)(get_machine()->res.mem_banks[i].end - get_machine()->res.mem_banks[i].start + 1));
		len += sprintf((char *)(p + len), (const char *)" memory bank%ld size  : %s\r\n", i, size);
	}

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

static s32_t link_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char size[16];
	char * p;
	s32_t len = 0;

	if(get_machine() == 0)
		return 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)" text start   : %p\r\n", (void *)get_machine()->link.text_start);
	len += sprintf((char *)(p + len), (const char *)" text end     : %p\r\n", (void *)get_machine()->link.text_end);
	ssize(size, (u64_t)(get_machine()->link.text_end - get_machine()->link.text_start + 1));
	len += sprintf((char *)(p + len), (const char *)" text size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" romdisk start: %p\r\n", (void *)get_machine()->link.romdisk_start);
	len += sprintf((char *)(p + len), (const char *)" romdisk end  : %p\r\n", (void *)get_machine()->link.romdisk_end);
	ssize(size, (u64_t)(get_machine()->link.romdisk_end - get_machine()->link.romdisk_start + 1));
	len += sprintf((char *)(p + len), (const char *)" romdisk size : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data' start  : %p\r\n", (void *)get_machine()->link.data_shadow_start);
	len += sprintf((char *)(p + len), (const char *)" data' end    : %p\r\n", (void *)get_machine()->link.data_shadow_end);
	ssize(size, (u64_t)(get_machine()->link.data_shadow_end - get_machine()->link.data_shadow_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data' size   : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" data start   : %p\r\n", (void *)get_machine()->link.data_start);
	len += sprintf((char *)(p + len), (const char *)" data end     : %p\r\n", (void *)get_machine()->link.data_end);
	ssize(size, (u64_t)(get_machine()->link.data_end - get_machine()->link.data_start + 1));
	len += sprintf((char *)(p + len), (const char *)" data size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" bss start    : %p\r\n", (void *)get_machine()->link.bss_start);
	len += sprintf((char *)(p + len), (const char *)" bss end      : %p\r\n", (void *)get_machine()->link.bss_end);
	ssize(size, (u64_t)(get_machine()->link.bss_end - get_machine()->link.bss_start + 1));
	len += sprintf((char *)(p + len), (const char *)" bss size     : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" heap start   : %p\r\n", (void *)get_machine()->link.heap_start);
	len += sprintf((char *)(p + len), (const char *)" heap end     : %p\r\n", (void *)get_machine()->link.heap_end);
	ssize(size, (u64_t)(get_machine()->link.heap_end - get_machine()->link.heap_start + 1));
	len += sprintf((char *)(p + len), (const char *)" heap size    : %s\r\n", size);

	len += sprintf((char *)(p + len), (const char *)" stack start  : %p\r\n", (void *)get_machine()->link.stack_start);
	len += sprintf((char *)(p + len), (const char *)" stack end    : %p\r\n", (void *)get_machine()->link.stack_end);
	ssize(size, (u64_t)(get_machine()->link.stack_end - get_machine()->link.stack_start + 1));
	len += sprintf((char *)(p + len), (const char *)" stack size   : %s", size);

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

static __init void machine_pure_sync_init(void)
{
	proc_register(&machine_proc);
	proc_register(&link_proc);
}

static __exit void machine_pure_sync_exit(void)
{
	proc_unregister(&machine_proc);
	proc_unregister(&link_proc);
}

pure_initcall_sync(machine_pure_sync_init);
pure_exitcall_sync(machine_pure_sync_exit);
