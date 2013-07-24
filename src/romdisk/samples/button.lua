local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local display_object = require("org.xboot.display.display_object")

local M = class(display_object)

function M:init(upstate, downstate)
	display_object.init(self)
	
	self.upstate = upstate
	self.downstate = downstate
	
	self.focus = false
	self:update_visual_state(false)

	self:add_child(self.upstate)
	self:add_child(self.downstate)

	self:add_event_listener(event.MOUSE_DOWN, self.on_mouse_down, self)
	self:add_event_listener(event.MOUSE_MOVE, self.on_mouse_move, self)
	self:add_event_listener(event.MOUSE_UP, self.on_mouse_up, self)

	self:add_event_listener(event.TOUCHES_BEGAN, self.on_touches_began, self)
	self:add_event_listener(event.TOUCHES_MOVE, self.on_touches_move, self)
	self:add_event_listener(event.TOUCHES_END, self.on_touches_end, self)
	self:add_event_listener(event.TOUCHES_CANCEL, self.on_touches_cancel, self)
end

function M:on_mouse_down(e, d)
	if self:hit_test_point(e.msg.x, e.msg.y) then
		self.focus = true
		self:update_visual_state(true)
		e:stop_propagation()
	end
end

function M:on_mouse_move(e, d)
	if self.focus then
		if not self:hit_test_point(e.msg.x, e.msg.y) then	
			self.focus = false
			self:update_visual_state(false)
		end
		e:stop_propagation()
	end
end

function M:on_mouse_up(e, d)
	if self.focus then
		self.focus = false
		self:update_visual_state(false)
		self:dispatch_event(event:new("click"))
		e:stop_propagation()
	end
end

function M:on_touches_began(d, e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_move(d, e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_end(d, e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_cancel(d, e)
	if self.focus then
		self.focus = false;
		self:update_visual_state(false)
		e:stop_propagation()
	end
end

function M:update_visual_state(state)
	if state then
		self.upstate:visible(false)
		self.downstate:visible(true)
		self.width = self.downstate.width
		self.height = self.downstate.height
	else
		self.upstate:visible(true)
		self.downstate:visible(false)
		self.width = self.upstate.width
		self.height = self.upstate.height
	end
end

return M
