/*
 * kernel/time/delay.c
 *
 * some function copyed from linux kernel.
 * the calibrate_delay function be write by Linus Torvalds
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
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/initcall.h>
#include <time/tick.h>
#include <time/timer.h>
#include <time/delay.h>


/*
 * how many loops runnning __delay() function per jiffy.
 */
static x_u32 loops_per_jiffy = 0;

/*
 * the base delay function.
 */
static void __delay(x_u32 loop)
{
	x_u32 base;

	while(loop--)
	{
		base += loop;
	}
}

/*
 * us delay function
 */
void udelay(x_u32 us)
{
	x_u32 hz = get_system_hz();

	if(hz)
		__delay(us * loops_per_jiffy / (1000000 / hz));
	else
		__delay(us);
}

/*
 * ms delay function
 */
void mdelay(x_u32 ms)
{
	x_u32 hz = get_system_hz();

	if(hz)
		__delay(ms * loops_per_jiffy / (1000 / hz));
	else
		__delay(ms * 1000);
}

/*
 * calibrating delay loop...
 */
void calibrate_delay(void)
{
	x_u32 ticks, loopbit;
	x_s32 lps_precision = 8;
	x_u32 hz = get_system_hz();

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
static x_s32 bogomips_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[16];
	x_s32 len;
	x_u32 hz = get_system_hz();

	if(hz!=0)
		len = sprintf(tmp, (const x_s8 *)"%lu.%02lu", (x_u32)( loops_per_jiffy / (500000 / hz) ), (x_u32)( (loops_per_jiffy / (5000 / hz) ) % 100) );
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

module_init(delay_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(delay_pure_sync_exit, LEVEL_PURE_SYNC);
