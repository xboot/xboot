/*
 * kernel/time/delay.c
 *
 * some function copyed from linux kernel.
 * the calibrate_delay function be write by Linus Torvalds
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <time/delay.h>

static volatile u32_t loops_per_jiffy = 0;

void __attribute__ ((noinline)) __delay(volatile u32_t loop)
{
	for(; loop > 0; loop--);
}

void udelay(u32_t us)
{
	if(HZ > 0)
		__delay(us * loops_per_jiffy / (1000000 / HZ));
	else
		__delay(us);
}
EXPORT_SYMBOL(udelay);

void mdelay(u32_t ms)
{
	if(HZ > 0)
		__delay(ms * loops_per_jiffy / (1000 / HZ));
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

	if(HZ > 0)
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

static s32_t bogomips_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	char tmp[16];
	s32_t len;

	if(HZ > 0)
		len = sprintf(tmp, (const char *)"%u.%02u", (u32_t)( loops_per_jiffy / (500000 / HZ) ), (u32_t)( (loops_per_jiffy / (5000 / HZ) ) % 100) );
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

static struct proc_t bogomips_proc = {
	.name	= "bogomips",
	.read	= bogomips_proc_read,
};

static __init void delay_proc_init(void)
{
	proc_register(&bogomips_proc);
}

static __exit void delay_proc_exit(void)
{
	proc_unregister(&bogomips_proc);
}

core_initcall(delay_proc_init);
core_exitcall(delay_proc_exit);
