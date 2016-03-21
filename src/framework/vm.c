/*
 * framework/vm.c
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

#include <xfs/xfs.h>
#include <framework/luahelper.h>
#include <framework/lang/l-class.h>
#include <framework/event/l-event.h>
#include <framework/stopwatch/l-stopwatch.h>
#include <framework/base64/l-base64.h>
#include <framework/display/l-display.h>
#include <framework/hardware/l-hardware.h>
#include <framework/vm.h>

extern int luaopen_cjson_safe(lua_State *);

struct __reader_data_t
{
	struct xfs_file_t * file;
	char buffer[LUAL_BUFFERSIZE];
};

static const char * __reader(lua_State * L, void * data, size_t * size)
{
	struct __reader_data_t * rd = (struct __reader_data_t *)data;
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
	struct __reader_data_t * rd;

	rd = malloc(sizeof(struct __reader_data_t));
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

static void luaopen_glblibs(lua_State * L)
{
	const luaL_Reg glblibs[] = {
		{ "class",	luaopen_class },
		{ NULL,		NULL },
	};
	const luaL_Reg * lib;

	for(lib = glblibs; lib->func; lib++)
	{
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
}

static void luaopen_prelibs(lua_State * L)
{
	const luaL_Reg prelibs[] = {
		{ "builtin.json",				luaopen_cjson_safe },

		{ "builtin.event",				luaopen_event },
		{ "builtin.stopwatch",			luaopen_stopwatch },
		{ "builtin.base64",				luaopen_base64 },

		{ "builtin.matrix",				luaopen_matrix },
		{ "builtin.easing",				luaopen_easing },
		{ "builtin.object",				luaopen_object },
		{ "builtin.pattern",			luaopen_pattern },
		{ "builtin.texture",			luaopen_texture },
		{ "builtin.ninepatch",			luaopen_ninepatch },
		{ "builtin.shape",				luaopen_shape },
		{ "builtin.font",				luaopen_font },
		{ "builtin.display",			luaopen_display },

		{ "xboot.hardware.buzzer",		luaopen_hardware_buzzer },
		{ "xboot.hardware.gpio",		luaopen_hardware_gpio },
		{ "xboot.hardware.i2c",			luaopen_hardware_i2c },
		{ "xboot.hardware.led",			luaopen_hardware_led },
		{ "xboot.hardware.ledtrig",		luaopen_hardware_ledtrig },
		{ "xboot.hardware.pwm",			luaopen_hardware_pwm },
		{ "xboot.hardware.uart",		luaopen_hardware_uart },
		{ "xboot.hardware.vibrator",	luaopen_hardware_vibrator },
		{ "xboot.hardware.watchdog",	luaopen_hardware_watchdog },

		{ NULL, NULL },
	};
	const luaL_Reg * lib;

	for(lib = prelibs; lib->func; lib++)
	{
		luahelper_preload(L, lib->name, lib->func);
	}
}

static int luaopen_boot(lua_State * L)
{
	if(luaL_loadfile(L, "/romdisk/framework/org/xboot/boot.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}

static int pmain(lua_State * L)
{
	int argc = (int)lua_tointeger(L, 1);
	char ** argv = (char **)lua_touserdata(L, 2);
	int i;

	/* open default libs */
	luaL_openlibs(L);

	/* package search rules */
	luahelper_package_searcher(L, searcher_package_lua, 2);
	luahelper_package_path(L, "./?/init.lua;./?.lua");
	luahelper_package_cpath(L, "./?.so");

	/* open global libs */
	luaopen_glblibs(L);

	/* open preload libs */
	luaopen_prelibs(L);

	/* global xboot table */
	lua_getglobal(L, "xboot");
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "xboot");
	}
	lua_pushstring(L, xboot_version_string());
	lua_setfield(L, -2, "VERSION");
	lua_pushstring(L, __ARCH__);
	lua_setfield(L, -2, "ARCH");
	lua_pushstring(L, __MACH__);
	lua_setfield(L, -2, "MACH");
	lua_pushstring(L, machine_uniqueid());
	lua_setfield(L, -2, "UNIQUEID");
	lua_createtable(L, argc, 0);
	for(i = 0; i < argc; i++)
	{
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i);
	}
	lua_setfield(L, -2, "arg");
	lua_pop(L, 1);

	/* open boot */
	luaopen_boot(L);
	return 1;
}

int vmexec(int argc, char ** argv)
{
	struct runtime_t rt, *r;
	lua_State * L;
	int status = LUA_ERRRUN, result;

	runtime_create_save(&rt, argv[0], &r);
	L = luaL_newstate();
	if(L)
	{
		lua_pushcfunction(L, &pmain);
		lua_pushinteger(L, argc);
		lua_pushlightuserdata(L, argv);
		status = luahelper_pcall(L, 2, 1);
		result = lua_toboolean(L, -1);
		if(status != LUA_OK)
		{
			const char * msg = lua_tostring(L, -1);
			lua_writestringerror("%s: ", argv[0]);
			lua_writestringerror("%s\n", msg);
			lua_pop(L, 1);
		}
		lua_close(L);
	}
	runtime_destroy_restore(&rt, r);
	return (result && (status == LUA_OK)) ? 0 : -1;
}
