/*
 * framework/framework.c
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

#include <framework/xfs/xfs.h>
#include <framework/framework.h>

static const luaL_Reg xboot_libs[] = {
	{ "xboot.framerate",			luaopen_framerate },
	{ "org.xboot.base64",			luaopen_base64 },
	{ "org.xboot.event", 			luaopen_event },
	{ "org.xboot.event.listener",	luaopen_event_listener },
	{ "org.xboot.event.dispatcher",	luaopen_event_dispatcher },
	{ "org.xboot.cairo",			luaopen_cairo },
	{ "org.xboot.boot",				luaopen_boot },
	{ NULL, NULL }
};

static bool_t register_preload(lua_State * L, const char * name, lua_CFunction f)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);

	return TRUE;
}

static bool_t register_searcher(lua_State * L, lua_CFunction f, int pos)
{
	lua_getglobal(L, "table");
	lua_getfield(L, -1, "insert");
	lua_remove(L, -2);

	lua_getglobal(L, "package");
	lua_getfield(L, -1, "searchers");
	lua_remove(L, -2);

	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	lua_pushnumber(L, pos);
	lua_pushcfunction(L, f);
	lua_call(L, 3, 0);

	lua_pop(L, 2);
	return TRUE;
}

struct __reader_data
{
	struct xfs_file_t * file;
	char buffer[LUAL_BUFFERSIZE];
};

static const char * __reader(lua_State * L, void * data, size_t * size)
{
	struct __reader_data * rd = (struct __reader_data *)data;
	s64_t ret;

	ret = xfs_read(rd->file, rd->buffer, 1, LUAL_BUFFERSIZE);
	if(ret < 0)
	{
		lua_error(L);
		return NULL;
	}

	*size = (size_t)ret;
	return rd->buffer;
}

static int __loadfile(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct __reader_data * rd;

	rd = malloc(sizeof(struct __reader_data));
	if(!rd)
		return lua_error(L);

	rd->file = xfs_open_read(filename);
	if(!rd->file)
	{
		free(rd);
		return lua_error(L);
	}

	if(lua_load(L, __reader, rd, filename, NULL))
	{
		free(rd);
		return lua_error(L);
	}

	xfs_close(rd->file);
	free(rd);

	return 1;
}

static int searcher_package_lua(lua_State * L)
{
	const char * filename = lua_tostring(L, -1);
	char * buf;
	size_t len, i;

	len = strlen(filename);
	buf = malloc(len + 16);
	if(!buf)
		return lua_error(L);

	strcpy(buf, filename);
	for(i = 0; i < len; i++)
	{
		if(buf[i] == '.')
			buf[i] = '/';
	}

	if(xfs_is_directory(buf))
		strcat(buf, "/init.lua");
	else
		strcat(buf, ".lua");

	if(xfs_exists(buf))
	{
		lua_pop(L, 1);
		lua_pushcfunction(L, __loadfile);
		lua_pushstring(L, buf);
		lua_call(L, 1, 1);
	}
	else
	{
		lua_pushfstring(L, "\n\tno file '%s' in application directories", buf);
	}

	free(buf);
	return 1;
}

int luaopen_xboot(lua_State * L)
{
	int i;

	lua_getglobal(L, "xboot");
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "xboot");
	}

	lua_pushstring(L, XBOOT_VERSION);
	lua_setfield(L, -2, "_VERSION");

	lua_pushnumber(L, XBOOT_MAJOY);
	lua_setfield(L, -2, "_MAJOY");

	lua_pushnumber(L, XBOOT_MINIOR);
	lua_setfield(L, -2, "_MINIOR");

	lua_pushnumber(L, XBOOT_PATCH);
	lua_setfield(L, -2, "_PATCH");

	lua_pushstring(L, __ARCH__);
	lua_setfield(L, -2, "_ARCH");

	lua_pushstring(L, __MACH__);
	lua_setfield(L, -2, "_MACH");

	register_searcher(L, searcher_package_lua, 2);
	register_preload(L, "xboot", luaopen_xboot);

	for(i = 0; xboot_libs[i].name != 0; i++)
		register_preload(L, xboot_libs[i].name, xboot_libs[i].func);

	return 1;
}
