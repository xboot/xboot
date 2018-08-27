/*
 * framework/vm.c
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

#include <xfs/xfs.h>
#include <shell/readline.h>
#include <framework/luahelper.h>
#include <framework/lang/l-debugger.h>
#include <framework/lang/l-class.h>
#include <framework/event/l-event.h>
#include <framework/event/l-event-dispatcher.h>
#include <framework/stopwatch/l-stopwatch.h>
#include <framework/base64/l-base64.h>
#include <framework/display/l-display.h>
#include <framework/hardware/l-hardware.h>
#include <framework/vm.h>

extern int luaopen_cjson_safe(lua_State *);

static void luaopen_glblibs(lua_State * L)
{
	const luaL_Reg glblibs[] = {
		{ "Debugger",				luaopen_debugger },
		{ "Class",					luaopen_class },
		{ "Event",					luaopen_event },
		{ "EventDispatcher",		luaopen_event_dispatcher },
		{ NULL,	NULL },
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
		{ "builtin.json",			luaopen_cjson_safe },
		{ "builtin.base64",			luaopen_base64 },

		{ "builtin.stopwatch",		luaopen_stopwatch },
		{ "builtin.matrix",			luaopen_matrix },
		{ "builtin.easing",			luaopen_easing },
		{ "builtin.object",			luaopen_object },
		{ "builtin.pattern",		luaopen_pattern },
		{ "builtin.texture",		luaopen_texture },
		{ "builtin.ninepatch",		luaopen_ninepatch },
		{ "builtin.shape",			luaopen_shape },
		{ "builtin.font",			luaopen_font },
		{ "builtin.display",		luaopen_display },

		{ "hardware.adc",			luaopen_hardware_adc },
		{ "hardware.battery",		luaopen_hardware_battery },
		{ "hardware.buzzer",		luaopen_hardware_buzzer },
		{ "hardware.compass",		luaopen_hardware_compass },
		{ "hardware.dac",			luaopen_hardware_dac },
		{ "hardware.gmeter",		luaopen_hardware_gmeter },
		{ "hardware.gpio",			luaopen_hardware_gpio },
		{ "hardware.gyroscope",		luaopen_hardware_gyroscope },
		{ "hardware.hygrometer",	luaopen_hardware_hygrometer },
		{ "hardware.i2c",			luaopen_hardware_i2c },
		{ "hardware.led",			luaopen_hardware_led },
		{ "hardware.ledstrip",		luaopen_hardware_ledstrip },
		{ "hardware.ledtrigger",	luaopen_hardware_ledtrigger },
		{ "hardware.light",			luaopen_hardware_light },
		{ "hardware.motor",			luaopen_hardware_motor },
		{ "hardware.nvmem",			luaopen_hardware_nvmem },
		{ "hardware.pressure",		luaopen_hardware_pressure },
		{ "hardware.proximity",		luaopen_hardware_proximity },
		{ "hardware.pwm",			luaopen_hardware_pwm },
		{ "hardware.servo",			luaopen_hardware_servo },
		{ "hardware.spi",			luaopen_hardware_spi },
		{ "hardware.stepper",		luaopen_hardware_stepper },
		{ "hardware.thermometer",	luaopen_hardware_thermometer },
		{ "hardware.uart",			luaopen_hardware_uart },
		{ "hardware.vibrator",		luaopen_hardware_vibrator },
		{ "hardware.watchdog",		luaopen_hardware_watchdog },

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
	if(luaL_loadfile(L, "/framework/xboot/boot.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}

struct __reader_data_t
{
	struct xfs_file_t * file;
	char buffer[LUAL_BUFFERSIZE];
};

static const char * __reader(lua_State * L, void * data, size_t * size)
{
	struct __reader_data_t * rd = (struct __reader_data_t *)data;
	s64_t ret;

	ret = xfs_read(rd->file, rd->buffer, LUAL_BUFFERSIZE);
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
	struct xfs_context_t * ctx = luahelper_runtime(L)->__xfs_ctx;
	const char * filename = luaL_checkstring(L, 1);
	struct __reader_data_t * rd;

	rd = malloc(sizeof(struct __reader_data_t));
	if(!rd)
		return lua_error(L);

	rd->file = xfs_open_read(ctx, filename);
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

static int l_search_package_lua(lua_State * L)
{
	struct xfs_context_t * ctx = luahelper_runtime(L)->__xfs_ctx;
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

	if(xfs_isdir(ctx, buf))
		strcat(buf, "/init.lua");
	else
		strcat(buf, ".lua");

	if(xfs_isfile(ctx, buf))
	{
		lua_pop(L, 1);
		lua_pushcfunction(L, __loadfile);
		lua_pushstring(L, buf);
		lua_call(L, 1, 1);
	}
	else
	{
		lua_pushfstring(L, "\r\n\tno file '%s' in application directories", buf);
	}

	free(buf);
	return 1;
}

static int l_xboot_version(lua_State * L)
{
	lua_pushstring(L, xboot_version_string());
	return 1;
}

static int l_xboot_uniqueid(lua_State * L)
{
	lua_pushstring(L, machine_uniqueid());
	return 1;
}

static int l_xboot_readline(lua_State * L)
{
	char * p = readline(luaL_optstring(L, 1, NULL));
	lua_pushstring(L, p);
	free(p);
	return 1;
}

static int pmain(lua_State * L)
{
	int argc = (int)lua_tointeger(L, 1);
	char ** argv = (char **)lua_touserdata(L, 2);
	int i;

	luaL_openlibs(L);
	luaopen_glblibs(L);
	luaopen_prelibs(L);

	luahelper_package_searcher(L, l_search_package_lua, 2);
	luahelper_package_path(L, "./?/init.lua;./?.lua");
	luahelper_package_cpath(L, "./?.so");

	lua_getglobal(L, "xboot");
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "xboot");
	}
	lua_pushcfunction(L, l_xboot_version);
	lua_setfield(L, -2, "version");
	lua_pushcfunction(L, l_xboot_uniqueid);
	lua_setfield(L, -2, "uniqueid");
	lua_pushcfunction(L, l_xboot_readline);
	lua_setfield(L, -2, "readline");
	lua_createtable(L, argc, 0);
	for(i = 0; i < argc; i++)
	{
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i);
	}
	lua_setfield(L, -2, "arg");
	lua_pop(L, 1);

	luaopen_boot(L);
	return 1;
}

static void * l_alloc(void * ud, void * ptr, size_t osize, size_t nsize)
{
	if(nsize == 0)
	{
		free(ptr);
		return NULL;
	}
	else
	{
		return realloc(ptr, nsize);
	}
}

static int l_panic(lua_State *L)
{
	lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\r\n", lua_tostring(L, -1));
	return 0;
}

static lua_State * l_newstate(void * ud)
{
	lua_State * L = lua_newstate(l_alloc, ud);
	if(L)
		lua_atpanic(L, &l_panic);
	return L;
}

int vmexec(int argc, char ** argv)
{
	struct runtime_t rt, *r;
	lua_State * L;
	int status = LUA_ERRRUN, result;

	runtime_create_save(&rt, argv[0], &r);
	L = l_newstate(&rt);
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
			lua_writestringerror("%s\r\n", msg);
			lua_pop(L, 1);
		}
		lua_close(L);
	}
	runtime_destroy_restore(&rt, r);
	return (result && (status == LUA_OK)) ? 0 : -1;
}
