/*
 * framework/core/l-display-scroll.c
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

#include <framework/core/l-display-scroll.h>

static const char display_scroll_lua[] = X(
local M = Class(DisplayObject)

function M:init(w, h, vertial, friction)
	self.super:init(w, h)
	self._vertial = vertial == nil and true or vertial
	self._friction = friction or 0.92
	self._sensitivity = 1
	self._threshold = 0.001
	self._width = 0
	self._height = 0
	self._offset = 0
	self._speed = 0
	self._view = DisplayObject.new(self._width, self._height)
	self:addChild(self._view)

	if not self._vertial then
		self._view:setLayoutDirection("row"):setLayoutJustify("between"):setLayoutAlign("stretch")
	else
		self._view:setLayoutDirection("column"):setLayoutJustify("between"):setLayoutAlign("stretch")
	end
	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)
	self:addEventListener("mouse-wheel", self.onMouseWheel)
	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
end

function M:setWidth(width)
	self.super:setWidth(width)
	if not self._vertial then
		self._height = self:getHeight()
	else
		self._width = self:getWidth()
	end
	self._view:setSize(self._width, self._height)
	return self
end

function M:setHeight(height)
	self.super:setHeight(height)
	if not self._vertial then
		self._height = self:getHeight()
	else
		self._width = self:getWidth()
	end
	self._view:setSize(self._width, self._height)
	return self
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	if not self._vertial then
		self._height = self:getHeight()
	else
		self._width = self:getWidth()
	end
	self._view:setSize(self._width, self._height)
	return self
end

function M:addItem(item)
	if item then
		local left, top, right, bottom = item:getLayoutMargin()
		if not self._vertial then
			item:setLayoutEnable(1):setLayoutAlignSelf("auto"):setLayoutGrow(1):setLayoutShrink(1):setLayoutBasis(1)
			self._width = self._width + item:getWidth() + left + right
			self._height = self:getHeight()
		else
			item:setLayoutEnable(1):setLayoutAlignSelf("auto"):setLayoutGrow(1):setLayoutShrink(1):setLayoutBasis(1)
			self._width = self:getWidth()
			self._height = self._height + item:getHeight() + top + bottom
		end
		self._view:addChild(item)
		self._view:setSize(self._width, self._height)
	end
	return self
end

function M:removeItem(item)
	if item then
		local left, top, right, bottom = item:getLayoutMargin()
		if not self._vertial then
			self._width = self._width - item:getWidth() - left - right
			self._height = self:getHeight()
		else
			self._width = self:getWidth()
			self._height = self._height - item:getHeight() - top - bottom
		end
		self._view:removeChild(item)
		self._view:setSize(self._width, self._height)
	end
	return self
end

function M:clear()
	self._width = 0
	self._height = 0
	self._offset = 0
	self._speed = 0
	self._view:removeChildren()
	self._view:setSize(self._width, self._height)
	return self
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.x, e.y) then
		self._touchid = -1
		self._tx = e.x
		self._ty = e.y
		self._tt = e.time
		if not self._vertial then
			self._offset = self._view:getX()
		else
			self._offset = self._view:getY()
		end
		if math.abs(self._speed) >= self._threshold * 10000 then
			e.stop = true
		else
			e.stop = false
		end
		self._speed = 0
	end
end

function M:onMouseMove(e)
	if self._touchid == -1 then
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local s = self._sensitivity
		if not self._vertial then
			if self._view:getX() > 0 or self._view:getX() + self._view:getWidth() < self:getWidth() then
				s = s / 2
			end
			self._view:setX(self._offset + (x2 - x1) * s)
		else
			if self._view:getY() > 0 or self._view:getY() + self._view:getHeight() < self:getHeight() then
				s = s / 2
			end
			self._view:setY(self._offset + (y2 - y1) * s)
		end
		e.stop = false
		self._speed = 0
	end
end

function M:onMouseUp(e)
	if self._touchid == -1 then
		self._touchid = nil
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local dt = (e.time - self._tt) / 1000000000 * 60
		if not self._vertial then
			self._speed = (x2 - x1) * self._sensitivity / dt
		else
			self._speed = (y2 - y1) * self._sensitivity / dt
		end
		if math.abs(self._speed) >= self._threshold then
			self:addEventListener("enter-frame", self.onEnterFrame)
			e.stop = true
		else
			e.stop = false
			self._speed = 0
		end
	end
end

function M:onMouseWheel(e)
	self._speed = e.dy * self._sensitivity * 20
	if math.abs(self._speed) >= self._threshold then
		self:addEventListener("enter-frame", self.onEnterFrame)
		e.stop = true
	else
		e.stop = false
		self._speed = 0
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.x, e.y) then
		self._touchid = e.id
		self._tx = e.x
		self._ty = e.y
		self._tt = e.time
		if not self._vertial then
			self._offset = self._view:getX()
		else
			self._offset = self._view:getY()
		end
		if math.abs(self._speed) >= self._threshold * 10000 then
			e.stop = true
		else
			e.stop = false
		end
		self._speed = 0
	end
end

function M:onTouchMove(e)
	if self._touchid == e.id then
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local s = self._sensitivity
		if not self._vertial then
			if self._view:getX() > 0 or self._view:getX() + self._view:getWidth() < self:getWidth() then
				s = s / 2
			end
			self._view:setX(self._offset + (x2 - x1) * s)
		else
			if self._view:getY() > 0 or self._view:getY() + self._view:getHeight() < self:getHeight() then
				s = s / 2
			end
			self._view:setY(self._offset + (y2 - y1) * s)
		end
		e.stop = false
		self._speed = 0
	end
end

function M:onTouchEnd(e)
	if self._touchid == e.id then
		self._touchid = nil
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local dt = (e.time - self._tt) / 1000000000 * 60
		if not self._vertial then
			self._speed = (x2 - x1) * self._sensitivity / dt
		else
			self._speed = (y2 - y1) * self._sensitivity / dt
		end
		if math.abs(self._speed) >= self._threshold then
			self:addEventListener("enter-frame", self.onEnterFrame)
			e.stop = true
		else
			e.stop = false
			self._speed = 0
		end
	end
end

function M:onEnterFrame(e)
	if math.abs(self._speed) >= self._threshold then
		if not self._vertial then
			self._speed = self._speed * self._friction
			self._view:setX(self._view:getX() + self._speed)
			if self._view:getWidth() < self:getWidth() then
				if self._view:getX() ~= 0 then
					self._view:animate({x = 0}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				end
			else
				if self._view:getX() > 0 then
					self._view:animate({x = 0}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				elseif self._view:getX() + self._view:getWidth() < self:getWidth() then
					self._view:animate({x = self:getWidth() - self._view:getWidth()}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				end
			end
		else
			self._speed = self._speed * self._friction
			self._view:setY(self._view:getY() + self._speed)
			if self._view:getHeight() < self:getHeight() then
				if self._view:getY() ~= 0 then
					self._view:animate({y = 0}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				end
			else
				if self._view:getY() > 0 then
					self._view:animate({y = 0}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				elseif self._view:getY() + self._view:getHeight() < self:getHeight() then
					self._view:animate({y = self:getHeight() - self._view:getHeight()}, 0.1, "circ-out")
					self:removeEventListener("enter-frame", self.onEnterFrame)
					self._speed = 0
				end
			end
		end
	else
		self:removeEventListener("enter-frame", self.onEnterFrame)
		self._speed = 0
	end
end

return M
);

int luaopen_display_scroll(lua_State * L)
{
	if(luaL_loadbuffer(L, display_scroll_lua, sizeof(display_scroll_lua) - 1, "DisplayScroll.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
