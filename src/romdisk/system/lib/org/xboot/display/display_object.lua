local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")

---
-- @module display_object
local M = class(event_dispatcher)

function M:init(name)
	event_dispatcher.init(self)

	self.__parent = self
	self.__children = {}
	
	self.__name = name
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

	child:remove_from_parent()
	table.insert(self.__children, child)
	child.__parent = self
	
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

	child:remove_from_parent()
	table.insert(self.__children, index, child)
	child.__parent = self

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

	local children = self.__children
	local index = 0

	for k, v in ipairs(children) do
		if v == child then
			index = k
			break
		end
	end
	
	if index <= 0 then
		return false
	end

	table.remove(children, index)
	child.__parent = child
	
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
	local child = self.__children[index]
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
--TODO
	return false
end

---
-- Returns the 'display_object' that contains this 'display_object'.
-- 
-- @function [parent=#display_object] get_parent
-- @param self
-- @return The parent display object.
function M:get_parent()
	return self.__parent
end

---
-- Returns the number of children of this display object.
-- 
-- @function [parent=#display_object] get_num_children
-- @param self
-- @return The number of children of this display object.
function M:get_num_children()
	return #self.__children
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
	return self.__children[index]
end

---
-- Returns the index of the specified child display object.
-- 
-- @function [parent=#display_object] get_child_index
-- @param self
-- @param child (display_object) The child display object to identify.
-- @return The index of the specified child display object.
function M:get_child_index(child)
	local children = self.__children
	local index = 0

	for k, v in ipairs(children) do
		if v == child then
			index = k
			break
		end
	end
	
	return index
end

return M
