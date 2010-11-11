/*
 * drivers/serial/serial.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/chrdev.h>
#include <serial/serial.h>

/*
 * serial open
 */
static x_s32 serial_open(struct chrdev * dev)
{
	return 0;
}

/*
 * serial read
 */
static x_s32 serial_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->read)
		return ((drv->read)(buf, count));

	return 0;
}

/*
 * serial write.
 */
static x_s32 serial_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->write)
		return ((drv->write)(buf, count));

	return 0;
}

/*
 * serial ioctl
 */
static x_s32 serial_ioctl(struct chrdev * dev, x_u32 cmd, void * arg)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

/*
 * serial close
 */
static x_s32 serial_close(struct chrdev * dev)
{
	return 0;
}

/*
 * register serial driver, return true is successed.
 */
x_bool register_serial(struct serial_driver * drv)
{
	struct chrdev * dev;

	if(!drv || !drv->info || !drv->info->name || !(drv->read || drv->write))
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= drv->info->name;
	dev->type		= CHR_DEV_SERIAL;
	dev->open 		= serial_open;
	dev->read 		= serial_read;
	dev->write 		= serial_write;
	dev->ioctl 		= serial_ioctl;
	dev->close		= serial_close;
	dev->driver 	= drv;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_SERIAL) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->init)
		(drv->init)();

	return TRUE;
}

/*
 * unregister serial driver
 */
x_bool unregister_serial(struct serial_driver * drv)
{
	struct chrdev * dev;
	struct serial_driver * driver;

	if(!drv || !drv->info || !drv->info->name)
		return FALSE;

	dev = search_chrdev_with_type(drv->info->name, CHR_DEV_SERIAL);
	if(!dev)
		return FALSE;

	driver = (struct serial_driver *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(dev);
	return TRUE;
}
