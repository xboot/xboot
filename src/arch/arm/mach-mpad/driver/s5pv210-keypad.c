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
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/keyboard/keyboard.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-keypad.h>


static x_bool keypad_probe(void)
{
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
