local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")

---
-- The 'display_object' class is the base class for all objects that can
-- be placed on the screen.
-- 
-- @module display_object
local M = class(event_dispatcher)

function M:init(dbg)
	event_dispatcher.init(self)

	self.parent = self
	self.children = {}

	self.x = 0
	self.y = 0
	self.w = 30
	self.h = 30
	self.visible = true

	-- Just for debugging information
	self.debug = dbg
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

	if child:get_parent() == self then
		return false
	end

	child:remove_from_parent()
	table.insert(self.children, child)
	child.parent = self

	return true
end

---
-- Adds a display object as a child to this display object. The child
-- is added at the index position specified. Indices start from 1.
-- 
-- Display object can have only one parent. Therefore if you add a child
-- object that already has a different display object as a parent, the
-- display object is removed from the child list of the other display 
-- object and then added to this display object.
-- 
-- @function [parent=#display_object] add_child_at
-- @param self
-- @param child (display_object) The child display object to add.
-- @param index (number) The index position to which the child is added.
-- @return A value of 'true' or 'false'.
function M:add_child_at(child, index)
	if child == nil or self == child then
		return false
	end

	if child:get_parent() == self then
		return false
	end

	child:remove_from_parent()
	table.insert(self.children, index, child)
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

	local children = self:get_children()
	local index = 0

	for i, v in ipairs(children) do
		if v == child then
			index = i
			break
		end
	end
	
	if index <= 0 then
		return false
	end

	table.remove(children, index)
	child.parent = child

	return true
end

---
-- Removes the child 'display_object' instance at the specifed index. 
-- Index of the first child is 1 and index of the last child can be
-- get from 'display_object:get_num_children' function.
-- 
-- @function [parent=#display_object] remove_child_at
-- @param self
-- @param index (number) The child index of the display object to remove.
-- @return A value of 'true' or 'false'.
function M:remove_child_at(index)
	local child = self:get_child_at(index)
	return self:remove_child(child)
end

---
-- If the display object has a parent, removes the display object from the
-- child list of its parent display object.
-- 
-- @function [parent=#display_object] remove_from_parent
-- @param self
-- @return A value of 'true' or 'false'.
function M:remove_from_parent()
	local parent = self:get_parent()

	if parent == nil or parent == self then
		return false
	end

	return parent:remove_child(self)
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
	local children = self:get_children()

	for i, v in ipairs(children) do
		if v == child then
			return true
--		elseif v:contains(child) then
--			return true
		end
	end

	return false
end

---
-- Returns the 'display_object' that contains this 'display_object'.
-- 
-- @function [parent=#display_object] get_parent
-- @param self
-- @return The parent display object.
function M:get_parent()
	return self.parent
end

---
-- Returns the children object of this display object.
-- 
-- @param self
-- @return The children of this display object.
----------------------------------------------------------------
function M:get_children()
	return self.children
end

---
-- Returns the number of children of this display object.
-- 
-- @function [parent=#display_object] get_num_children
-- @param self
-- @return The number of children of this display object.
function M:get_num_children()
	return #self.children
end

---
-- Returns the child 'display object' instance that exists at the
-- specified index. First child is at index 1.
-- 
-- @function [parent=#display_object] get_child_at
-- @param self
-- @param index (number) The index position of the child object.
-- @return The child display object at the specified index position.
function M:get_child_at(index)
	return self.children[index]
end

---
-- Returns the index of the specified child display object.
-- 
-- @function [parent=#display_object] get_child_index
-- @param self
-- @param child (display_object) The child display object to identify.
-- @return The index of the specified child display object.
function M:get_child_index(child)
	local children = self:get_children()
	local index = 0

	for i, v in ipairs(children) do
		if v == child then
			index = i
			break
		end
	end
	
	return index
end

--- 
-- Returns the x coordinate of the display object.
-- 
-- @function [parent=#display_object] getx
-- @param self
-- @return The x coordinate of the display object.
function M:getx()
	return self.x
end

--- 
-- Sets the x coordinate of the display object.
-- 
-- @function [parent=#display_object] setx
-- @param self
-- @param x (number) The new x coordinate of the display object.
function M:setx(x)
	self.x = x
end

--- 
-- Returns the y coordinate of the display object.
-- 
-- @function [parent=#display_object] gety
-- @param self
-- @return The y coordinate of the display object.
function M:gety()
	return self.y
end

--- 
-- Sets the y coordinate of the display object.
-- 
-- @function [parent=#display_object] sety
-- @param self
-- @param y (number) The new y coordinate of the display object.
function M:sety(y)
	self.y = y
end

--- 
-- Returns the x and y coordinates of the display object.
-- 
-- @function [parent=#display_object] getxy
-- @param self
-- @return The x and y coordinates of the display object.
function M:getxy()
	return self.x, self.y
end

---
-- Sets the x and y coordinates of the display object.
-- 
-- @function [parent=#display_object] setxy
-- @param self
-- @param x (number) The new x coordinate of the display object.
-- @param y (number) The new y coordinate of the display object.
function M:setxy(x, y)
	self.x = x
	self.y = y
end

---
-- Returns the width of the display object, in pixels. The width is calculated based on the
-- bounds of the content of the display object.
-- 
-- @function [parent=#display_object] getw
-- @param self
-- @return Width of the display object.
function M:getw()
    return self.w
end

--- 
-- Returns the height of the sprite, in pixels. The height is calculated based on the
-- bounds of the content of the display object.
-- 
-- @function [parent=#display_object] geth
-- @param self
-- @return Height of the display object.
function M:geth()
    return self.h
end

---
-- Returns the width and height of the display object, in pixels. The width and height is
-- calculated based on the bounds of the content of the display object.
-- 
-- @function [parent=#display_object] getwh
-- @param self
-- @return The width and height of the display object.
function M:getwh()
    return self.w, self.h
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
-- Returns whether or not the display object is visible. Child display objects that are not visible are also taken
-- into consideration while calculating bounds.
-- 
-- @function [parent=#display_object] get_visible
-- @param self
-- @return A value of 'true' if display object is visible; 'false' otherwise.
function M:get_visible()
    return self.visible
end

---
-- Sets whether or not the display object is visible. Display objects that are not visible are also taken
-- into consideration while calculating bounds.
-- 
-- @function [parent=#display_object] set_visible
-- @param self
-- @param visible (bool) whether or not the display object is visible
function M:set_visible(visible)
	self.visible = visible
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
		return self.x, self.y, self.w, self.h
	end
end

function M:render(cr)
	if not self.visible then return end

	local children = self:get_children()
	for i, v in ipairs(children) do v:render(cr) end
end

return M
