/*
 * framework/core/l-class.c
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

#include <framework/core/l-class.h>

static const char class_lua[] = X(
local function rethack(t, bbak, mbak, ...)
	t.base = bbak
	setmetatable(t.self, mbak)

	return ...
end

local function super(t, k)
	return function(self, ...)
		local bbak = t.base
		local mbak = getmetatable(t.self)

		setmetatable(t.self, t.base)
		t.base = t.base.base

		return rethack(t, bbak, mbak, bbak[k](t.self, ...))
	end
end

return function(b)
	local o = {}
	o.__index = o

	function o.new(...)
		local self = {}
		setmetatable (self, o)
		self.super = setmetatable({self = self, base = b}, {__index = super})

		if self.init then
			self:init(...)
		end

		return self
	end

	if b then
		o.base = b
		setmetatable(o, {__index = b})
	end

	return o
end
);

int luaopen_class(lua_State * L)
{
	if(luaL_loadbuffer(L, class_lua, sizeof(class_lua) - 1, "Class.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
