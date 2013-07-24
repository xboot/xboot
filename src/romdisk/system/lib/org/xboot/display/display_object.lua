local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")

---
-- The 'display_object' class is the base class for all objects that can
-- be placed on the screen.
-- 
-- @module display_object
local M = class(event_dispatcher)

function M:init()
	event_dispatcher.init(self)

	self.parent = self
	self.children = {}
	self.isvisible = true

	self.x = 0
	self.y = 0
	self.width = 0
	self.height = 0
	self.xorigin = 0
	self.yorigin = 0
	self.xscale = 1
	self.yscale = 1
	self.rotation = 0
	self.alpha = 1
end

---
-- Adds a display object as a child to this display object. The child
-- is added as a last child of this 'display_object' instance.
-- 
-- Display object can have only one parent. Therefore if you add a child
-- object that already has a different display object as a parent, the
-- display object is removed from the child list of the other display 
-- object and then added to this display object.
-- 
-- @function [parent=#display_object] add_child
-- @param self
-- @param child (display_object) The child display object to add.
-- @return A value of 'true' or 'false'.
function M:add_child(child)
	if child == nil or self == child then
		return false
	end

	if child.parent == self then
		return false
	end

	child:remove_self()
	table.insert(self.children, child)
	child.parent = self

	return true
end

---
-- Removes the specified child 'display_object' instance from the child list
-- of this 'display_object' instance.
-- 
-- @function [parent=#display_object] remove_child
-- @param self
-- @param child (display_object) The child display object to remove.
-- @return A value of 'true' or 'false'.
function M:remove_child(child)
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
-- @function [parent=#display_object] remove_self
-- @param self
-- @return A value of 'true' or 'false'.
function M:remove_self()
	local parent = self.parent

	if parent == nil or parent == self then
		return false
	end

	return parent:remove_child(self)
end

---
-- Moves the display object to the visual front of its parent.
-- 
-- @function [parent=#display_object] tofront
-- @param self
-- @return A value of 'true' or 'false'.
function M:tofront()
	local parent = self.parent

	if parent == nil or parent == self then
		return false
	end

	if not parent:remove_child(self) then
		return false
	end

	table.insert(parent.children, self)
	self.parent = parent

	return true
end

---
-- Moves the display object to the visual back of its parent.
-- 
-- @function [parent=#display_object] toback
-- @param self
-- @return A value of 'true' or 'false'.
function M:toback()
	local parent = self.parent

	if parent == nil or parent == self then
		return false
	end

	if not parent:remove_child(self) then
		return false
	end

	table.insert(parent.children, 1, self)
	self.parent = parent

	return true
end

---
-- Determines whether the specified display object is contained in the subtree of
-- this 'display_object' instance.
-- 
-- @function [parent=#display_object] contains
-- @param self
-- @param child (display_object) The child object to test.
-- @return `true` if the child object is contained in the subtree of this 'display_object'
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
-- @function [parent=#display_object] visible
-- @param self
-- @param visible (bool) whether or not the display object is visible
function M:visible(visible)
	self.isvisible = visible
end

---
-- Effectively adds values to the x and y properties of an display object. (changing its on-screen position)
-- 
-- @function [parent=#display_object] translate
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
-- Effectively multiplies xscale and yscale properties by sx and sy respectively.
-- The scaling occurs around the object's reference point.
-- The default reference point for most display objects is center.
-- 
-- @function [parent=#display_object] scale
-- @param self
-- @param sx (number) Factors by which to change the scale in the x directions.
-- @param sy (number) Factors by which to change the scale in the y directions.
function M:scale(sx, sy)
	self.xscale = self.xscale * sx
	self.yscale = self.yscale * sy
	
	for i, v in ipairs(self.children) do
		v:scale(sx, sy)
	end
end

---
-- Retrieve or change the rotation of an object.
-- The rotation occurs around the object's reference point.
-- The default reference point for most display objects is the center.
-- 
-- @function [parent=#display_object] rotate
-- @param self
-- @param angle (number) The rotation angle in radian.
function M:rotate(angle)
	self.rotation = self.rotation + angle

	for i, v in ipairs(self.children) do
		v:rotation(angle)
	end
end

---
-- Sets the x and y coordinates of the display object.
-- 
-- @function [parent=#display_object] setxy
-- @param self
-- @param x (number) The new x coordinate of the display object.
-- @param y (number) The new y coordinate of the display object.
function M:setxy(x, y)
	local dx = x - self.x
	local dy = y - self.y

	self:translate(dx, dy)
end

function M:origin(dx, dy)
	self.xorigin = self.xorigin + dx
	self.yorigin = self.yorigin + dy

	for i, v in ipairs(self.children) do
		v:origin(dx, dy)
	end
end

function M:hit_test_point(x, y)
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
-- Converts the x,y coordinates from the global to the display object's (local) coordinates.
-- 
-- @function [parent=#display_object] global_to_local
-- @param self
-- @param x (number) x coordinate of the global coordinate.
-- @param y (number) y coordinate of the global coordinate.
-- @return x coordinate relative to the display object.
-- @return y coordinate relative to the display object.
function M:global_to_local(x, y)
	return x - self.x, y - self.y
end

--- 
-- Converts the x,y coordinates from the display object's (local) coordinates to the global coordinates.
-- 
-- @function [parent=#display_object] local_to_global
-- @param self
-- @param x (number) x coordinate of the local coordinate.
-- @param y (number) y coordinate of the local coordinate.
-- @return x coordinate relative to the display area.
-- @return y coordinate relative to the display area.
function M:local_to_global(x, y)
	return x + self.x, y + self.y
end

---
-- Returns a rectangle (as x, y, width and height) that encloses the display object as
-- it appears in another display object’s coordinate system.
-- 
-- @function [parent=#display_object] get_bounds
-- @param self
-- @param target (display_object) The display object that defines the other coordinate system to transform
-- @return 4 values as x, y, width and height of bounds
function M:get_bounds(target)
	if target ~= nil and target ~= self then

	else
		return self.x, self.y, self.width, self.height
	end
end

function M:update(cairo)
end

function M:render(cairo, event)
	self:dispatch_event(event)

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

	self:dispatch_event(event)
end

return M
