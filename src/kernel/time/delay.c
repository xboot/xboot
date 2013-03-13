/*
 * kernel/time/delay.c
 *
 * some function copyed from linux kernel.
 * the calibrate_delay function be write by Linus Torvalds
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
#include <stdio.h>
#include <xboot/module.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/initcall.h>
#include <time/tick.h>
#include <time/timer.h>
#include <time/delay.h>


/*
 * how many loops runnning __delay() function per jiffy.
 */
static volatile u32_t loops_per_jiffy = 0;

/*
 * the base delay function.
 */
void __attribute__ ((noinline)) __delay(volatile u32_t loop)
{
	for(; loop > 0; loop--);
}

/*
 * us delay function
 */
void udelay(u32_t us)
{
	u32_t hz = get_system_hz();

	if(hz)
		__delay(us * loops_per_jiffy / (1000000 / hz));
	else
		__delay(us);
}
EXPORT_SYMBOL(udelay);

/*
 * ms delay function
 */
void mdelay(u32_t ms)
{
	u32_t hz = get_system_hz();

	if(hz)
		__delay(ms * loops_per_jiffy / (1000 / hz));
	else
		__delay(ms * 1000);
}
EXPORT_SYMBOL(mdelay);

/*
 * calibrating delay loop...
 */
void calibrate_delay(void)
{
	u32_t ticks, loopbit;
	s32_t lps_precision = 8;
	u32_t hz = get_system_hz();

	if(hz > 0)
	{
		loops_per_jiffy = (1<<12);

		while((loops_per_jiffy <<= 1) != 0)
		{
			/* wait for "start of" clock tick */
			ticks = jiffies;
			while (ticks == jiffies);

			/* go ... */
			ticks = jiffies;
			__delay(loops_per_jiffy);
			ticks = jiffies - ticks;

			if(ticks)
				break;
		}

		loops_per_jiffy >>= 1;
		loopbit = loops_per_jiffy;

		while(lps_precision-- && (loopbit >>= 1))
		{
			loops_per_jiffy |= loopbit;
			ticks = jiffies;
			while(ticks == jiffies);

			ticks = jiffies;
			__delay(loops_per_jiffy);

			/* longer than 1 tick */
			if(jiffies != ticks)
				loops_per_jiffy &= ~loopbit;
		}
	}
	else
	{
		loops_per_jiffy = 0;
	}
}

/*
 * bogomips proc interface
 */
static s32_t bogomips_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;
	u32_t hz = get_system_hz();

	if(hz != 0)
		len = sprintf(tmp, (const char *)"%lu.%02lu", (u32_t)( loops_per_jiffy / (500000 / hz) ), (u32_t)( (loops_per_jiffy / (5000 / hz) ) % 100) );
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

static struct proc bogomips_proc = {
	.name	= "bogomips",
	.read	= bogomips_proc_read,
};

static __init void delay_pure_sync_init(void)
{
	/* register bogomips proc interface */
	proc_register(&bogomips_proc);
}

static __exit void delay_pure_sync_exit(void)
{
	/* unregister bogomips proc interface */
	proc_unregister(&bogomips_proc);
}

pure_initcall_sync(delay_pure_sync_init);
pure_exitcall_sync(delay_pure_sync_exit);
