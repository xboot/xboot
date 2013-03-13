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
#include <types.h>
#include <sizes.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <mode/mode.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/proc.h>
#include <xboot/module.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <time/tick.h>

/*
 * exec task in timer list.
 */
extern void exec_timer_task(void);

/*
 * jiffies, userd by timer tick count.
 */
volatile u32_t jiffies = 0;
EXPORT_SYMBOL(jiffies);

/*
 * system tick.
 */
static struct tick * xboot_tick = NULL;

/*
 * tick frequency.
 */
static u32_t tick_hz = 0;

/*
 * tick interrupt.
 */
inline void tick_interrupt(void)
{
	/* tick count */
	jiffies++;

	/* exec task in timer list */
	exec_timer_task();
}

/*
 * get system tick's hz
 */
u32_t get_system_hz(void)
{
	return tick_hz;
}
EXPORT_SYMBOL(get_system_hz);

/*
 * register system tick.
 */
bool_t register_tick(struct tick * tick)
{
	if( tick && (tick->hz > 0) && (xboot_tick->init != NULL))
	{
		xboot_tick = tick;
		return TRUE;
	}
	else
	{
		xboot_tick = NULL;
		return FALSE;
	}
}

/*
 * initial system tick, enable tick timer.
 */
bool_t init_system_tick(void)
{
	tick_hz = 0;
	jiffies = 0;

	if(!xboot_tick)
		return FALSE;

	if( !xboot_tick->init() )
		return FALSE;

	/* set system tick's hz */
	tick_hz = xboot_tick->hz;

	return TRUE;
}

/*
 * jiffies proc interface
 */
static s32_t jiffies_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	len = sprintf(tmp, (const char *)"%ld", jiffies);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc jiffies_proc = {
	.name	= "jiffies",
	.read	= jiffies_proc_read,
};

/*
 * uptime proc interface
 */
static s32_t uptime_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	if(tick_hz != 0)
		len = sprintf(tmp, (const char *)"%lu.%02lu", jiffies / tick_hz, jiffies % tick_hz);
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

static struct proc uptime_proc = {
	.name	= "uptime",
	.read	= uptime_proc_read,
};

/*
 * hz proc interface
 */
static s32_t hz_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	len = sprintf(tmp, (const char *)"%lu", tick_hz);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc hz_proc = {
	.name	= "hz",
	.read	= hz_proc_read,
};

static __init void tick_pure_sync_init(void)
{
	/* register proc interface */
	proc_register(&jiffies_proc);
	proc_register(&uptime_proc);
	proc_register(&hz_proc);
}

static __exit void tick_pure_sync_exit(void)
{
	/* unregister proc interface */
	proc_unregister(&jiffies_proc);
	proc_unregister(&uptime_proc);
	proc_unregister(&hz_proc);
}

pure_initcall_sync(tick_pure_sync_init);
pure_exitcall_sync(tick_pure_sync_exit);
