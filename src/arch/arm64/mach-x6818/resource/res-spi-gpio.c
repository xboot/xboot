/*
 * resource/res-spi-gpio.c
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

#include <xboot.h>
#include <bus/spi-gpio.h>
#include <s5p6818-gpio.h>

static struct spi_gpio_data_t spi_gpio_datas[] = {
	[0] = {
		.sclk_pin	= S5P6818_GPIOC(29),
		.mosi_pin	= S5P6818_GPIOC(31),
		.miso_pin	= S5P6818_GPIOD(0),
		.cs_pin		= S5P6818_GPIOC(30),
	},
};

static struct resource_t res_spi_gpios[] = {
	{
		.name	= "spi-gpio",
		.id		= 0,
		.data	= &spi_gpio_datas[0],
	},
};

static __init void resource_spi_gpio_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_spi_gpios); i++)
	{
		if(register_resource(&res_spi_gpios[i]))
			LOG("Register resource %s:'%s.%d'", res_spi_gpios[i].mach, res_spi_gpios[i].name, res_spi_gpios[i].id);
		else
			LOG("Failed to register resource %s:'%s.%d'", res_spi_gpios[i].mach, res_spi_gpios[i].name, res_spi_gpios[i].id);
	}
}

static __exit void resource_spi_gpio_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_spi_gpios); i++)
	{
		if(unregister_resource(&res_spi_gpios[i]))
			LOG("Unregister resource %s:'%s.%d'", res_spi_gpios[i].mach, res_spi_gpios[i].name, res_spi_gpios[i].id);
		else
			LOG("Failed to unregister resource %s:'%s.%d'", res_spi_gpios[i].mach, res_spi_gpios[i].name, res_spi_gpios[i].id);
	}
}

resource_initcall(resource_spi_gpio_init);
resource_exitcall(resource_spi_gpio_exit);
