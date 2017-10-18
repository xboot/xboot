/*
 * framework/hardware/l-uart.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <uart/uart.h>
#include <framework/hardware/l-hardware.h>

static int l_uart_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct uart_t * uart = search_uart(name);
	if(!uart)
		return 0;
	if(lua_gettop(L) > 1)
	{
		int baud = luaL_optinteger(L, 2, 115200);
		int data = luaL_optinteger(L, 3, 8);
		int parity = luaL_optinteger(L, 4, 0);
		int stop = luaL_optinteger(L, 5, 1);
		uart_set(uart, baud, data, parity, stop);
	}
	lua_pushlightuserdata(L, uart);
	luaL_setmetatable(L, MT_HARDWARE_UART);
	return 1;
}

static int l_uart_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct uart_t * uart;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_UART], head)
	{
		uart = (struct uart_t *)(pos->priv);
		if(!uart)
			continue;
		lua_pushlightuserdata(L, uart);
		luaL_setmetatable(L, MT_HARDWARE_UART);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_uart[] = {
	{"new",		l_uart_new},
	{"list",	l_uart_list},
	{NULL,	NULL}
};

static int m_uart_tostring(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	lua_pushstring(L, uart->name);
	return 1;
}

static int m_uart_set(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	int baud = luaL_optinteger(L, 2, 115200);
	int data = luaL_optinteger(L, 3, 8);
	int parity = luaL_optinteger(L, 4, 0);
	int stop = luaL_optinteger(L, 5, 1);
	uart_set(uart, baud, data, parity, stop);
	lua_settop(L, 1);
	return 1;
}

static int m_uart_get(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	lua_pushinteger(L, baud);
	lua_pushinteger(L, data);
	lua_pushinteger(L, parity);
	lua_pushinteger(L, stop);
	return 4;
}

static int m_uart_read(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	size_t count = luaL_checkinteger(L, 2);
	if(count <= 0)
	{
		lua_pushnil(L);
	}
	else if(count <= SZ_4K)
	{
		char buf[SZ_4K];
		if(uart_read(uart, (u8_t *)buf, count) == count)
			lua_pushlstring(L, buf, count);
		else
			lua_pushnil(L);
	}
	else
	{
		char * p = malloc(count);
		if(p && uart_read(uart, (u8_t *)p, count) == count)
			lua_pushlstring(L, p, count);
		else
			lua_pushnil(L);
		free(p);
	}
	return 1;
}

static int m_uart_write(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	size_t count;
	const char * buf = luaL_checklstring(L, 2, &count);
	if(count > 0)
		lua_pushboolean(L, (uart_write(uart, (const u8_t *)buf, count) == count));
	else
		lua_pushboolean(L, 0);
	return 1;
}

static const luaL_Reg m_uart[] = {
	{"__tostring",	m_uart_tostring},
	{"set",			m_uart_set},
	{"get",			m_uart_get},
	{"read",		m_uart_read},
	{"write",		m_uart_write},
	{NULL,	NULL}
};

int luaopen_hardware_uart(lua_State * L)
{
	luaL_newlib(L, l_uart);
	luahelper_create_metatable(L, MT_HARDWARE_UART, m_uart);
	return 1;
}
