/*
 * framework/hardware/l-spi.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <spi/spi.h>
#include <framework/hardware/l-hardware.h>

struct lspi_t {
	struct spi_device_t * dev;
};

static int l_spi_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	int mode = luaL_optinteger(L, 2, 0);
	int bits = luaL_optinteger(L, 3, 8);
	int speed = luaL_optinteger(L, 4, 0);
	struct spi_device_t * dev = spi_device_alloc(name, mode, bits, speed);
	if(!dev)
		return 0;
	struct lspi_t * spi = lua_newuserdata(L, sizeof(struct lspi_t));
	spi->dev = dev;
	luaL_setmetatable(L, MT_HARDWARE_SPI);
	return 1;
}

static const luaL_Reg l_spi[] = {
	{"new",	l_spi_new},
	{NULL,	NULL}
};

static int m_spi_gc(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	spi_device_free(spi->dev);
	return 0;
}

static int m_spi_read(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	int count = luaL_checkinteger(L, 2);
	if(count <= 0)
	{
		lua_pushnil(L);
	}
	else if(count <= SZ_4K)
	{
		char buf[SZ_4K];
		if(!(spi_device_write_then_read(spi->dev, 0, 0, buf, count) < 0))
			lua_pushlstring(L, buf, count);
		else
			lua_pushnil(L);
	}
	else
	{
		char * p = malloc(count);
		if(p && !(spi_device_write_then_read(spi->dev, 0, 0, p, count) < 0))
			lua_pushlstring(L, p, count);
		else
			lua_pushnil(L);
		free(p);
	}
	return 1;
}

static int m_spi_write(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	int count;
	const char * buf = luaL_checklstring(L, 2, (size_t *)&count);
	if(count > 0)
		lua_pushboolean(L, (spi_device_write_then_read(spi->dev, (void *)buf, count, 0, 0) < 0) ? 0 : 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}

static int m_spi_chipselect(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	spi_device_chipselect(spi->dev, lua_toboolean(L, 2) ? 1 : 0);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_spi[] = {
	{"__gc",		m_spi_gc},
	{"read",		m_spi_read},
	{"write",		m_spi_write},
	{"chipselect",	m_spi_chipselect},
	{NULL,	NULL}
};

int luaopen_hardware_spi(lua_State * L)
{
	luaL_newlib(L, l_spi);
	luahelper_create_metatable(L, MT_HARDWARE_SPI, m_spi);
	return 1;
}
