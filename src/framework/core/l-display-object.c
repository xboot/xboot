/*
 * framework/core/l-display-object.c
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

#include <framework/core/l-display-object.h>

static const char display_object_lua[] = X(
local Dobject = Dobject
local Easing = Easing
local Event = Event
local table = table

local M = Class(EventDispatcher)

function M:init(width, height, content)
	self.super:init()
	self.__parent = nil
	self.__children = {}
	self.__dobj = Dobject.new(width, height, content)
end

function M:contains(child)
	for i, v in ipairs(self.__children) do
		if v == child then
			return true
		end
	end
	return false
end

function M:getParent()
	return self.__parent
end

function M:addChild(child)
	if child and child ~= self and child.__parent ~= self then
		child:removeSelf()
		table.insert(self.__children, child)
		child.__parent = self
		self.__dobj:addChild(child.__dobj, true)
	end
	return self
end

function M:removeChild(child)
	if child and child ~= self then
		for i, v in ipairs(self.__children) do
			if v == child then
				table.remove(self.__children, i)
				v.__parent = nil
				self.__dobj:removeChild(v.__dobj)
				break;
			end
		end
	end
	return self
end

function M:removeChildren()
	for i, v in ipairs(self.__children) do
		v.__parent = nil
		self.__dobj:removeChild(v.__dobj)
	end
	self.__children = {}
	return self
end

function M:removeSelf()
	local parent = self.__parent
	if parent then
		parent:removeChild(self)
	end
	return self
end

function M:toFront()
	local parent = self.__parent
	if parent then
		parent:removeChild(self)
		table.insert(parent.__children, self)
		self.__parent = parent
		parent.__dobj:addChild(self.__dobj, true)
	end
	return self
end

function M:toBack()
	local parent = self.__parent
	if parent then
		parent:removeChild(self)
		table.insert(parent.__children, 1, self)
		self.__parent = parent
		parent.__dobj:addChild(self.__dobj, false)
	end
	return self
end

function M:setWidth(width)
	self.__dobj:setWidth(width)
	return self
end

function M:getWidth()
	return self.__dobj:getWidth()
end

function M:setHeight(height)
	self.__dobj:setHeight(height)
	return self
end

function M:getHeight()
	return self.__dobj:getHeight()
end

function M:setSize(width, height)
	self.__dobj:setSize(width, height)
	return self
end

function M:getSize()
	return self.__dobj:getSize()
end

function M:setX(x)
	self.__dobj:setX(x)
	return self
end

function M:getX()
	return self.__dobj:getX()
end

function M:setY(y)
	self.__dobj:setY(y)
	return self
end

function M:getY()
	return self.__dobj:getY()
end

function M:setPosition(x, y)
	self.__dobj:setPosition(x, y)
	return self
end

function M:getPosition()
	return self.__dobj:getPosition()
end

function M:setRotation(rotation)
	self.__dobj:setRotation(rotation)
	return self
end

function M:getRotation()
	return self.__dobj:getRotation()
end

function M:setScaleX(x)
	self.__dobj:setScaleX(x)
	return self
end

function M:getScaleX()
	return self.__dobj:getScaleX()
end

function M:setScaleY(y)
	self.__dobj:setScaleY(y)
	return self
end

function M:getScaleY()
	return self.__dobj:getScaleY()
end

function M:setScale(x, y)
	self.__dobj:setScale(x, y or x)
	return self
end

function M:getScale()
	return self.__dobj:getScale()
end

function M:setSkewX(x)
	self.__dobj:setSkewX(x)
	return self
end

function M:getSkewX()
	return self.__dobj:getSkewX()
end

function M:setSkewY(y)
	self.__dobj:setSkewY(y)
	return self
end

function M:getSkewY()
	return self.__dobj:getSkewY()
end

function M:setSkew(x, y)
	self.__dobj:setSkew(x, y or x)
	return self
end

function M:getSkew()
	return self.__dobj:getSkew()
end

function M:setAnchor(x, y)
	self.__dobj:setAnchor(x, y or x)
	return self
end

function M:getAnchor()
	return self.__dobj:getAnchor()
end

function M:setAlpha(alpha)
	self.__dobj:setAlpha(alpha)
	return self
end

function M:getAlpha()
	return self.__dobj:getAlpha()
end

function M:setAlignment(align)
	self.__dobj:setAlignment(align)
	return self
end

function M:getAlignment()
	return self.__dobj:getAlignment()
end

function M:setMargin(left, top, right, bottom)
	self.__dobj:setMargin(left, top, right, bottom)
	return self
end

function M:getMargin()
	return self.__dobj:getMargin()
end

function M:setCollider(type, ...)
	self.__dobj:setCollider(type, ...)
	return self
end

function M:getCollider()
	return self.__dobj:getCollider()
end

function M:setVisible(visible)
	self.__dobj:setVisible(visible)
	return self
end

function M:getVisible()
	return self.__dobj:getVisible()
end

function M:setTouchable(touchable)
	self.__dobj:setTouchable(touchable)
	return self
end

function M:getTouchable()
	return self.__dobj:getTouchable()
end

function M:globalToLocal(x, y)
	return self.__dobj:globalToLocal(x, y)
end

function M:localToGlobal(x, y)
	return self.__dobj:localToGlobal(x, y)
end

function M:hitTestPoint(x, y)
	return self.__dobj:hitTestPoint(x, y)
end

function M:getBounds()
	return self.__dobj:bounds()
end

function M:animate(properties, duration, easing)
	local function listener(d, e)
		if next(d.__tweenlist) then
			local tween = d.__tweenlist[1]
			if not tween.easinglist then
				tween.easinglist = {}
				for k, v in pairs(tween.properties) do
					local b = nil
					if k == "x" then
						b = d:getX()
					elseif k == "y" then
						b = d:getY()
					elseif k == "rotation" then
						b = d:getRotation()
					elseif k == "scalex" then
						b = d:getScaleX()
					elseif k == "scaley" then
						b = d:getScaleY()
					elseif k == "skewx" then
						b = d:getSkewX()
					elseif k == "skewy" then
						b = d:getSkewY()
					elseif k == "alpha" then
						b = d:getAlpha()
					elseif k == "width" then
						b = d:getWidth()
					elseif k == "height" then
						b = d:getHeight()
					end
					if b ~= nil then
						tween.easinglist[k] = Easing.new(b, v - b, tween.duration, tween.easing)
					end
				end
			end

			local elapsed = d.__stopwatch:elapsed()
			if elapsed > tween.duration then
				elapsed = tween.duration
			end

			for k, v in pairs(tween.easinglist) do
				if k == "x" then
					d:setX(v(elapsed))
				elseif k == "y" then
					d:setY(v(elapsed))
				elseif k == "rotation" then
					d:setRotation(v(elapsed))
				elseif k == "scalex" then
					d:setScaleX(v(elapsed))
				elseif k == "scaley" then
					d:setScaleY(v(elapsed))
				elseif k == "skewx" then
					d:setSkewX(v(elapsed))
				elseif k == "skewy" then
					d:setSkewY(v(elapsed))
				elseif k == "alpha" then
					d:setAlpha(v(elapsed))
				elseif k == "width" then
					d:setWidth(v(elapsed))
				elseif k == "height" then
					d:setHeight(v(elapsed))
				end
			end

			if elapsed >= tween.duration then
				table.remove(d.__tweenlist, 1)
				d.__stopwatch:reset()
			end
		else
			d:dispatchEvent(Event.new(Event.ANIMATE_COMPLETE))
			if not next(d.__tweenlist) then
				d:removeEventListener(Event.ENTER_FRAME, listener)
				d.__stopwatch = nil
			end
		end
	end

	if not properties or type(properties) ~= "table" or not next(properties) then
		return self
	end
	if duration and duration <= 0 then
		return self
	end
	if not self.__tweenlist then
		self.__tweenlist = {}
	end

	local tween = {properties = {}, duration = duration or 1, easing = easing}
	for k, v in pairs(properties) do
		tween.properties[k] = v
	end
	table.insert(self.__tweenlist, tween)

	if next(self.__tweenlist) and not self.__stopwatch then
		self:addEventListener(Event.ENTER_FRAME, listener)
		self.__stopwatch = Stopwatch.new()
	end
	return self
end

function M:layout()
	local x1, y1, x2, y2
	for i, v in ipairs(self.__children) do
		if v:getVisible() then
			x1, y1, x2, y2 = self.__dobj:layout(v.__dobj, x1, y1, x2, y2)
			v:layout()
		end
	end
end

function M:render(event)
	self:dispatchEvent(event)
	self.__dobj:draw()

	for i, v in ipairs(self.__children) do
		v:render(event)
	end
end

function M:dispatch(event)
	local children = self.__children

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
