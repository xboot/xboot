/*
 * framework/core/l-display-object.c
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

#include <framework/core/l-display-object.h>

static const char display_object_lua[] = X(
local Dobject = Dobject
local Easing = Easing
local Event = Event
local table = table

local M = Class(EventDispatcher)

function M:init(width, height, content)
	self.super:init()
	self._parent = nil
	self._children = {}
	self._dobj = Dobject.new(width, height, content)
end

function M:getParent()
	return self._parent
end

function M:contains(child)
	if child and child._parent == self then
		return true
	end
	return false
end

function M:addChild(child)
	if child and child ~= self and child._parent ~= self then
		if child._parent ~= nil then
			for i, v in ipairs(child._parent) do
				if v == child then
					table.remove(self._children, i)
					break
				end
			end
		end
		table.insert(self._children, child)
		child._parent = self
		self._dobj:addChild(child._dobj)
	end
	return self
end

function M:removeChild(child)
	if child and child ~= self then
		for i, v in ipairs(self._children) do
			if v == child then
				table.remove(self._children, i)
				v._parent = nil
				break
			end
		end
		self._dobj:removeChild(child._dobj)
	end
	return self
end

function M:removeChildren()
	for i, v in ipairs(self._children) do
		v._parent = nil
		self._dobj:removeChild(v._dobj)
	end
	self._children = {}
	return self
end

function M:removeSelf()
	local parent = self._parent
	if parent then
		parent:removeChild(self)
	end
	return self
end

function M:toFront()
	local parent = self._parent
	if parent then
		local children = parent._children
		for i, v in ipairs(children) do
			if v == self then
				table.remove(children, i)
				table.insert(children, self)
				break
			end
		end
		self._dobj:toFront()
	end
	return self
end

function M:toBack()
	local parent = self._parent
	if parent then
		local children = parent._children
		for i, v in ipairs(children) do
			if v == self then
				table.remove(children, i)
				table.insert(children, 1, self)
				break
			end
		end
		self._dobj:toBack()
	end
	return self
end

function M:setWidth(width)
	self._dobj:setWidth(width)
	return self
end

function M:getWidth()
	return self._dobj:getWidth()
end

function M:setHeight(height)
	self._dobj:setHeight(height)
	return self
end

function M:getHeight()
	return self._dobj:getHeight()
end

function M:setSize(width, height)
	self._dobj:setSize(width, height)
	return self
end

function M:getSize()
	return self._dobj:getSize()
end

function M:setX(x)
	self._dobj:setX(x)
	return self
end

function M:getX()
	return self._dobj:getX()
end

function M:setY(y)
	self._dobj:setY(y)
	return self
end

function M:getY()
	return self._dobj:getY()
end

function M:setPosition(x, y)
	self._dobj:setPosition(x, y)
	return self
end

function M:getPosition()
	return self._dobj:getPosition()
end

function M:setRotation(rotation)
	self._dobj:setRotation(rotation)
	return self
end

function M:getRotation()
	return self._dobj:getRotation()
end

function M:setScaleX(x)
	self._dobj:setScaleX(x)
	return self
end

function M:getScaleX()
	return self._dobj:getScaleX()
end

function M:setScaleY(y)
	self._dobj:setScaleY(y)
	return self
end

function M:getScaleY()
	return self._dobj:getScaleY()
end

function M:setScale(x, y)
	self._dobj:setScale(x, y or x)
	return self
end

function M:getScale()
	return self._dobj:getScale()
end

function M:setSkewX(x)
	self._dobj:setSkewX(x)
	return self
end

function M:getSkewX()
	return self._dobj:getSkewX()
end

function M:setSkewY(y)
	self._dobj:setSkewY(y)
	return self
end

function M:getSkewY()
	return self._dobj:getSkewY()
end

function M:setSkew(x, y)
	self._dobj:setSkew(x, y or x)
	return self
end

function M:getSkew()
	return self._dobj:getSkew()
end

function M:setAnchor(x, y)
	self._dobj:setAnchor(x, y or x)
	return self
end

function M:getAnchor()
	return self._dobj:getAnchor()
end

function M:setBackgroundColor(color)
	self._dobj:setBackgroundColor(color)
	return self
end

function M:getBackgroundColor()
	return self._dobj:getBackgroundColor()
end

function M:setLayoutEnable(enable)
	self._dobj:setLayoutEnable(enable)
	return self
end

function M:getLayoutEnable()
	return self._dobj:getLayoutEnable()
end

function M:setLayoutSpecial(enable)
	self._dobj:setLayoutSpecial(enable)
	return self
end

function M:getLayoutSpecial()
	return self._dobj:getLayoutSpecial()
end

function M:setLayoutDirection(direction)
	self._dobj:setLayoutDirection(direction)
	return self
end

function M:getLayoutDirection()
	return self._dobj:getLayoutDirection()
end

function M:setLayoutJustify(justify)
	self._dobj:setLayoutJustify(justify)
	return self
end

function M:getLayoutJustify()
	return self._dobj:getLayoutJustify()
end

function M:setLayoutAlign(align)
	self._dobj:setLayoutAlign(align)
	return self
end

function M:getLayoutAlign()
	return self._dobj:getLayoutAlign()
end

function M:setLayoutAlignSelf(align)
	self._dobj:setLayoutAlignSelf(align)
	return self
end

function M:getLayoutAlignSelf()
	return self._dobj:getLayoutAlignSelf()
end

function M:setLayoutGrow(grow)
	self._dobj:setLayoutGrow(grow)
	return self
end

function M:getLayoutGrow()
	return self._dobj:getLayoutGrow()
end

function M:setLayoutShrink(shrink)
	self._dobj:setLayoutShrink(shrink)
	return self
end

function M:getLayoutShrink()
	return self._dobj:getLayoutShrink()
end

function M:setLayoutBasis(basis)
	self._dobj:setLayoutBasis(basis)
	return self
end

function M:getLayoutBasis()
	return self._dobj:getLayoutBasis()
end

function M:setLayoutMargin(left, top, right, bottom)
	self._dobj:setLayoutMargin(left, top, right, bottom)
	return self
end

function M:getLayoutMargin()
	return self._dobj:getLayoutMargin()
end

function M:setCollider(type, ...)
	self._dobj:setCollider(type, ...)
	return self
end

function M:getCollider()
	return self._dobj:getCollider()
end

function M:setVisible(visible)
	self._dobj:setVisible(visible)
	return self
end

function M:getVisible()
	return self._dobj:getVisible()
end

function M:setTouchable(touchable)
	self._dobj:setTouchable(touchable)
	return self
end

function M:getTouchable()
	return self._dobj:getTouchable()
end

function M:globalToLocal(x, y)
	return self._dobj:globalToLocal(x, y)
end

function M:localToGlobal(x, y)
	return self._dobj:localToGlobal(x, y)
end

function M:hitTestPoint(x, y)
	return self._dobj:hitTestPoint(x, y)
end

function M:markDirty()
	return self._dobj:markDirty()
end

function M:getBounds()
	return self._dobj:getBounds()
end

function M:animate(properties, duration, easing)
	local function listener(d, e)
		if next(d._tweenlist) then
			local tween = d._tweenlist[1]
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

			local elapsed = d._awatch:elapsed()
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
				elseif k == "width" then
					d:setWidth(v(elapsed))
				elseif k == "height" then
					d:setHeight(v(elapsed))
				end
			end

			if elapsed >= tween.duration then
				table.remove(d._tweenlist, 1)
				d._awatch:reset()
			end
		else
			d:dispatchEvent(Event.new("animate-complete"))
			if not next(d._tweenlist) then
				d:removeEventListener("enter-frame", listener)
				d._awatch = nil
			end
		end
	end

	if not properties or type(properties) ~= "table" or not next(properties) then
		return self
	end
	if duration and duration <= 0 then
		return self
	end
	if not self._tweenlist then
		self._tweenlist = {}
	end

	local tween = {properties = {}, duration = duration or 1, easing = easing}
	for k, v in pairs(properties) do
		tween.properties[k] = v
	end
	table.insert(self._tweenlist, tween)

	if next(self._tweenlist) and not self._awatch then
		self:addEventListener("enter-frame", listener)
		self._awatch = Stopwatch.new()
	end
	return self
end

function M:spring(properties, velocity, stiffness, damping)
	local function listener(d, e)
		if d._spring and type(d._spring) == "table" and next(d._spring) then
			local complete = true
			local delta = d._swatch:elapsed()
			d._swatch:reset()

			for k, v in pairs(d._spring) do
				local flag, ns, nv = v(delta)
				if flag then
					if k == "x" then
						d:setX(ns)
					elseif k == "y" then
						d:setY(ns)
					elseif k == "rotation" then
						d:setRotation(ns)
					elseif k == "scalex" then
						d:setScaleX(ns)
					elseif k == "scaley" then
						d:setScaleY(ns)
					elseif k == "skewx" then
						d:setSkewX(ns)
					elseif k == "skewy" then
						d:setSkewY(ns)
					elseif k == "width" then
						d:setWidth(ns)
					elseif k == "height" then
						d:setHeight(ns)
					end
					complete = false
				end
			end
			if complete then
				d:removeEventListener("enter-frame", listener)
				d:dispatchEvent(Event.new("animate-complete"))
				d._spring = nil
				d._swatch = nil
			end
		else
			d:removeEventListener("enter-frame", listener)
			d._spring = nil
			d._swatch = nil
		end
	end

	self._spring = nil
	if properties and type(properties) == "table" and next(properties) then
		self._spring = {}
		for k, v in pairs(properties) do
			local s = nil
			if k == "x" then
				s = self:getX()
			elseif k == "y" then
				s = self:getY()
			elseif k == "rotation" then
				s = self:getRotation()
			elseif k == "scalex" then
				s = self:getScaleX()
			elseif k == "scaley" then
				s = self:getScaleY()
			elseif k == "skewx" then
				s = self:getSkewX()
			elseif k == "skewy" then
				s = self:getSkewY()
			elseif k == "width" then
				s = self:getWidth()
			elseif k == "height" then
				s = self:getHeight()
			end
			if s ~= nil then
				self._spring[k] = Spring.new(s, v, velocity, stiffness, damping)
			end
		end
		if next(self._spring) and not self._swatch then
			self:addEventListener("enter-frame", listener)
			self._swatch = Stopwatch.new()
		end
	end
	return self
end

function M:dispatch(event)
	self:dispatchEvent(event)
	local children = self._children
	for i = #children, 1, -1 do
		children[i]:dispatch(event)
	end
end

function M:render(display)
	self._dobj:render(display)
end

return M
);

int luaopen_display_object(lua_State * L)
{
	if(luaL_loadbuffer(L, display_object_lua, sizeof(display_object_lua) - 1, "DisplayObject.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
