/*
 * framework/core/l-display-text.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

#include <framework/core/l-display-text.h>

static const char display_text_lua[] = X(
local M = Class(DisplayObject)

function M:init(text, color, wrap, family, size)
	self._text = Text.new(text or "", color, wrap, family, size)
	local x, y, w, h = self._text:getMetrics()
	self.super:init(w, h, self._text)
end

function M:setWidth(width)
	return self
end

function M:setHeight(height)
	return self
end

function M:setSize(width, height)
	return self
end

function M:setText(text)
	self._text:setText(text or "")
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setColor(color)
	self._text:setColor(color)
	self:markDirty()
	return self
end

function M:setWrap(wrap)
	self._text:setWrap(wrap)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setFamily(family)
	self._text:setFamily(family)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setSize(size)
	self._text:setSize(size)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

return M
);

int luaopen_display_text(lua_State * L)
{
	if(luaL_loadbuffer(L, display_text_lua, sizeof(display_text_lua) - 1, "DisplayText.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
