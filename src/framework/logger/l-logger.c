/*
 * framework/logger/l-logger.c
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

#include <framework/logger/l-logger.h>

static int l_logger_print(lua_State * L)
{
	int n = lua_gettop(L);
	int i;
	const char * s;
	size_t l;

	lua_getglobal(L, "tostring");
	for(i = 1; i <= n; i++)
	{
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);
		if(s == NULL)
			return luaL_error(L,"tostring must return a string");
		if(i > 1)
			logger_output("\t", 1);
		logger_output(s, l);
		lua_pop(L, 1);
	}
	logger_output("\r\n", 2);

	return 0;
}

static const luaL_Reg l_logger[] = {
	{"print",	l_logger_print},
	{NULL, NULL}
};

int luaopen_logger(lua_State * L)
{
	luaL_newlib(L, l_logger);
	return 1;
}
