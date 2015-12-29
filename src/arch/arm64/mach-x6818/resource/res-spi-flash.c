/*
 * resource/res-spi-flash.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <block/spi-flash.h>

static struct spi_flash_data_t spi_flash_data = {
	.spibus		= "spi-gpio.0",
	.mode		= 0,
	.speed		= 0,
};

static struct resource_t res_spi_flash = {
	.name		= "spi-flash",
	.id			= -1,
	.data		= &spi_flash_data,
};

static __init void resource_spi_flash_init(void)
{
	if(register_resource(&res_spi_flash))
		LOG("Register resource '%s:%s.%d'", res_spi_flash.mach, res_spi_flash.name, res_spi_flash.id);
	else
		LOG("Failed to register resource '%s:%s.%d'", res_spi_flash.mach, res_spi_flash.name, res_spi_flash.id);
}

static __exit void resource_spi_flash_exit(void)
{
	if(unregister_resource(&res_spi_flash))
		LOG("Unregister resource '%s:%s.%d'", res_spi_flash.mach, res_spi_flash.name, res_spi_flash.id);
	else
		LOG("Failed to unregister resource '%s:%s.%d'", res_spi_flash.mach, res_spi_flash.name, res_spi_flash.id);
}

resource_initcall(resource_spi_flash_init);
resource_exitcall(resource_spi_flash_exit);
