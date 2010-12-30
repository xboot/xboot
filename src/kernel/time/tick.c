/*
 * kernel/time/tick.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <mode/mode.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/proc.h>
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
x_u32 jiffies = 0;

/*
 * system tick.
 */
static struct tick * xboot_tick = NULL;

/*
 * tick frequency.
 */
static x_u32 tick_hz = 0;

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
inline x_u32 get_system_hz(void)
{
	return tick_hz;
}

/*
 * register system tick.
 */
x_bool register_tick(struct tick * tick)
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
x_bool init_system_tick(void)
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
static x_s32 jiffies_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[16];
	x_s32 len;

	len = sprintf(tmp, (const x_s8 *)"%ld", jiffies);
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
static x_s32 uptime_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[16];
	x_s32 len;

	if(tick_hz != 0)
		len = sprintf(tmp, (const x_s8 *)"%lu.%02lu", jiffies / tick_hz, jiffies % tick_hz);
	else
		len = sprintf(tmp, (const x_s8 *)"0.00");

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
static x_s32 hz_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[16];
	x_s32 len;

	len = sprintf(tmp, (const x_s8 *)"%lu", tick_hz);
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

module_init(tick_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(tick_pure_sync_exit, LEVEL_PURE_SYNC);
