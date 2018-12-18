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
#include <framework/luahelper.h>
#include <framework/core/l-assets.h>
#include <framework/core/l-class.h>
#include <framework/core/l-display-image.h>
#include <framework/core/l-display-ninepatch.h>
#include <framework/core/l-display-object.h>
#include <framework/core/l-display-shape.h>
#include <framework/core/l-event.h>
#include <framework/core/l-event-dispatcher.h>
#include <framework/core/l-stage.h>
#include <framework/core/l-stopwatch.h>
#include <framework/core/l-timer.h>
#include <framework/codec/l-base64.h>
#include <framework/codec/l-json.h>
#include <framework/graphic/l-graphic.h>
#include <framework/hardware/l-hardware.h>
#include <framework/vm.h>

static void luaopen_prelibs(lua_State * L)
{
	const luaL_Reg prelibs[] = {
		{ "codec.base64",			luaopen_base64 },
		{ "codec.json",				luaopen_cjson_safe },

		{ "graphic.display",		luaopen_display },
		{ "graphic.dobject",		luaopen_dobject },
		{ "graphic.easing",			luaopen_easing },
		{ "graphic.font",			luaopen_font },
		{ "graphic.image",			luaopen_image },
		{ "graphic.matrix",			luaopen_matrix },
		{ "graphic.ninepatch",		luaopen_ninepatch },
		{ "graphic.pattern",		luaopen_pattern },
		{ "graphic.shape",			luaopen_shape },

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

static void luaopen_glblibs(lua_State * L)
{
	const luaL_Reg glblibs[] = {
		{ "Class",					luaopen_class },
		{ "Stopwatch",				luaopen_stopwatch },
		{ "Timer",					luaopen_timer },
		{ "Event",					luaopen_event },
		{ "EventDispatcher",		luaopen_event_dispatcher },
		{ "DisplayObject",			luaopen_display_object },
		{ "DisplayImage",			luaopen_display_image },
		{ "DisplayNinepatch",		luaopen_display_ninepatch },
		{ "DisplayShape",			luaopen_display_shape },
		{ "Stage",					luaopen_stage },
		{ "Assets",					luaopen_assets },
		{ NULL,	NULL },
	};
	const luaL_Reg * lib;

	for(lib = glblibs; lib->func; lib++)
	{
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
}

static const char boot_lua[] = X(
	stage = Stage.new()
	assets = Assets.new()
	if require("main") then
		stage:loop()
	end
);

static int luaopen_boot(lua_State * L)
{
	if(luaL_loadbuffer(L, boot_lua, sizeof(boot_lua)-1, "Boot.lua") == LUA_OK)
		lua_call(L, 0, 0);
	return 0;
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
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
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
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
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

static int pmain(lua_State * L)
{
	luaL_openlibs(L);
	luaopen_prelibs(L);
	luaopen_glblibs(L);

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

	luaopen_boot(L);
	return 0;
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

static struct vmctx_t * vmctx_alloc(const char * path, const char * fb)
{
	struct vmctx_t * ctx;

	if(!is_absolute_path(path))
		return NULL;

	ctx = malloc(sizeof(struct vmctx_t));
	if(!ctx)
		return NULL;

	ctx->xfs = xfs_alloc(path);
	ctx->d = display_alloc(fb);
	return ctx;
}

static void vmctx_free(struct vmctx_t * ctx)
{
	if(!ctx)
		return;

	xfs_free(ctx->xfs);
	display_free(ctx->d);
	free(ctx);
}

static void vm_task(struct task_t * task, void * data)
{
	struct vmctx_t * ctx = (struct vmctx_t *)data;
	lua_State * L;

	L = l_newstate(ctx);
	if(L)
	{
		lua_pushcfunction(L, &pmain);
		if(luahelper_pcall(L, 0, 0) != LUA_OK)
		{
			lua_writestringerror("%s: ", task->name);
			lua_writestringerror("%s\r\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		lua_close(L);
	}
	vmctx_free(ctx);
}

int vmexec(const char * path, const char * fb)
{
	struct task_t * task;
	struct vmctx_t * ctx;

	if(!is_absolute_path(path))
		return -1;

	ctx = vmctx_alloc(path, fb);
	if(!ctx)
		return -1;

	task = task_create(NULL, path, vm_task, ctx, 0, 0);
	if(!task)
	{
		vmctx_free(ctx);
		return -1;
	}

	task_resume(task);
	return 0;
}
