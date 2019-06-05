/*
 * framework/core/l-display-pager.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

#include <framework/core/l-display-pager.h>

static const char display_pager_lua[] = X(
local M = Class(DisplayObject)

function M:init(width, height, vertial)
	self.super:init(width, height)
	self._vertial = vertial == nil and true or vertial
	self._pagers = {}
	self._index = 0
	self._page = nil
	self._next = nil
	self._pageSpring = nil
	self._nextSpring = nil
	self._pwatch = nil
	self._action = nil
	self._springing = false

	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)
	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
	self:addEventListener("key-down", self.onKeyDown)
end

function M:addPage(page)
	if page and page ~= self then
		for i, v in ipairs(self._pagers) do
			if v == page then
				table.remove(self._pagers, i)
				if self:contains(page) then
					self:removeChild(page)
				end
				break
			end
		end
		table.insert(self._pagers, page)
		if #self._pagers > 0 then
			if self._index < 1 then
				self._index = 1
			elseif self._index > #self._pagers then
				self._index = #self._pagers
			end
			self._page = self._pagers[self._index]
		else
			self._index = 0
			self._page = nil
		end
		if self._page and not self:contains(self._page) then
			self:addChild(self._page)
		end
	end
	return self
end

function M:removePage(page)
	if page and page ~= self then
		for i, v in ipairs(self._pagers) do
			if v == page then
				table.remove(self._pagers, i)
				if self:contains(page) then
					self:removeChild(page)
				end
				if #self._pagers > 0 then
					if self._index < 1 then
						self._index = 1
					elseif self._index > #self._pagers then
						self._index = #self._pagers
					end
					self._page = self._pagers[self._index]
				else
					self._index = 0
					self._page = nil
				end
				if self._page and not self:contains(self._page) then
					self:addChild(self._page)
				end
				break
			end
		end
	end
	return self
end

function M:clear()
	self:removeChildren()
	self._pagers = {}
	self._index = 0
	self._page = nil
	return self
end

function M:onMouseDown(e)
	if not self._springing and self._page and self:hitTestPoint(e.x, e.y) then
		self._touchid = -1
		self._tx = e.x
		self._ty = e.y
		self._tt = e.time
	end
end

function M:onMouseMove(e)
	if self._touchid == -1 then
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		if not self._vertial then
			if self._index == 1 and self._page:getX() > 0 then
				self._page:setX((x2 - x1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			elseif self._index == #self._pagers and self._page:getX() < 0 then
				self._page:setX((x2 - x1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			else
				self._page:setX(x2 - x1)
				if self._page:getX() > 0 and self._index > 1 then
					self._next = self._pagers[self._index - 1]
					if self._next then
						self._next:setX(self._page:getX() - self._page:getWidth())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				elseif self._page:getX() < 0 and self._index < #self._pagers then
					self._next = self._pagers[self._index + 1]
					if self._next then
						self._next:setX(self._page:getX() + self._page:getWidth())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				else
					if self:contains(self._next) then
						self:removeChild(self._next)
						self._next = nil
					end
				end
			end
		else
			if self._index == 1 and self._page:getY() > 0 then
				self._page:setY((y2 - y1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			elseif self._index == #self._pagers and self._page:getY() < 0 then
				self._page:setY((y2 - y1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			else
				self._page:setY(y2 - y1)
				if self._page:getY() > 0 and self._index > 1 then
					self._next = self._pagers[self._index - 1]
					if self._next then
						self._next:setY(self._page:getY() - self._page:getHeight())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				elseif self._page:getY() < 0 and self._index < #self._pagers then
					self._next = self._pagers[self._index + 1]
					if self._next then
						self._next:setY(self._page:getY() + self._page:getHeight())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				else
					if self:contains(self._next) then
						self:removeChild(self._next)
						self._next = nil
					end
				end
			end
		end
	end
end

function M:onMouseUp(e)
	if self._touchid == -1 then
		self._touchid = nil
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local speed
		if not self._vertial then
			speed = (x1 - x2) * 1000000000 / (e.time - self._tt)
		else
			speed = (y1 - y2) * 1000000000 / (e.time - self._tt)
		end
		if speed < -100 and self._index > 1 then
			self:prevPage()
		elseif speed > 100 and self._index < #self._pagers then
			self:nextPage()
		else
			self:restore()
		end
	end
end

function M:onTouchBegin(e)
	if not self._springing and self._page and self:hitTestPoint(e.x, e.y) then
		self._touchid = e.id
		self._tx = e.x
		self._ty = e.y
		self._tt = e.time
	end
end

function M:onTouchMove(e)
	if self._touchid == e.id then
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		if not self._vertial then
			if self._index == 1 and self._page:getX() > 0 then
				self._page:setX((x2 - x1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			elseif self._index == #self._pagers and self._page:getX() < 0 then
				self._page:setX((x2 - x1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			else
				self._page:setX(x2 - x1)
				if self._page:getX() > 0 and self._index > 1 then
					self._next = self._pagers[self._index - 1]
					if self._next then
						self._next:setX(self._page:getX() - self._page:getWidth())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				elseif self._page:getX() < 0 and self._index < #self._pagers then
					self._next = self._pagers[self._index + 1]
					if self._next then
						self._next:setX(self._page:getX() + self._page:getWidth())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				else
					if self:contains(self._next) then
						self:removeChild(self._next)
						self._next = nil
					end
				end
			end
		else
			if self._index == 1 and self._page:getY() > 0 then
				self._page:setY((y2 - y1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			elseif self._index == #self._pagers and self._page:getY() < 0 then
				self._page:setY((y2 - y1) / 3)
				if self:contains(self._next) then
					self:removeChild(self._next)
					self._next = nil
				end
			else
				self._page:setY(y2 - y1)
				if self._page:getY() > 0 and self._index > 1 then
					self._next = self._pagers[self._index - 1]
					if self._next then
						self._next:setY(self._page:getY() - self._page:getHeight())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				elseif self._page:getY() < 0 and self._index < #self._pagers then
					self._next = self._pagers[self._index + 1]
					if self._next then
						self._next:setY(self._page:getY() + self._page:getHeight())
						if not self:contains(self._next) then
							self:addChild(self._next)
						end
					end
				else
					if self:contains(self._next) then
						self:removeChild(self._next)
						self._next = nil
					end
				end
			end
		end
	end
end

function M:onTouchEnd(e)
	if self._touchid == e.id then
		self._touchid = nil
		local x1, y1 = self:globalToLocal(self._tx, self._ty)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		local speed
		if not self._vertial then
			speed = (x1 - x2) * 1000000000 / (e.time - self._tt)
		else
			speed = (y1 - y2) * 1000000000 / (e.time - self._tt)
		end
		if speed < -100 and self._index > 1 then
			self:prevPage()
		elseif speed > 100 and self._index < #self._pagers then
			self:nextPage()
		else
			self:restore()
		end
	end
end

function M:onKeyDown(e)
	if e.key == 2 or e.key == 4 then
		self:prevPage()
		e.stop = true
	elseif e.key == 3 or e.key == 5 then
		self:nextPage()
		e.stop = true
	end
end

function M:restore(flag)
	if not self._springing and self._index > 0 then
		if not self._vertial then
			if self._page:getX() ~= 0 then
				self._pageSpring = Spring.new(self._page:getX(), 0, 0, 1500, 50)
				if self._next then
					if self._page:getX() > 0 then
						self._nextSpring = Spring.new(self._page:getX() - self._next:getWidth(), -self._next:getWidth(), 0, 1500, 50)
					else
						self._nextSpring = Spring.new(self._page:getX() + self._page:getWidth(), self._page:getWidth(), 0, 1500, 50)
					end
				end
				self._pwatch = Stopwatch.new()
				self._action = nil
				self._springing = true
				self:addEventListener("enter-frame", self.onEnterFrame)
			end
		else
			if self._page:getY() ~= 0 then
				self._pageSpring = Spring.new(self._page:getY(), 0, 0, 1500, 50)
				if self._next then
					if self._page:getY() > 0 then
						self._nextSpring = Spring.new(self._page:getY() - self._next:getWidth(), -self._next:getWidth(), 0, 1500, 50)
					else
						self._nextSpring = Spring.new(self._page:getY() + self._page:getWidth(), self._page:getWidth(), 0, 1500, 50)
					end
				end
				self._pwatch = Stopwatch.new()
				self._action = nil
				self._springing = true
				self:addEventListener("enter-frame", self.onEnterFrame)
			end
		end
	end
end

function M:prevPage()
	if not self._springing and self._index > 1 and self._index <= #self._pagers then
		self._page = self._pagers[self._index]
		self._next = self._pagers[self._index - 1]
		self._index = self._index - 1
		if self._index < 1 then
			self._index = 1
		elseif self._index > #self._pagers then
			self._index = #self._pagers
		end
		if self._page and not self:contains(self._page) then
			self:addChild(self._page)
		end
		if self._next and not self:contains(self._next) then
			self:addChild(self._next)
		end
		if not self._vertial then
			self._pageSpring = Spring.new(self._page:getX(), self._next:getWidth(), 0, 1500, 50)
			self._nextSpring = Spring.new(self._page:getX() - self._next:getWidth(), 0, 0, 1500, 50)
		else
			self._pageSpring = Spring.new(self._page:getY(), self._next:getHeight(), 0, 1500, 50)
			self._nextSpring = Spring.new(self._page:getY() - self._next:getHeight(), 0, 0, 1500, 50)
		end
		self._pwatch = Stopwatch.new()
		self._action = "prev-page"
		self._springing = true
		self:addEventListener("enter-frame", self.onEnterFrame)
	end
end

function M:nextPage()
	if not self._springing and self._index > 0 and self._index < #self._pagers then
		self._page = self._pagers[self._index]
		self._next = self._pagers[self._index + 1]
		self._index = self._index + 1
		if self._index < 1 then
			self._index = 1
		elseif self._index > #self._pagers then
			self._index = #self._pagers
		end
		if self._page and not self:contains(self._page) then
			self:addChild(self._page)
		end
		if self._next and not self:contains(self._next) then
			self:addChild(self._next)
		end
		if not self._vertial then
			self._pageSpring = Spring.new(self._page:getX(), -self._page:getWidth(), 0, 1500, 50)
			self._nextSpring = Spring.new(self._page:getX() + self._page:getWidth(), 0, 0, 1500, 50)
		else
			self._pageSpring = Spring.new(self._page:getY(), -self._page:getHeight(), 0, 1500, 50)
			self._nextSpring = Spring.new(self._page:getY() + self._page:getHeight(), 0, 0, 1500, 50)
		end
		self._pwatch = Stopwatch.new()
		self._action = "next-page"
		self._springing = true
		self:addEventListener("enter-frame", self.onEnterFrame)
	end
end

function M:onEnterFrame(e)
	local delta = self._pwatch:elapsed()
	self._pwatch:reset()
	local pflag, pns, pnv = self._pageSpring(delta)
	if pflag then
		if not self._vertial then
			self._page:setX(pns)
		else
			self._page:setY(pns)
		end
	end
	if self._next then
		local nflag, nns, nnv = self._nextSpring(delta)
		if nflag then
			if not self._vertial then
				self._next:setX(nns)
			else
				self._next:setY(nns)
			end
		end
	end
	if not pflag and not nflag then
		if self._action == "prev-page" or self._action == "next-page" then
			if self._page and self:contains(self._page) then
				self:removeChild(self._page)
			end
			if self._next and not self:contains(self._next) then
				self:addChild(self._next)
			end
			self._page = self._next
			self._next = nil
		else
			if self._page and not self:contains(self._page) then
				self:addChild(self._page)
			end
			if self._next and self:contains(self._next) then
				self:removeChild(self._next)
			end
			self._next = nil
		end
		self._pageSpring = nil
		self._nextSpring = nil
		self._pwatch = nil
		self._action = nil
		self._springing = false
		self:removeEventListener("enter-frame", self.onEnterFrame)
	end
end

return M
);

int luaopen_display_pager(lua_State * L)
{
	if(luaL_loadbuffer(L, display_pager_lua, sizeof(display_pager_lua) - 1, "DisplayPager.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
