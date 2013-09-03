local M = class(display_object)

function M:init(on, off)
	display_object.init(self)

	self.on = on
	self.off = off
	self.ison = false
	self.focus = nil

	self:add_child(self.on)
	self:add_child(self.off)
	self:update_visual_state(self.ison)

	self:add_event_listener(event.MOUSE_DOWN, self.on_mouse_down, self)
	self:add_event_listener(event.MOUSE_MOVE, self.on_mouse_move, self)
	self:add_event_listener(event.MOUSE_UP, self.on_mouse_up, self)

	self:add_event_listener(event.TOUCHES_BEGIN, self.on_touches_begin, self)
	self:add_event_listener(event.TOUCHES_MOVE, self.on_touches_move, self)
	self:add_event_listener(event.TOUCHES_END, self.on_touches_end, self)
	self:add_event_listener(event.TOUCHES_CANCEL, self.on_touches_cancel, self)
end

function M:on_mouse_down(e)
	if self.focus == nil and self:hit_test_point(e.info.x, e.info.y) then
		self.focus = -1
		self.ison = not self.ison
		self:update_visual_state(self.ison)
		self:dispatch_event(event:new("toggled", {on = self.ison}))
		e:stop_propagation()
	end
end

function M:on_mouse_move(e)
	if self.focus == -1 then
		if not self:hit_test_point(e.info.x, e.info.y) then	
			self.focus = nil
		end
		e:stop_propagation()
	end
end

function M:on_mouse_up(e)
	if self.focus == -1 then
		self.focus = nil
		e:stop_propagation()
	end
end

function M:on_touches_begin(e)
	if self.focus == nil and self:hit_test_point(e.info.x, e.info.y) then
		self.focus = e.info.id
		self.ison = not self.ison
		self:update_visual_state(self.ison)
		self:dispatch_event(event:new("toggled", {on = self.ison}))
		e:stop_propagation()
	end
end

function M:on_touches_move(e)
	if self.focus == e.info.id then
		if not self:hit_test_point(e.info.x, e.info.y) then
			self.focus = nil
		end
		e:stop_propagation()
	end
end

function M:on_touches_end(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stop_propagation()
	end
end

function M:on_touches_cancel(e)
	if self.focus == e.info.id then
		self.focus = nil;
		e:stop_propagation()
	end
end

function M:update_visual_state(state)
	if state then
		self.on:visible(true)
		self.off:visible(false)
		self.width = self.off.width
		self.height = self.off.height
	else
		self.on:visible(false)
		self.off:visible(true)
		self.width = self.on.width
		self.height = self.on.height
	end
end

return M
