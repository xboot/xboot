/*
 * kernel/time/tick.c
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
#include <time/tick.h>

static struct tick_t * __tick = NULL;

volatile u32_t jiffies = 0;
EXPORT_SYMBOL(jiffies);

volatile u32_t HZ = 0;
EXPORT_SYMBOL(HZ);

inline void tick_interrupt(void)
{
	jiffies++;

	extern void exec_timer_task(void);
	exec_timer_task();
}

bool_t register_tick(struct tick_t * tick)
{
	if( tick && (tick->hz > 0) && (__tick->init != NULL))
	{
		__tick = tick;
		return TRUE;
	}
	else
	{
		__tick = NULL;
		return FALSE;
	}
}

bool_t init_system_tick(void)
{
	HZ = 0;
	jiffies = 0;

	if(!__tick)
		return FALSE;

	if( !__tick->init() )
		return FALSE;

	HZ = __tick->hz;
	return TRUE;
}

u64_t clock_gettime(void)
{
	if(HZ > 0)
		return (u64_t)jiffies * 1000000L / HZ;

	return 0;
}

static s32_t jiffies_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	len = sprintf(tmp, (const char *)"%d", jiffies);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc_t jiffies_proc = {
	.name	= "jiffies",
	.read	= jiffies_proc_read,
};

static s32_t uptime_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	if(HZ != 0)
		len = sprintf(tmp, (const char *)"%u.%03u", jiffies * 1000 / HZ / 1000, jiffies * 1000 / HZ % 1000);
	else
		len = sprintf(tmp, (const char *)"0.00");

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc_t uptime_proc = {
	.name	= "uptime",
	.read	= uptime_proc_read,
};

static s32_t hz_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	len = sprintf(tmp, (const char *)"%u", HZ);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc_t hz_proc = {
	.name	= "hz",
	.read	= hz_proc_read,
};

static __init void tick_proc_init(void)
{
	proc_register(&jiffies_proc);
	proc_register(&uptime_proc);
	proc_register(&hz_proc);
}

static __exit void tick_proc_exit(void)
{
	proc_unregister(&jiffies_proc);
	proc_unregister(&uptime_proc);
	proc_unregister(&hz_proc);
}

core_initcall(tick_proc_init);
core_exitcall(tick_proc_exit);
