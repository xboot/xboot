/*
 * framework/hardware/l-uart.c
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

#include <bus/uart.h>
#include <framework/hardware/l-hardware.h>

static int l_uart_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct uart_t * uart = search_bus_uart(name);
	if(!uart)
		return 0;
	if(lua_gettop(L) > 1)
	{
		enum baud_rate_t baud = luaL_optinteger(L, 2, B115200);
		enum data_bits_t data = luaL_optinteger(L, 3, DATA_BITS_8);
		enum parity_bits_t parity = luaL_optinteger(L, 4, PARITY_NONE);
		enum stop_bits_t stop = luaL_optinteger(L, 5, STOP_BITS_1);
		uart_setup(uart, baud, data, parity, stop);
	}
	lua_pushlightuserdata(L, uart);
	luaL_setmetatable(L, MT_HARDWARE_UART);
	return 1;
}

static int l_uart_list(lua_State * L)
{
	struct bus_list_t * pos, * n;
	struct uart_t * uart;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		if(pos->bus->type == BUS_TYPE_UART)
		{
			uart = (struct uart_t *)(pos->bus->driver);
			if(!uart)
				continue;
			lua_pushlightuserdata(L, uart);
			luaL_setmetatable(L, MT_HARDWARE_UART);
			lua_setfield(L, -2, pos->bus->name);
		}
	}
	return 1;
}

static const luaL_Reg l_uart[] = {
	{"new",		l_uart_new},
	{"list",	l_uart_list},
	{NULL,	NULL}
};

static int m_uart_setup(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
	enum baud_rate_t baud = luaL_optinteger(L, 2, B115200);
	enum data_bits_t data = luaL_optinteger(L, 3, DATA_BITS_8);
	enum parity_bits_t parity = luaL_optinteger(L, 4, PARITY_NONE);
	enum stop_bits_t stop = luaL_optinteger(L, 5, STOP_BITS_1);
	uart_setup(uart, baud, data, parity, stop);
	lua_settop(L, 1);
	return 1;
}

static int m_uart_read(lua_State * L)
{
	struct uart_t * uart = luaL_checkudata(L, 1, MT_HARDWARE_UART);
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
	{"setup",	m_uart_setup},
	{"read",	m_uart_read},
	{"write",	m_uart_write},
	{NULL,	NULL}
};

int luaopen_hardware_uart(lua_State * L)
{
	luaL_newlib(L, l_uart);
    /* baud_rate_t */
	luahelper_set_intfield(L, "B50",			B50);
	luahelper_set_intfield(L, "B75",			B75);
	luahelper_set_intfield(L, "B110",			B110);
	luahelper_set_intfield(L, "B134",			B134);
	luahelper_set_intfield(L, "B200",			B200);
	luahelper_set_intfield(L, "B300",			B300);
	luahelper_set_intfield(L, "B600",			B600);
	luahelper_set_intfield(L, "B1200",			B1200);
	luahelper_set_intfield(L, "B1800",			B1800);
	luahelper_set_intfield(L, "B2400",			B2400);
	luahelper_set_intfield(L, "B4800",			B4800);
	luahelper_set_intfield(L, "B9600",			B9600);
	luahelper_set_intfield(L, "B19200",			B19200);
	luahelper_set_intfield(L, "B38400",			B38400);
	luahelper_set_intfield(L, "B57600",			B57600);
	luahelper_set_intfield(L, "B76800",			B76800);
	luahelper_set_intfield(L, "B115200",		B115200);
	luahelper_set_intfield(L, "B230400",		B230400);
	luahelper_set_intfield(L, "B380400",		B380400);
	luahelper_set_intfield(L, "B460800",		B460800);
	luahelper_set_intfield(L, "B921600",		B921600);
	/* data_bits_t */
	luahelper_set_intfield(L, "DATA_BITS_5",	DATA_BITS_5);
	luahelper_set_intfield(L, "DATA_BITS_6",	DATA_BITS_6);
	luahelper_set_intfield(L, "DATA_BITS_7",	DATA_BITS_7);
	luahelper_set_intfield(L, "DATA_BITS_8",	DATA_BITS_8);
	/* parity_bits_t */
	luahelper_set_intfield(L, "PARITY_NONE",	PARITY_NONE);
	luahelper_set_intfield(L, "PARITY_EVEN",	PARITY_EVEN);
	luahelper_set_intfield(L, "PARITY_ODD",		PARITY_ODD);
	/* stop_bits_t */
	luahelper_set_intfield(L, "STOP_BITS_1",	STOP_BITS_1);
	luahelper_set_intfield(L, "STOP_BITS_1_5",	STOP_BITS_1_5);
	luahelper_set_intfield(L, "STOP_BITS_2",	STOP_BITS_2);
	luahelper_create_metatable(L, MT_HARDWARE_UART, m_uart);
	return 1;
}
