/*
 * framework/lang/l-class.c
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
	if(luaL_loadbuffer(L, class_lua, sizeof(class_lua)-1, "Class.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
