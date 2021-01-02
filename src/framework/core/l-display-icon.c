/*
 * framework/core/l-display-icon.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <core/l-display-icon.h>

static const char display_icon_lua[] = X(
local M = Class(DisplayObject)

function M:init(code, color, family, sz)
	self._icon = Icon.new(code, color, family, sz)
	local size = self._icon:getSize()
	self.super:init(size, size, self._icon)
end

function M:setWidth(width)
	self._icon:setSize(width)
	local size = self._icon:getSize()
	self.super:setSize(size, size)
	return self
end

function M:setHeight(height)
	self._icon:setSize(height)
	local size = self._icon:getSize()
	self.super:setSize(size, size)
	return self
end

function M:setSize(width, height)
	self._icon:setSize(width < height and width or height)
	local size = self._icon:getSize()
	self.super:setSize(size, size)
	return self
end

function M:setCode(code)
	self._icon:setCode(code)
	self:markDirty()
	return self
end

function M:setColor(color)
	self._icon:setColor(color)
	self:markDirty()
	return self
end

function M:setFamily(family)
	self._icon:setFamily(family)
	self:markDirty()
	return self
end

return M
);

int luaopen_display_icon(lua_State * L)
{
	if(luaL_loadbuffer(L, display_icon_lua, sizeof(display_icon_lua) - 1, "DisplayIcon.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
