/*
 * framework/hardware/l-gpio.c
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

#include <xboot/gpio.h>
#include <framework/hardware/l-hardware.h>

struct lgpio_t {
	struct gpiochip_t * chip;
	int offset;
};

static int l_gpio_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	int offset = luaL_checkinteger(L, 2);
	struct gpiochip_t * chip = search_gpiochip(name);
	if(chip && (offset >= 0) && (offset < chip->ngpio))
	{
		struct lgpio_t * gpio = lua_newuserdata(L, sizeof(struct lgpio_t));
		gpio->chip = chip;
		gpio->offset = offset;
		lua_pushlightuserdata(L, gpio);
		luaL_setmetatable(L, MT_NAME_HARDWARE_GPIO);
		return 1;
	}
	return 0;
}

static const luaL_Reg l_hardware_gpio[] = {
	{"new",	l_gpio_new},
	{NULL,	NULL}
};

static int m_gpio_set_cfg(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	int cfg = luaL_checkinteger(L, 2);
	gpio->chip->set_cfg(gpio->chip, gpio->offset, cfg);
	return 0;
}

static int m_gpio_get_cfg(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	int cfg = gpio->chip->get_cfg(gpio->chip, gpio->offset);
	lua_pushinteger(L, cfg);
	return 1;
}

static int m_gpio_set_pull(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_pull_t pull = (enum gpio_pull_t)luaL_checkinteger(L, 2);
	gpio->chip->set_pull(gpio->chip, gpio->offset, pull);
	return 0;
}

static int m_gpio_get_pull(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_pull_t pull = gpio->chip->get_pull(gpio->chip, gpio->offset);
	lua_pushinteger(L, pull);
	return 1;
}

static int m_gpio_set_drv(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_drv_t drv = (enum gpio_pull_t)luaL_checkinteger(L, 2);
	gpio->chip->set_drv(gpio->chip, gpio->offset, drv);
	return 0;
}

static int m_gpio_get_drv(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_drv_t drv = gpio->chip->get_drv(gpio->chip, gpio->offset);
	lua_pushinteger(L, drv);
	return 1;
}

static int m_gpio_set_rate(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_rate_t rate = (enum gpio_pull_t)luaL_checkinteger(L, 2);
	gpio->chip->set_rate(gpio->chip, gpio->offset, rate);
	return 0;
}

static int m_gpio_get_rate(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_rate_t rate = gpio->chip->get_rate(gpio->chip, gpio->offset);
	lua_pushinteger(L, rate);
	return 1;
}

static int m_gpio_set_dir(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_direction_t dir = (enum gpio_pull_t)luaL_checkinteger(L, 2);
	gpio->chip->set_dir(gpio->chip, gpio->offset, dir);
	return 0;
}

static int m_gpio_get_dir(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	enum gpio_direction_t dir = gpio->chip->get_dir(gpio->chip, gpio->offset);
	lua_pushinteger(L, dir);
	return 1;
}

static int m_gpio_set_value(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	int value = luaL_checkinteger(L, 2);
	gpio->chip->set_value(gpio->chip, gpio->offset, value);
	return 0;
}

static int m_gpio_get_value(lua_State * L)
{
	struct lgpio_t * gpio = luaL_checkudata(L, 1, MT_NAME_HARDWARE_GPIO);
	int value = gpio->chip->get_value(gpio->chip, gpio->offset);
	lua_pushinteger(L, value);
	return 1;
}

static const luaL_Reg m_hardware_gpio[] = {
	{"setCfg",		m_gpio_set_cfg},
	{"getCfg",		m_gpio_get_cfg},
	{"setPull",		m_gpio_set_pull},
	{"getPull",		m_gpio_get_pull},
	{"setDrv",		m_gpio_set_drv},
	{"getDrv",		m_gpio_get_drv},
	{"setRate",		m_gpio_set_rate},
	{"getRate", 	m_gpio_get_rate},
	{"setDir",		m_gpio_set_dir},
	{"getDir",		m_gpio_get_dir},
	{"setValue",	m_gpio_set_value},
	{"getValue",	m_gpio_get_value},
	{NULL,	NULL}
};

int luaopen_hardware_gpio(lua_State * L)
{
	luaL_newlib(L, l_hardware_gpio);
    /* gpio_pull_t */
	luahelper_set_intfield(L, "GPIO_PULL_NONE",			GPIO_PULL_NONE);
	luahelper_set_intfield(L, "GPIO_PULL_UP",			GPIO_PULL_UP);
	luahelper_set_intfield(L, "GPIO_PULL_DOWN",			GPIO_PULL_DOWN);
    /* gpio_drv_t */
	luahelper_set_intfield(L, "GPIO_DRV_NONE",			GPIO_DRV_NONE);
	luahelper_set_intfield(L, "GPIO_DRV_LOW",			GPIO_DRV_LOW);
	luahelper_set_intfield(L, "GPIO_DRV_MEDIAN",		GPIO_DRV_MEDIAN);
	luahelper_set_intfield(L, "GPIO_DRV_HIGH",			GPIO_DRV_HIGH);
    /* gpio_rate_t */
	luahelper_set_intfield(L, "GPIO_RATE_NONE",			GPIO_RATE_NONE);
	luahelper_set_intfield(L, "GPIO_RATE_FAST",			GPIO_RATE_FAST);
	luahelper_set_intfield(L, "GPIO_RATE_SLOW",			GPIO_RATE_SLOW);
    /* gpio_direction_t */
	luahelper_set_intfield(L, "GPIO_DIRECTION_NONE",	GPIO_DIRECTION_NONE);
	luahelper_set_intfield(L, "GPIO_DIRECTION_INPUT",	GPIO_DIRECTION_INPUT);
	luahelper_set_intfield(L, "GPIO_DIRECTION_OUTPUT",	GPIO_DIRECTION_OUTPUT);
	luahelper_create_metatable(L, MT_NAME_HARDWARE_GPIO, m_hardware_gpio);
	return 1;
}
