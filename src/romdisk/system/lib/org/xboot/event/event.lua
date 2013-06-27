local class = require("org.xboot.lang.class")

---
-- The objects of 'event' class contains information about an event that has occurred. 'event' objects
-- are passed to event listeners when an event occurs.
-- 
-- @module event
local M = class()

---
-- @field [parent=#event] KEY_DOWN key_down
M.KEY_DOWN = "key_down"

---
-- @field [parent=#event] KEY_UP key_up
M.KEY_UP = "key_up"

---
-- @field [parent=#event] MOUSE_DOWN mouse_down
M.MOUSE_DOWN = "mouse_down"

---
-- @field [parent=#event] MOUSE_MOVE mouse_move
M.MOUSE_MOVE = "mouse_move"

---
-- @field [parent=#event] MOUSE_UP mouse_up
M.MOUSE_UP = "mouse_up"

---
-- @field [parent=#event] TOUCHES_BEGAN touches_began
M.TOUCHES_BEGAN = "touches_began"

---
-- @field [parent=#event] TOUCHES_MOVE touches_move
M.TOUCHES_MOVE = "touches_move"

---
-- @field [parent=#event] TOUCHES_END touches_end
M.TOUCHES_END = "touches_end"

---
-- @field [parent=#event] TOUCHES_CANCEL touches_cancel
M.TOUCHES_CANCEL = "touches_cancel"

---
-- Creates a new 'event' object to be dispatched from an 'event_dispatcher'.
-- 
-- @function [parent=#event] new
-- @param type (string)
-- @return New 'event' object.
function M:init(type)
    self.type = type
    self.stoped = false
end

---
-- Returns a string that represents the type of the event.
-- 
-- @function [parent=#event] get_type
-- @param self
-- @return Type of event.
function M:get_type()
  return self.type
end

---
-- Returns the element on which the event listener was registered.
-- 
-- @function [parent=#event] get_target
-- @param self
-- @return Target of event.
function M:get_target()
  return nil
end

---
-- Disables the propagation of the current event.
-- 
-- @function [parent=#event] stop_propagation
-- @param self
function M:stop_propagation()
	self.stoped = true
end

return M
