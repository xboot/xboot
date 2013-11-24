---
-- The 'DisplayObject' class is the base class for all objects that can
-- be placed on the screen.
--
-- @module DisplayObject
local M = Class(EventDispatcher)

---
-- Creates a new display object.
--
-- @function [parent=#DisplayObject] new
-- @return #DisplayObject
function M:init()
	self.super:init()

	self.parent = nil
	self.children = {}
	self.object = Object.new()
end

---
-- Determines whether the specified display object is contained in the subtree of
-- this 'DisplayObject' instance.
--
-- @function [parent=#DisplayObject] contains
-- @param self
-- @param child (DisplayObject) The child object to test.
-- @return 'true' if the child object is contained in the subtree of this 'DisplayObject'
-- instance, otherwise 'false'.
function M:contains(child)
	for i, v in ipairs(self.children) do
		if v == child then
			return true
		end
	end

	return false
end

---
-- Adds a display object as a child to this display object. The child
-- is added as a last child of this 'DisplayObject' instance.
--
-- Display object can have only one parent. Therefore if you add a child
-- object that already has a different display object as a parent, the
-- display object is removed from the child list of the other display
-- object and then added to this display object.
--
-- @function [parent=#DisplayObject] addChild
-- @param self
-- @param child (DisplayObject) The child display object to add.
-- @return A value of 'true' or 'false'.
function M:addChild(child)
	if child == nil or self == child then
		return false
	end

	if child.parent == self then
		return false
	end

	child:removeSelf()
	table.insert(self.children, child)
	child.parent = self

	return true
end

---
-- Removes the specified child 'DisplayObject' instance from the child list
-- of this 'DisplayObject' instance.
--
-- @function [parent=#DisplayObject] removeChild
-- @param self
-- @param child (DisplayObject) The child display object to remove.
-- @return A value of 'true' or 'false'.
function M:removeChild(child)
	if child == nil or self == child then
		return false
	end

	local index = 0

	for i, v in ipairs(self.children) do
		if v == child then
			index = i
			break
		end
	end

	if index <= 0 then
		return false
	end

	table.remove(self.children, index)
	child.parent = nil

	return true
end

---
-- If the display object has a parent, removes the display object from the
-- child list of its parent display object.
--
-- @function [parent=#DisplayObject] removeSelf
-- @param self
-- @return A value of 'true' or 'false'.
function M:removeSelf()
	local parent = self.parent

	if parent == nil then
		return false
	end

	return parent:removeChild(self)
end

---
-- Moves the display object to the visual front of its parent.
--
-- @function [parent=#DisplayObject] toFront
-- @param self
-- @return A value of 'true' or 'false'.
function M:toFront()
	local parent = self.parent

	if parent == nil then
		return false
	end

	if not parent:removeChild(self) then
		return false
	end

	table.insert(parent.children, self)
	self.parent = parent

	return true
end

---
-- Moves the display object to the visual back of its parent.
--
-- @function [parent=#DisplayObject] toBack
-- @param self
-- @return A value of 'true' or 'false'.
function M:toBack()
	local parent = self.parent

	if parent == nil then
		return false
	end

	if not parent:removeChild(self) then
		return false
	end

	table.insert(parent.children, 1, self)
	self.parent = parent

	return true
end

---
-- Sets the x coordinates of the display object.
--
-- @function [parent=#DisplayObject] setX
-- @param self
-- @param x (number) The new x coordinate of the display object.
function M:setX(x)
	self.object:setX(x)
	return self
end

---
-- Returns the x coordinate of the display object.
--
-- @function [parent=#DisplayObject] getX
-- @param self
-- @return The x coordinate of the display object.
function M:getX()
	return self.object:getX()
end

---
-- Sets the y coordinates of the display object.
--
-- @function [parent=#DisplayObject] setY
-- @param self
-- @param y (number) The new y coordinate of the display object.
function M:setY(y)
	self.object:setY(y)
	return self
end

---
-- Returns the y coordinate of the display object.
--
-- @function [parent=#DisplayObject] getY
-- @param self
-- @return The y coordinate of the display object.
function M:getY()
	return self.object:getY()
end

---
-- Sets the x and y coordinates of the display object.
--
-- @function [parent=#DisplayObject] setPosition
-- @param self
-- @param x (number) The new x coordinate of the display object.
-- @param y (number) The new y coordinate of the display object.
function M:setPosition(x, y)
	self.object:setPosition(x, y)
	return self
end

---
-- Returns the x and y coordinates of the display object.
--
-- @function [parent=#DisplayObject] getPosition
-- @param self
-- @return The x and y coordinates of the display object.
function M:getPosition()
	return self.object:getPosition()
end

---
-- Sets the rotation of the display object in degrees.
--
-- @function [parent=#DisplayObject] setRotation
-- @param self
-- @param rotation (number) rotation of the display object
function M:setRotation(rotation)
	self.object:setRotation(rotation)
	return self
end

---
-- Returns the rotation of the display object in degrees.
--
-- @function [parent=#DisplayObject] getRotation
-- @param self
-- @return Rotation of the display object.
function M:getRotation()
	return self.object:getRotation()
end

---
-- Sets the horizontal scale of the display object.
--
-- @function [parent=#DisplayObject] setScaleX
-- @param self
-- @param x (number) horizontal scale of the display object
function M:setScaleX(x)
	self.object:setScaleX(x)
	return self
end

---
-- Returns the horizontal scale of the display object.
--
-- @function [parent=#DisplayObject] getScaleX
-- @param self
-- @return The horizontal scale (percentage) of the display object.
function M:getScaleX()
	return self.object:getScaleX()
end

---
-- Sets the vertical scale of the display object.
--
-- @function [parent=#DisplayObject] setScaleY
-- @param self
-- @param y (number) vertical scale of the display object
function M:setScaleY(y)
	self.object:setScaleY(y)
	return self
end

---
-- Returns the vertical scale of the display object.
--
-- @function [parent=#DisplayObject] getScaleY
-- @param self
-- @return The vertical scale of the display object.
function M:getScaleY()
	return self.object:getScaleY()
end

---
-- Sets the horizontal and vertical scales of the display object.
--
-- @function [parent=#DisplayObject] setScale
-- @param self
-- @param x (number) horizontal scale (percentage) of the display object
-- @param y (number) vertical scale (percentage) of the display object
function M:setScale(x, y)
	self.object:setScale(x, y or x)
	return self
end

---
-- Returns the horizontal and vertical scales of the display object.
--
-- @function [parent=#DisplayObject] getScale
-- @param self
-- @return The horizontal and vertical scales of the display object
function M:getScale()
	return self.object:getScale()
end

---
-- Sets the anchor point of the display object in percentage.
--
-- @function [parent=#DisplayObject] setAnchor
-- @param self
-- @param x (number) The horizontal percentage of anchor point.
-- @param y (number) The vertical percentage of anchor point.
function M:setAnchor(x, y)
	self.object:setAnchor(x, y or x)
	return self
end

---
-- Returns the anchor point of the display object in percentage.
--
-- @function [parent=#DisplayObject] getAnchor
-- @param self
-- @return The anchor point of the display object in percentage.
function M:getAnchor()
	return self.object:getAnchor()
end

---
-- Sets the alpha transparency of this display object. 0 means fully transparent and 1 means fully opaque.
--
-- @function [parent=#DisplayObject] setAlpha
-- @param self
-- @param alpha (number) The new alpha transparency of the display object
function M:setAlpha(alpha)
	self.object:setAlpha(alpha)
	return self
end

---
-- Returns the alpha transparency of this display object.
--
-- @function [parent=#DisplayObject] getAlpha
-- @param self
-- @return The alpha of the display object
function M:getAlpha()
	return self.object:getAlpha()
end

---
-- Sets whether or not the display object is visible. Display objects that are not visible are also taken
-- into consideration while calculating bounds.
--
-- @function [parent=#DisplayObject] setVisible
-- @param self
-- @param visible (bool) whether or not the display object is visible
function M:setVisible(visible)
	self.object:setVisible(visible)
	return self
end

---
-- Returns whether or not the display object is visible.
--
-- @function [parent=#DisplayObject] getVisible
-- @param self
-- @return A value of 'true' if display object is visible; 'false' otherwise.
function M:getVisible()
	return self.object:getVisible()
end

---
-- Sets whether or not the display object is touchable.
--
-- @function [parent=#DisplayObject] setTouchable
-- @param self
-- @param touchable (bool) whether or not the display object is touchable
function M:setTouchable(touchable)
	self.object:setTouchable(touchable)
	return self
end

---
-- Returns whether or not the display object is touchable.
--
-- @function [parent=#DisplayObject] getTouchable
-- @param self
-- @return A value of 'true' if display object is touchable; 'false' otherwise.
function M:getTouchable()
	return self.object:getTouchable()
end

---
-- Sets the width and height of the display object in pixels. (Inner, No transform matrix)
--
-- @function [parent=#DisplayObject] setInnerSize
-- @param self
-- @param width (number) The width of the display object.
-- @param height (number) The height of the display object.
function M:setInnerSize(width, height)
	self.object:setInnerSize(width, height)
	return self
end

---
-- Returns the width and height of the display object in pixels. (Inner, No transform matrix)
--
-- @function [parent=#DisplayObject] getInnerSize
-- @param self
-- @return The width and height of the display object.
function M:getInnerSize()
	return self.object:getInnerSize()
end

---
-- Sets the size of the display object in pixels.
--
-- @function [parent=#DisplayObject] setContentSize
-- @param self
-- @param width (number) The new width of the display object.
-- @param height (number) The new height of the display object.
function M:setContentSize(width, height)
	self.object:setContentSize(width, height)
	return self
end

---
-- Returns the size of the display object in pixels. The size is calculated based on the
-- bounds of the content of the display object.
--
-- @function [parent=#DisplayObject] getContentSize
-- @param self
-- @return Size of the display object.
function M:getContentSize()
	return self.object:getContentSize()
end

---
-- Converts the x,y coordinates from the global to the display object's (local) coordinates.
--
-- @function [parent=#DisplayObject] globalToLocal
-- @param self
-- @param x (number) x coordinate of the global coordinate.
-- @param y (number) y coordinate of the global coordinate.
-- @param target (optional) The destination space of the transformation, nil for the screen space.
-- @return x coordinate relative to the display object.
-- @return y coordinate relative to the display object.
function M:globalToLocal(x, y, target)
	local m = self:getTransformMatrix(target)
	m:invert()
	return m:transformPoint(x, y)
end

---
-- Converts the x,y coordinates from the display object's (local) coordinates to the global coordinates.
--
-- @function [parent=#DisplayObject] localToGlobal
-- @param self
-- @param x (number) x coordinate of the local coordinate.
-- @param y (number) y coordinate of the local coordinate.
-- @param target (optional) The destination space of the transformation, nil for the screen space.
-- @return x coordinate relative to the display area.
-- @return y coordinate relative to the display area.
function M:localToGlobal(x, y, target)
	local m = self:getTransformMatrix(target)
	return m:transformPoint(x, y)
end

---
-- Return a matrix that represents the transformation from the local coordinate system to another.
--
-- @function [parent=#DisplayObject] getTransformMatrix
-- @param self
-- @param target (optional) The destination space of the transformation, nil for the screen space.
-- @return The transformation matrix of the display object to another
function M:getTransformMatrix(target)
	local matrix = Matrix.new()
	local o = self

	while(o and o ~= target) do
		matrix:multiply(matrix, o.object:getMatrix())
		o = o.parent
	end

	return matrix
end

---
-- Returns a rectangle (as x, y, w and h) that encloses the display object as
-- it appears in another display object’s coordinate system.
--
-- @function [parent=#DisplayObject] getBounds
-- @param self
-- @param target (DisplayObject) The display object that defines the other coordinate system to transform
-- @return rectangle has 4 values as x, y, w and h of bounds
function M:getBounds(target)
	local m = self:getTransformMatrix(target)
	return self.object:bounds(m)
end

---
-- Checks whether the given coordinates (in global coordinate system) is in bounds of the display object.
--
-- @function [parent=#DisplayObject] hitTestPoint
-- @param self
-- @param x (number)
-- @param y (number)
-- @param target (DisplayObject) The display object that defines the other coordinate system to transform
-- @return 'true' if the given global coordinates are in bounds of the display object, 'false' otherwise.
function M:hitTestPoint(x, y, target)
	if self:getVisible() and self:getTouchable() then
		local ox, oy = self:globalToLocal(x, y, target)
		local r = self:getBounds(self)
		return r:hitTestPoint(ox, oy)
	end
	return false
end

---
-- Perform a custom animation of a set of display object properties.
--
-- @function [parent=#DisplayObject] animate
-- @param self
-- @param properties (table) The properties values that the animation will move toward. ['x' 'y' 'rotation' 'scalex' 'scaley' 'alpha']
-- @param duration (number) Determining how long the animation will run in seconds.
-- @param easing (#Easing) The string indicating which easing function to use for transition.
-- The following easing functions can be used:
-- "linear"
-- "inSine"		"outSine"		"inOutSine"
-- "inQuad"		"outQuad"		"inOutQuad"
-- "inCubic"	"outCubic"		"inOutCubic"
-- "inQuart"	"outQuart"		"inOutQuart"
-- "inQuint"	"outQuint"		"inOutQuint"
-- "inExpo"		"outExpo"		"inOutExpo"
-- "inCirc"		"outCirc"		"inOutCirc"
-- "inBack"		"outBack"		"inOutBack"
-- "inElastic"	"outElastic"	"inOutElastic"
-- "inBounce"	"outBounce"		"inOutBounce"
function M:animate(properties, duration, easing)
	local function __animate_listener(d, e)
		if d.__animate ~= true then
			d:removeEventListener(Event.ENTER_FRAME, __animate_listener, d)
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
			elseif k == "alpha" then
				d:setAlpha(v:easing(elapsed))
			end
		end

		if elapsed >= d.__duration then
			d:removeEventListener(Event.ENTER_FRAME, __animate_listener, d)
			d.__duration = nil
			d.__tween = nil
			d.__watch = nil
			d.__animate = nil
		end
	end

	if self.__animate == true then
		self:removeEventListener(Event.ENTER_FRAME, __animate_listener, self)
		self.__duration = nil
		self.__tween = nil
		self.__watch = nil
		self.__animate = nil
	end

	if not properties or type(properties) ~= "table" or not next(properties) then
		return
	end

	if duration and duration <= 0 then
		return
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
		elseif k == "alpha" then
			b = self:getAlpha()
		end

		if b ~= nil then
			self.__tween[k] = Easing.new(b, v - b, self.__duration, easing)
		end
	end

	if not next(self.__tween) then
		self:removeEventListener(Event.ENTER_FRAME, __animate_listener, self)
		self.__duration = nil
		self.__tween = nil
		self.__watch = nil
		self.__animate = nil
	else
		self:addEventListener(Event.ENTER_FRAME, __animate_listener, self)
		self.__watch = Stopwatch.new()
		self.__animate = true
	end
end

---
-- Draw display object to the screen. This method must be subclassing.
--
-- @function [parent=#DisplayObject] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
end

---
-- Render display object and it's children to the screen.
--
-- @function [parent=#DisplayObject] render
-- @param self
-- @param display (Display) The context of the screen.
-- @param event (Event) The 'Event' object to be dispatched.
function M:render(display, event)
	self:dispatchEvent(event)

	if self:getVisible() then
		self:__draw(display)
	end

	for i, v in ipairs(self.children) do
		v:render(display, event)
	end
end

---
-- Dispatches an event to display object and it's children.
--
-- @function [parent=#DisplayObject] dispatch
-- @param self
-- @param event (Event) The 'Event' object to be dispatched.
function M:dispatch(event)
	local children = self.children

	for i = #children, 1, -1 do
		children[i]:dispatch(event)
	end

	self:dispatchEvent(event)
end

return M
