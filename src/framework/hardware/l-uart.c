/*
 * framework/hardware/l-uart.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <bus/uart.h>
#include <framework/hardware/l-hardware.h>

static int l_uart_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	enum baud_rate_t baud = luaL_optinteger(L, 2, B115200);
	enum data_bits_t data = luaL_optinteger(L, 3, DATA_BITS_8);
	enum parity_bits_t parity = luaL_optinteger(L, 4, PARITY_NONE);
	enum stop_bits_t stop = luaL_optinteger(L, 5, STOP_BITS_1);
	struct uart_t * uart = search_bus_uart(name);
	if(!uart)
		return 0;
	uart_setup(uart, baud, data, parity, stop);
	lua_pushlightuserdata(L, uart);
	luaL_setmetatable(L, MT_NAME_HARDWARE_UART);
	return 1;
}

static const luaL_Reg l_hardware_uart[] = {
	{"new",	l_uart_new},
	{NULL,	NULL}
};

static int m_uart_read(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_NAME_HARDWARE_UART);
	size_t count = luaL_checkinteger(L, 2);
	if(count <= 0)
	{
		lua_pushnil(L);
	}
	else if(count > SZ_4K)
	{
		char * p = malloc(count);
		if(p && uart_read(uart, (u8_t *)p, count) == count)
			lua_pushlstring(L, p, count);
		else
			lua_pushnil(L);
		free(p);
	}
	else
	{
		char buf[SZ_4K];
		if(uart_read(uart, (u8_t *)buf, count) == count)
			lua_pushlstring(L, buf, count);
		else
			lua_pushnil(L);
	}
	return 1;
}

static int m_uart_write(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_NAME_HARDWARE_UART);
	size_t count;
	const char * buf = luaL_checklstring(L, 2, &count);
	if(count > 0)
		lua_pushboolean(L, (uart_write(uart, (const u8_t *)buf, count) == count));
	else
		lua_pushboolean(L, 0);
	return 1;
}

static const luaL_Reg m_hardware_uart[] = {
	{"read",	m_uart_read},
	{"write",	m_uart_write},
	{NULL,	NULL}
};

int luaopen_hardware_uart(lua_State * L)
{
	luaL_newlib(L, l_hardware_uart);
	luahelper_create_metatable(L, MT_NAME_HARDWARE_UART, m_hardware_uart);
	return 1;
}
