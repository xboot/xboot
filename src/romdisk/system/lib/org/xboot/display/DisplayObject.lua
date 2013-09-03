local M_PI2 = math.pi * 2

---
-- The 'DisplayObject' class is the base class for all objects that can
-- be placed on the screen.
-- 
-- @module DisplayObject
local M = Class(EventDispatcher)

function M:init()
	EventDispatcher.init(self)

	self.parent = self
	self.children = {}
	self.isvisible = true

	self.x = 0
	self.y = 0
	self.width = 0
	self.height = 0
	self.rotation = 0
	self.scalex = 1
	self.scaley = 1
	self.anchorx = 0
	self.anchory = 0
	self.alpha = 1

	self.__rotate = false;
	self.__scale = false;
	self.__alpha = false;
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
	child.parent = child

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

	if parent == nil or parent == self then
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

	if parent == nil or parent == self then
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

	if parent == nil or parent == self then
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
-- Determines whether the specified display object is contained in the subtree of
-- this 'DisplayObject' instance.
-- 
-- @function [parent=#DisplayObject] contains
-- @param self
-- @param child (DisplayObject) The child object to test.
-- @return `true` if the child object is contained in the subtree of this 'DisplayObject'
-- instance, otherwise `false`.
function M:contains(child)
	for i, v in ipairs(self.children) do
		if v == child then
			return true
		elseif v:contains(child) then
			return true
		end
	end

	return false
end

---
-- Sets whether or not the display object is visible. Display objects that are not visible are also taken
-- into consideration while calculating bounds.
-- 
-- @function [parent=#DisplayObject] visible
-- @param self
-- @param visible (bool) whether or not the display object is visible
function M:visible(visible)
	self.isvisible = visible
end

---
-- Effectively adds values to the x and y properties of an display object. (changing its on-screen position)
-- 
-- @function [parent=#DisplayObject] translate
-- @param self
-- @param dx (number) Amount to add to the display object's x properties.
-- @param dy (number) Amount to add to the display object's y properties.
function M:translate(dx, dy)
	self.x = self.x + dx
	self.y = self.y + dy

	for i, v in ipairs(self.children) do
		v:translate(dx, dy)
	end
end

---
-- Retrieve or change the rotation of an object.
-- The rotation occurs around the object's reference point.
-- The default reference point for most display objects is the center.
-- 
-- @function [parent=#DisplayObject] rotate
-- @param self
-- @param angle (number) The rotation angle in radian.
function M:rotate(angle)
	self.rotation = self.rotation + angle

	while(self.rotation < 0) do
		self.rotation = self.rotation + M_PI2
	end

	while(self.rotation > M_PI2) do
		self.rotation = self.rotation - M_PI2
	end

	self.__rotate = self.rotation ~= 0

	for i, v in ipairs(self.children) do
		v:rotate(angle)
	end
end

---
-- Effectively multiplies scalex and scaley properties by sx and sy respectively.
-- The scaling occurs around the object's reference point.
-- The default reference point for most display objects is center.
-- 
-- @function [parent=#DisplayObject] scale
-- @param self
-- @param sx (number) Factors by which to change the scale in the x directions.
-- @param sy (number) Factors by which to change the scale in the y directions.
function M:scale(sx, sy)
	self.scalex = self.scalex * sx
	self.scaley = self.scaley * sy

	self.__scale = self.scalex ~= 1 or self.scaley ~= 1

	for i, v in ipairs(self.children) do
		v:scale(sx, sy)
	end
end

---
-- Sets the x coordinates of the display object.
-- 
-- @function [parent=#DisplayObject] setX
-- @param self
-- @param x (number) The new x coordinate of the display object.
function M:setX(x)
	self:translate(x - self.x, 0)
end

---
-- Sets the y coordinates of the display object.
-- 
-- @function [parent=#DisplayObject] setY
-- @param self
-- @param y (number) The new y coordinate of the display object.
function M:setY(y)
	self:translate(0, y - self.y)
end

---
-- Sets the x and y coordinates of the display object.
-- 
-- @function [parent=#DisplayObject] setXY
-- @param self
-- @param x (number) The new x coordinate of the display object.
-- @param y (number) The new y coordinate of the display object.
function M:setXY(x, y)
	self:translate(x - self.x, y - self.y)
end

function M:setRotate(angle)
	self:rotate(angle - self.rotation)
end

function M:setScalex(x)
	self:scale(x / self.scalex, 1)
end

function M:setScaley(y)
	self:scale(1, y / self.scaley)
end

function M:setScale(x, y)
	self:scale(x / self.scalex, y / self.scaley)
end

function M:setAnchor(x, y)
	self.anchorx = x - self.x
	self.anchory = y - self.y
	
	for i, v in ipairs(self.children) do
		v:setAnchor(x, y)
	end
end

function M:setAlpha(alpha)
	self.alpha = alpha

	self.__alpha = self.alpha ~= 1

	for i, v in ipairs(self.children) do
		v:setAlpha(alpha)
	end
end

--- 
-- Converts the x,y coordinates from the global to the display object's (local) coordinates.
-- 
-- @function [parent=#DisplayObject] globalToLocal
-- @param self
-- @param x (number) x coordinate of the global coordinate.
-- @param y (number) y coordinate of the global coordinate.
-- @return x coordinate relative to the display object.
-- @return y coordinate relative to the display object.
function M:globalToLocal(x, y)
	return x - self.x, y - self.y
end

--- 
-- Converts the x,y coordinates from the display object's (local) coordinates to the global coordinates.
-- 
-- @function [parent=#DisplayObject] localToGlobal
-- @param self
-- @param x (number) x coordinate of the local coordinate.
-- @param y (number) y coordinate of the local coordinate.
-- @return x coordinate relative to the display area.
-- @return y coordinate relative to the display area.
function M:localToGlobal(x, y)
	return x + self.x, y + self.y
end

function M:hitTest(x, y)
	local left = self.x
	local top = self.y
	local right = left + self.width
	local bottom = top + self.height
	
	if x < left then return false end
	if x > right then return false end
	if y < top then return false end
	if y > bottom then return false end
	
	return true
end

---
-- Returns a rectangle (as x, y, width and height) that encloses the display object as
-- it appears in another display object’s coordinate system.
-- 
-- @function [parent=#DisplayObject] getBounds
-- @param self
-- @param target (DisplayObject) The display object that defines the other coordinate system to transform
-- @return 4 values as x, y, width and height of bounds
function M:getBounds(target)
	if target ~= nil and target ~= self then

	else
		return self.x, self.y, self.width, self.height
	end
end

function M:update(cairo)
end

function M:render(cairo, event)
	self:dispatchEvent(event)

	if self.isvisible then
		self:update(cairo)
	end

	for i, v in ipairs(self.children) do
		v:render(cairo, event)
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
