/*
 * drivers/input/input.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <malloc.h>
#include <hash.h>
#include <fifo.h>
#include <xboot/chrdev.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <terminal/terminal.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/input.h>

static struct fifo * input_event_fifo;

void input_report(enum input_type type, x_u32 code, x_u32 value)
{
	struct input_event event;

	event.time = jiffies;
	event.time = type;
	event.code = code;
	event.value = value;

	fifo_put(input_event_fifo, (x_u8 *)&event, sizeof(struct input_event));
}

void input_sync(enum input_type type)
{

}

static __init void input_pure_sync_init(void)
{
	input_event_fifo = fifo_alloc(sizeof(struct input_event) * 256);
}

static __init void input_pure_sync_exit(void)
{
	fifo_free(input_event_fifo);
}

module_init(input_pure_sync_init, LEVEL_PURE_SYNC);
module_init(input_pure_sync_exit, LEVEL_PURE_SYNC);
