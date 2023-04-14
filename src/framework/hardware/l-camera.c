/*
 * framework/hardware/l-camera.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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

#include <camera/camera.h>
#include <core/l-image.h>
#include <hardware/l-hardware.h>

struct lcamera_t {
	struct camera_t * cam;
	struct video_frame_t frame;
};

static int l_camera_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct camera_t * cam = name ? search_camera(name) : search_first_camera();
	if(cam)
	{
		struct lcamera_t * lcam = lua_newuserdata(L, sizeof(struct lcamera_t));
		lcam->cam = cam;
		luaL_setmetatable(L, MT_HARDWARE_CAMERA);
		return 1;
	}
	return 0;
}

static int l_camera_list(lua_State * L)
{
	struct device_t * pos, * n;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_CAMERA], head)
	{
		struct camera_t * cam = (struct camera_t *)(pos->priv);
		if(cam)
		{
			struct lcamera_t * lcam = lua_newuserdata(L, sizeof(struct lcamera_t));
			lcam->cam = cam;
			luaL_setmetatable(L, MT_HARDWARE_CAMERA);
			lua_setfield(L, -2, pos->name);
		}
	}
	return 1;
}

static const luaL_Reg l_camera[] = {
	{"new",		l_camera_new},
	{"list",	l_camera_list},
	{NULL,	NULL}
};

static int m_camera_tostring(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	lua_pushstring(L, lcam->cam->name);
	return 1;
}

static int m_camera_get_type(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	switch(lcam->frame.fmt)
	{
	case VIDEO_FORMAT_ARGB:
		lua_pushstring(L, "argb");
		break;
	case VIDEO_FORMAT_YUYV:
		lua_pushstring(L, "yuyv");
		break;
	case VIDEO_FORMAT_UYVY:
		lua_pushstring(L, "uyvy");
		break;
	case VIDEO_FORMAT_NV12:
		lua_pushstring(L, "nv12");
		break;
	case VIDEO_FORMAT_NV21:
		lua_pushstring(L, "nv21");
		break;
	case VIDEO_FORMAT_YU12:
		lua_pushstring(L, "yu12");
		break;
	case VIDEO_FORMAT_YV12:
		lua_pushstring(L, "yv12");
		break;
	case VIDEO_FORMAT_MJPG:
		lua_pushstring(L, "mjpg");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_camera_get_width(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	lua_pushnumber(L, lcam->frame.width);
	return 1;
}

static int m_camera_get_height(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	lua_pushnumber(L, lcam->frame.height);
	return 1;
}

static int m_camera_get_size(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	lua_pushnumber(L, lcam->frame.width);
	lua_pushnumber(L, lcam->frame.height);
	return 2;
}

static int m_camera_start(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	enum video_format_t fmt = VIDEO_FORMAT_ARGB;
	switch(shash(luaL_optstring(L, 4, NULL)))
	{
	case 0x7c9432e1: /* "argb" */
		fmt = VIDEO_FORMAT_ARGB;
		break;
	case 0x7ca16b22: /* "yuyv" */
		fmt = VIDEO_FORMAT_YUYV;
		break;
	case 0x7c9f4a42: /* "uyvy" */
		fmt = VIDEO_FORMAT_UYVY;
		break;
	case 0x7c9b5dac: /* "nv12" */
		fmt = VIDEO_FORMAT_NV12;
		break;
	case 0x7c9b5dcc: /* "nv21" */
		fmt = VIDEO_FORMAT_NV21;
		break;
	case 0x7ca16196: /* "yu12" */
		fmt = VIDEO_FORMAT_YU12;
		break;
	case 0x7ca165d7: /* "yv12" */
		fmt = VIDEO_FORMAT_YV12;
		break;
	case 0x7c9aa693: /* "mjpg" */
		fmt = VIDEO_FORMAT_MJPG;
		break;
	default:
		break;
	}
	int timeout = luaL_optnumber(L, 5, 3000) * (lua_Number)(1000);
	if(camera_start(lcam->cam, fmt, width, height) && camera_capture(lcam->cam, &lcam->frame, timeout))
		lua_pushboolean(L, TRUE);
	else
		lua_pushboolean(L, FALSE);
	return 1;
}

static int m_camera_stop(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	int r = camera_stop(lcam->cam);
	lua_pushboolean(L, r);
	return 1;
}

static int m_camera_capture(lua_State * L)
{
	struct lcamera_t * lcam = luaL_checkudata(L, 1, MT_HARDWARE_CAMERA);
	struct limage_t * img = NULL;
	if(luaL_testudata(L, 2, MT_IMAGE))
		img = lua_touserdata(L, 2);
	int timeout = luaL_optnumber(L, 3, 0) * (lua_Number)(1000);
	if(camera_capture(lcam->cam, &lcam->frame, timeout))
	{
		if(img && (lcam->frame.width == img->s->width) && (lcam->frame.height == img->s->height))
			video_frame_to_argb(&lcam->frame, img->s->pixels);
		lua_pushboolean(L, 1);
	}
	else
		lua_pushboolean(L, 0);
	return 1;
}

static const luaL_Reg m_camera[] = {
	{"__tostring",	m_camera_tostring},
	{"getType",		m_camera_get_type},
	{"getWidth",	m_camera_get_width},
	{"getHeight",	m_camera_get_height},
	{"getSize",		m_camera_get_size},

	{"start",		m_camera_start},
	{"stop",		m_camera_stop},
	{"capture",		m_camera_capture},
	{NULL,	NULL}
};

int luaopen_hardware_camera(lua_State * L)
{
	luaL_newlib(L, l_camera);
	luahelper_create_metatable(L, MT_HARDWARE_CAMERA, m_camera);
	return 1;
}
