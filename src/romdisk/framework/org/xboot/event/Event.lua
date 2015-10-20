---
-- The objects of 'Event' class contains information about an event that has occurred. 'Event' objects
-- are passed to event listeners when an event occurs.
-- 
-- @module Event
local M = class()

---
-- @field [parent=#Event] KEY_DOWN KeyDown
M.KEY_DOWN = "KeyDown"

---
-- @field [parent=#Event] KEY_UP KeyUp
M.KEY_UP = "KeyUp"

---
-- @field [parent=#Event] MOUSE_DOWN MouseDown
M.MOUSE_DOWN = "MouseDown"

---
-- @field [parent=#Event] MOUSE_MOVE MouseMove
M.MOUSE_MOVE = "MouseMove"

---
-- @field [parent=#Event] MOUSE_UP MouseUp
M.MOUSE_UP = "MouseUp"

---
-- @field [parent=#Event] MOUSE_WHEEL MouseWheel
M.MOUSE_WHEEL = "MouseWheel"

---
-- @field [parent=#Event] TOUCH_BEGIN TouchBegin
M.TOUCH_BEGIN = "TouchBegin"

---
-- @field [parent=#Event] TOUCH_MOVE TouchMove
M.TOUCH_MOVE = "TouchMove"

---
-- @field [parent=#Event] TOUCH_END TouchEnd
M.TOUCH_END = "TouchEnd"

---
-- @field [parent=#Event] ENTER_FRAME EnterFrame
M.ENTER_FRAME = "EnterFrame"

---
-- @field [parent=#Event] ANIMATE_COMPLETE AnimateComplete
M.ANIMATE_COMPLETE = "AnimateComplete"

---
-- Creates a new 'Event' object to be dispatched from an 'EventDispatcher'.
-- 
-- @function [parent=#Event] new
-- @param type (string)
-- @param info (optional) An optional table of information attach to event object.
-- @return New 'Event' object.
function M:init(type, info)
	self.type = type
	self.info = info or {}
	self.stoped = false
end

---
-- Disables the propagation of the current event.
-- 
-- @function [parent=#Event] stopPropagation
-- @param self
function M:stopPropagation()
	self.stoped = true
end

return M
