/*
 * framework/hardware/l-spi.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
	int cs = luaL_optinteger(L, 2, 0);
	int mode = luaL_optinteger(L, 3, 0);
	int bits = luaL_optinteger(L, 4, 8);
	int speed = luaL_optinteger(L, 5, 0);
	struct spi_device_t * dev = spi_device_alloc(name, cs, mode, bits, speed);
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

static int m_spi_select(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	spi_device_select(spi->dev);
	lua_settop(L, 1);
	return 1;
}

static int m_spi_deselect(lua_State * L)
{
	struct lspi_t * spi = luaL_checkudata(L, 1, MT_HARDWARE_SPI);
	spi_device_deselect(spi->dev);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_spi[] = {
	{"__gc",		m_spi_gc},
	{"read",		m_spi_read},
	{"write",		m_spi_write},
	{"select",		m_spi_select},
	{"deselect",	m_spi_deselect},
	{NULL,	NULL}
};

int luaopen_hardware_spi(lua_State * L)
{
	luaL_newlib(L, l_spi);
	luahelper_create_metatable(L, MT_HARDWARE_SPI, m_spi);
	return 1;
}
