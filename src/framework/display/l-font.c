/*
 * framework/display/l-font.c
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

#include <cairo.h>
#include <framework/display/l-display.h>

struct font_t {
	const char * family;
	cairo_font_slant_t slant;
	cairo_font_weight_t weight;
};

int l_font_new(lua_State * L)
{
	struct font_t * font = lua_newuserdata(L, sizeof(struct font_t));
	luaL_setmetatable(L, MT_NAME_FONT);
	return 1;
}

static const luaL_Reg l_font[] = {
	{"new",	l_font_new},
	{NULL,	NULL}
};

static const luaL_Reg m_font[] = {
	{NULL,			NULL}
};

int luaopen_font(lua_State * L)
{
	luaL_newlib(L, l_font);
	luahelper_create_metatable(L, MT_NAME_FONT, m_font);
	return 1;
}
