/*
 * drivers/bus/spi/spi-gpio.c
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
#include <bus/spi-algo-bit.h>
#include <bus/spi-gpio.h>

struct spi_gpio_private_data_t {
	struct spi_algo_bit_data_t bdat;
	struct spi_gpio_data_t rdat;
};

static void spi_gpio_setclk(struct spi_algo_bit_data_t * bdat, int state)
{
	struct spi_gpio_data_t * rdat = (struct spi_gpio_data_t *)bdat->priv;
	gpio_set_value(rdat->clk_pin, state);
}

static void spi_gpio_setmosi(struct spi_algo_bit_data_t * bdat, int state)
{
	struct spi_gpio_data_t * rdat = (struct spi_gpio_data_t *)bdat->priv;
	gpio_set_value(rdat->mosi_pin, state);
}

static int spi_gpio_getmiso(struct spi_algo_bit_data_t * bdat)
{
	struct spi_gpio_data_t * rdat = (struct spi_algo_bit_data_t *)bdat->priv;
	return gpio_get_value(rdat->miso_pin);
}

static void spi_gpio_setcs(struct spi_algo_bit_data_t * bdat, int state)
{
	struct spi_gpio_data_t * rdat = (struct spi_gpio_data_t *)bdat->priv;
	gpio_set_value(rdat->cs_pin, state);
}

static void spi_gpio_init(struct spi_t * spi)
{
}

static void spi_gpio_exit(struct spi_t * spi)
{
}

static int spi_gpio_xfer(struct spi_t * spi, struct spi_msg_t * msgs, int num)
{
	struct spi_gpio_private_data_t * dat = (struct spi_gpio_private_data_t *)spi->priv;
	struct spi_algo_bit_data_t * bdat = &(dat->bdat);

	return spi_algo_bit_xfer(bdat, msgs, num);
}

static bool_t spi_gpio_register_bus(struct resource_t * res)
{
	struct spi_gpio_data_t * rdat = (struct spi_gpio_data_t *)res->data;
	struct spi_gpio_private_data_t * dat;
	struct spi_t * spi;
	char name[64];

	dat = malloc(sizeof(struct spi_gpio_private_data_t));
	if(!dat)
		return FALSE;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	memcpy(&(dat->rdat), rdat, sizeof(struct spi_gpio_data_t));
	dat->bdat.priv = &(dat->rdat);

	spi->name = strdup(name);
	spi->init = spi_gpio_init;
	spi->exit = spi_gpio_exit;
	spi->xfer = spi_gpio_xfer,
	spi->priv = dat;

	if(register_bus_spi(spi))
		return TRUE;

	free(spi->priv);
	free(spi->name);
	free(spi);
	return FALSE;
}

static bool_t spi_gpio_unregister_bus(struct resource_t * res)
{
	struct spi_t * spi;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	spi = search_bus_spi(name);
	if(!spi)
		return FALSE;

	if(!unregister_bus_spi(spi))
		return FALSE;

	free(spi->priv);
	free(spi->name);
	free(spi);
	return TRUE;
}

static __init void spi_gpio_bus_init(void)
{
	resource_for_each_with_name("spi-gpio", spi_gpio_register_bus);
}

static __exit void spi_gpio_bus_exit(void)
{
	resource_for_each_with_name("spi-gpio", spi_gpio_unregister_bus);
}

bus_initcall(spi_gpio_bus_init);
bus_exitcall(spi_gpio_bus_exit);
