/*
 * drivers/input/keyboard/keyboard.c
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
#include <time/timer.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>


static x_s32 keyboard_open(struct chrdev * dev)
{
	return 0;
}

static x_s32 keyboard_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct keyboard_driver * drv = (struct keyboard_driver *)(dev->driver);

	return 0;
}

static x_s32 keyboard_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	return -1;
}

static x_s32 keyboard_ioctl(struct chrdev * dev, x_u32 cmd, void * arg)
{
	struct keyboard_driver * drv = (struct keyboard_driver *)(dev->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

static x_s32 keyboard_close(struct chrdev * dev)
{
	return 0;
}

/*
 * register keyboard driver
 */
x_bool register_keyboard(struct keyboard_driver * drv)
{
	struct chrdev * dev;

	if(!drv || !drv->name || !drv->probe)
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= drv->name;
	dev->type		= CHR_DEV_INPUT;
	dev->open 		= keyboard_open;
	dev->read 		= keyboard_read;
	dev->write 		= keyboard_write;
	dev->ioctl 		= keyboard_ioctl;
	dev->close		= keyboard_close;
	dev->driver 	= drv;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_INPUT) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->probe)
		(drv->probe)();

	return TRUE;
}

/*
 * unregister keyboard driver
 */
x_bool unregister_keyboard(struct keyboard_driver * drv)
{
	struct chrdev * dev;
	struct keyboard_driver * driver;

	if(!drv || !drv->name)
		return FALSE;

	dev = search_chrdev_with_type(drv->name, CHR_DEV_INPUT);
	if(!dev)
		return FALSE;

	driver = (struct keyboard_driver *)(dev->driver);
	if(driver && driver->remove)
		(driver->remove)();

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(dev);

	return TRUE;
}
