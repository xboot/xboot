/*
 * driver/s5pv210-keypad.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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
#include <string.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/irq.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/keyboard/keyboard.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-keypad.h>

/*
static void keypad_scan(void)
{
	x_u32 col;

	u32 col,cval,rval;

	DPRINTK("H3C %x H2C %x \n",readl(S5PC11X_GPH3CON),readl(S5PC11X_GPH2CON));
	DPRINTK("keypad_scan() is called\n");

	DPRINTK("row val = %x",readl(key_base + S3C_KEYIFROW));

	for (col=0; col < KEYPAD_COLUMNS; col++) {
		 clear that column number and make that normal output
		cval = KEYCOL_DMASK & ~((1 << col) | (1 << col+ 8));
		writel(cval, key_base+S3C_KEYIFCOL);

		udelay(KEYPAD_DELAY);

		rval = ~(readl(key_base+S3C_KEYIFROW)) & ((1<<KEYPAD_ROWS)-1) ;
		keymask[col] = rval;
	}

	writel(KEYIFCOL_CLEAR, key_base+S3C_KEYIFCOL);
}
*/

static x_u32 keymask[8];
static x_u32 prevmask[8];

static void delay(x_u32 loop)
{
	x_u32 base;

	while(loop--)
	{
		base += loop;
	}
}

static x_u32 keypad_scan(void)
{
	x_u32 col;

	printk("sts = 0x%08lx\r\n", readl(S5PV210_KEYPAD_STSCLR));


	for(col = 0; col < 8; col++)
	{
		writel(S5PV210_KEYPAD_COL, 0xffff & ~(0x101 << col));

		delay(100);

		keymask[col] = readl(S5PV210_KEYPAD_ROW) & 0x3fff;
			printk("0x%08lx\r\n", keymask[col]);
	}
	printk("\r\n");

	//writel(S5PV210_KEYPAD_STSCLR, 0x3fffffff);

//	writel(S5PV210_KEYPAD_COL, (readl(S5PV210_KEYPAD_COL) & ~0xffff));
	return 0;
}

static struct timer_list keypad_timer;

static void keypad_timer_function(x_u32 data)
{
	keypad_scan();

	/* mod timer for next 10 ms */
	mod_timer(&keypad_timer, jiffies + get_system_hz() / 1);
}

static x_bool keypad_probe(void)
{
	/* set GPJ1_5 for KP_COL0, and pull none */
	writel(S5PV210_GPJ1CON, (readl(S5PV210_GPJ1CON) & ~(0xf<<20)) | (0x3<<20));
	writel(S5PV210_GPJ1PUD, (readl(S5PV210_GPJ1PUD) & ~(0x3<<10)) | (0x0<<10));

	/* set GPJ2 for KP_COL1 ~ KP_COL7 and KP_ROW0, and pull none*/
	writel(S5PV210_GPJ2CON, 0x33333333);
	writel(S5PV210_GPJ2PUD, 0x00000000);

	/* set GPJ3 for KP_ROW1 ~ KP_ROW8, and pull none*/
	writel(S5PV210_GPJ3CON, 0x33333333);
	writel(S5PV210_GPJ3PUD, 0x00000000);

	/* set GPJ4_0 to GPJ4_5 for KP_ROW9 ~ KP_ROW13, and pull none*/
	writel(S5PV210_GPJ4CON, (readl(S5PV210_GPJ4CON) & ~(0x000fffff)) | (0x00033333));
	writel(S5PV210_GPJ4PUD, (readl(S5PV210_GPJ4PUD) & ~(0x000003ff)) | (0x00000000));

	writel(S5PV210_KEYPAD_CON, S5PV210_KEYPAD_CON_INT_F_EN | S5PV210_KEYPAD_CON_INT_R_EN);

	//writel(S5PV210_KEYPAD_CON, 0);
	writel(S5PV210_KEYPAD_FC, 0);
	writel(S5PV210_KEYPAD_COL, (readl(S5PV210_KEYPAD_COL) & ~0xffff));

	/* setup timer for keypad */
	setup_timer(&keypad_timer, keypad_timer_function, 0);

	/* mod timer for 10 ms */
	mod_timer(&keypad_timer, jiffies + get_system_hz() / 1);

	return TRUE;
}

static x_bool keypad_remove(void)
{
	return TRUE;
}

static x_s32 keypad_ioctl(x_u32 cmd, void * arg)
{
	return -1;
}

static struct input matrix_keypad = {
	.name		= "keypad",
	.type		= INPUT_KEYBOARD,
	.probe		= keypad_probe,
	.remove		= keypad_remove,
	.ioctl		= keypad_ioctl,
};

static __init void matrix_keypad_init(void)
{
	if(!register_input(&matrix_keypad))
		LOG_E("failed to register input '%s'", matrix_keypad.name);
}

static __exit void matrix_keypad_exit(void)
{
	if(!unregister_input(&matrix_keypad))
		LOG_E("failed to unregister input '%s'", matrix_keypad.name);
}

module_init(matrix_keypad_init, LEVEL_DRIVER);
module_exit(matrix_keypad_exit, LEVEL_DRIVER);
