/*
 * drivers/serial/serial.c
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
#include <console/scon.h>
#include <serial/serial.h>

static void serial_suspend(struct device_t * dev)
{
}

static void serial_resume(struct device_t * dev)
{
}

bool_t register_serial(struct serial_driver_t * drv)
{
	struct device_t * dev;

	if(!drv || !drv->info || !drv->info->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(drv->info->name);
	dev->type = DEVICE_TYPE_SERIAL;
	dev->suspend = serial_suspend;
	dev->resume = serial_resume;
	dev->driver = drv;

	if(!register_device(dev))
	{
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->init)
		(drv->init)();

	register_serial_console(drv);
	return TRUE;
}

bool_t unregister_serial(struct serial_driver_t * drv)
{
	struct device_t * dev;
	struct serial_driver_t * driver;

	if(!drv || !drv->info || !drv->info->name)
		return FALSE;

	dev = search_device_with_type(drv->info->name, DEVICE_TYPE_SERIAL);
	if(!dev)
		return FALSE;

	driver = (struct serial_driver_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_device(dev))
		return FALSE;

	free(dev->name);
	free(dev);

	unregister_serial_console(dev);
	return TRUE;
}
