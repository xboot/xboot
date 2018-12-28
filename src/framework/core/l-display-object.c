/*
 * framework/core/l-display-object.c
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

#include <framework/core/l-display-object.h>

static const char display_object_lua[] = X(
local Dobject = require "graphic.dobject"
local Easing = require "graphic.easing"
local table = table

local M = Class(EventDispatcher)

function M:init(width, height, content)
	self.super:init()
	self.parent = nil
	self.children = {}
	self.dobj = Dobject.new(width, height, content)
end

function M:contains(child)
	for i, v in ipairs(self.children) do
		if v == child then
			return true
		end
	end
	return false
end

function M:getParent()
	return self.parent
end

function M:addChild(child)
	if child and child ~= self and child.parent ~= self then
		child:removeSelf()
		table.insert(self.children, child)
		child.parent = self
		self.dobj:addChild(child.dobj, true)
	end
	return self
end

function M:removeChild(child)
	if child and child ~= self then
		for i, v in ipairs(self.children) do
			if v == child then
				table.remove(self.children, i)
				v.parent = nil
				self.dobj:removeChild(v.dobj)
				break;
			end
		end
	end
	return self
end

function M:removeChildAll()
	for i, v in ipairs(self.children) do
		v.parent = nil
		self.dobj:removeChild(v.dobj)
	end
	self.children = {}
	return self
end

function M:removeSelf()
	local parent = self.parent
	if parent then
		parent:removeChild(self)
	end
	return self
end

function M:toFront()
	local parent = self.parent
	if parent then
		parent:removeChild(self)
		table.insert(parent.children, self)
		self.parent = parent
		parent.dobj:addChild(self.dobj, true)
	end
	return self
end

function M:toBack()
	local parent = self.parent
	if parent then
		parent:removeChild(self)
		table.insert(parent.children, 1, self)
		self.parent = parent
		parent.dobj:addChild(self.dobj, false)
	end
	return self
end

function M:setSize(width, height)
	self.dobj:setSize(width, height)
	return self
end

function M:getSize()
	return self.dobj:getSize()
end

function M:setX(x)
	self.dobj:setX(x)
	return self
end

function M:getX()
	return self.dobj:getX()
end

function M:setY(y)
	self.dobj:setY(y)
	return self
end

function M:getY()
	return self.dobj:getY()
end

function M:setPosition(x, y)
	self.dobj:setPosition(x, y)
	return self
end

function M:getPosition()
	return self.dobj:getPosition()
end

function M:setRotation(rotation)
	self.dobj:setRotation(rotation)
	return self
end

function M:getRotation()
	return self.dobj:getRotation()
end

function M:setScaleX(x)
	self.dobj:setScaleX(x)
	return self
end

function M:getScaleX()
	return self.dobj:getScaleX()
end

function M:setScaleY(y)
	self.dobj:setScaleY(y)
	return self
end

function M:getScaleY()
	return self.dobj:getScaleY()
end

function M:setScale(x, y)
	self.dobj:setScale(x, y or x)
	return self
end

function M:getScale()
	return self.dobj:getScale()
end

function M:setSkewX(x)
	self.dobj:setSkewX(x)
	return self
end

function M:getSkewX()
	return self.dobj:getSkewX()
end

function M:setSkewY(y)
	self.dobj:setSkewY(y)
	return self
end

function M:getSkewY()
	return self.dobj:getSkewY()
end

function M:setSkew(x, y)
	self.dobj:setSkew(x, y or x)
	return self
end

function M:getSkew()
	return self.dobj:getSkew()
end

function M:setAnchor(x, y)
	self.dobj:setAnchor(x, y or x)
	return self
end

function M:getAnchor()
	return self.dobj:getAnchor()
end

function M:setAlpha(alpha)
	self.dobj:setAlpha(alpha)
	return self
end

function M:getAlpha()
	return self.dobj:getAlpha()
end

function M:setAlignment(align)
	self.dobj:setAlignment(align)
	return self
end

function M:getAlignment()
	return self.dobj:getAlignment()
end

function M:setVisible(visible)
	self.dobj:setVisible(visible)
	return self
end

function M:getVisible()
	return self.dobj:getVisible()
end

function M:setTouchable(touchable)
	self.dobj:setTouchable(touchable)
	return self
end

function M:getTouchable()
	return self.dobj:getTouchable()
end

function M:globalToLocal(x, y)
	return self.dobj:globalToLocal(x, y)
end

function M:localToGlobal(x, y)
	return self.dobj:localToGlobal(x, y)
end

function M:hitTestPoint(x, y)
	return self.dobj:hitTestPoint(x, y)
end

function M:getBounds()
	return self.dobj:bounds()
end

function M:animate(properties, duration, easing)
	local function __animate_listener(d, e)
		if d.__animate ~= true then
			d:removeEventListener(Event.ENTER_FRAME, __animate_listener)
			d.__duration = nil
			d.__tween = nil
			d.__watch = nil
			d.__animate = nil
			return
		end

		local elapsed = d.__watch:elapsed()

		if elapsed > d.__duration then
			elapsed = d.__duration
		end

		for k, v in pairs(d.__tween) do
			if k == "x" then
				d:setX(v:easing(elapsed))
			elseif k == "y" then
				d:setY(v:easing(elapsed))
			elseif k == "rotation" then
				d:setRotation(v:easing(elapsed))
			elseif k == "scalex" then
				d:setScaleX(v:easing(elapsed))
			elseif k == "scaley" then
				d:setScaleY(v:easing(elapsed))
			elseif k == "skewx" then
				d:setSkewX(v:easing(elapsed))
			elseif k == "skewy" then
				d:setSkewY(v:easing(elapsed))
			elseif k == "alpha" then
				d:setAlpha(v:easing(elapsed))
			end
		end

		if elapsed >= d.__duration then
			d:removeEventListener(Event.ENTER_FRAME, __animate_listener)
			self:dispatchEvent(Event.new(Event.ANIMATE_COMPLETE))
			d.__duration = nil
			d.__tween = nil
			d.__watch = nil
			d.__animate = nil
		end
	end

	if self.__animate == true then
		self:removeEventListener(Event.ENTER_FRAME, __animate_listener)
		self:dispatchEvent(Event.new(Event.ANIMATE_COMPLETE))
		self.__duration = nil
		self.__tween = nil
		self.__watch = nil
		self.__animate = nil
	end

	if not properties or type(properties) ~= "table" or not next(properties) then
		return self
	end

	if duration and duration <= 0 then
		return self
	end

	self.__duration = duration or 1
	self.__tween = {}

	for k, v in pairs(properties) do
		local b = nil

		if k == "x" then
			b = self:getX()
		elseif k == "y" then
			b = self:getY()
		elseif k == "rotation" then
			b = self:getRotation()
		elseif k == "scalex" then
			b = self:getScaleX()
		elseif k == "scaley" then
			b = self:getScaleY()
		elseif k == "skewx" then
			b = self:getSkewX()
		elseif k == "skewy" then
			b = self:getSkewY()
		elseif k == "alpha" then
			b = self:getAlpha()
		end

		if b ~= nil then
			self.__tween[k] = Easing.new(b, v - b, self.__duration, easing)
		end
	end

	if not next(self.__tween) then
		self:removeEventListener(Event.ENTER_FRAME, __animate_listener)
		self.__duration = nil
		self.__tween = nil
		self.__watch = nil
		self.__animate = nil
	else
		self:addEventListener(Event.ENTER_FRAME, __animate_listener)
		self.__watch = Stopwatch.new()
		self.__animate = true
	end

	return self
end

function M:layout()
	local x1, y1, x2, y2
	for i, v in ipairs(self.children) do
		if v:getVisible() then
			x1, y1, x2, y2 = self.dobj:layout(v.dobj, x1, y1, x2, y2)
			v:layout()
		end
	end
end

function M:render(event)
	self:dispatchEvent(event)
	self.dobj:draw()

	for i, v in ipairs(self.children) do
		v:render(event)
	end
end

function M:dispatch(event)
	local children = self.children

	for i = #children, 1, -1 do
		children[i]:dispatch(event)
	end

	self:dispatchEvent(event)
end

return M
);

int luaopen_display_object(lua_State * L)
{
	if(luaL_loadbuffer(L, display_object_lua, sizeof(display_object_lua) - 1, "DisplayObject.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
