/*
 * framework/lang/l-class.c
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

#include <framework/lang/l-class.h>

static const char class_lua[] =
"local function rethack(t, bbak, mbak, ...)"									"\n"
"	t.base = bbak"																"\n"
"	setmetatable(t.self, mbak)"													"\n"
""																				"\n"
"	return ..."																	"\n"
"end"																			"\n"
""																				"\n"
"local function super(t, k)"													"\n"
"	return function(self, ...)"													"\n"
"		local bbak = t.base"													"\n"
"		local mbak = getmetatable(t.self)"										"\n"
""																				"\n"
"		setmetatable(t.self, t.base)"											"\n"
"		t.base = t.base.base"													"\n"
""																				"\n"
"		return rethack(t, bbak, mbak, bbak[k](t.self, ...))"					"\n"
"	end"																		"\n"
"end"																			"\n"
""																				"\n"
"return function(b)"															"\n"
"	local o = {}"																"\n"
"	o.__index = o"																"\n"
""																				"\n"
"	function o.new(...)"														"\n"
"		local self = {}"														"\n"
"		setmetatable(self, o)"													"\n"
"		self.super = setmetatable({self = self, base = b}, {__index = super})"	"\n"
""																				"\n"
"		if self.init then"														"\n"
"			self:init(...)"														"\n"
"		end"																	"\n"
""																				"\n"
"		return self"															"\n"
"	end"																		"\n"
""																				"\n"
"	if b then"																	"\n"
"		o.base = b"																"\n"
"		setmetatable(o, {__index = b})"											"\n"
"	end"																		"\n"
""																				"\n"
"	return o"																	"\n"
"end"																			"\n"
;

int luaopen_class(lua_State * L)
{
	if(luaL_loadbuffer(L, class_lua, sizeof(class_lua)-1, "class.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
