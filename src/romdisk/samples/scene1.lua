local M = class(display_object)

function M:init()
	display_object.init(self)

	self:add_child(display_image:new("/romdisk/samples/images/scene1.png"))
	
	for i=1,5 do
		local img = display_image:new("/romdisk/samples/images/" .. i ..".png", math.random(0, 800 - 100), math.random(0, 480 - 100))
		img.alpha = 1
	
		img:add_event_listener(event.MOUSE_DOWN, self.on_mouse_down, img)
		img:add_event_listener(event.MOUSE_MOVE, self.on_mouse_move, img)
		img:add_event_listener(event.MOUSE_UP, self.on_mouse_up, img)
	
		img:add_event_listener(event.TOUCHES_BEGIN, self.on_touches_begin, img)
		img:add_event_listener(event.TOUCHES_MOVE, self.on_touches_move, img)
		img:add_event_listener(event.TOUCHES_END, self.on_touches_end, img)
		img:add_event_listener(event.TOUCHES_CANCEL, self.on_touches_cancel, img)
	
		self:add_child(img)
	end
	
	self:add_event_listener("enterBegin", self.on_transition_in_begin, self)
	self:add_event_listener("enterEnd", self.on_transition_in_end, self)
	self:add_event_listener("exitBegin", self.on_transition_out_begin, self)
	self:add_event_listener("exitEnd", self.on_transition_out_end, self)
end

function M:on_mouse_down(e)
	if self:hit_test_point(e.info.x, e.info.y) then
		self.isfocus = true
		self:tofront()
		
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stop_propagation()
	end
end

function M:on_mouse_move(e)
	if self.isfocus then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		
		self:translate(dx, dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stop_propagation()
	end
end

function M:on_mouse_up(e)
	if self.isfocus then
		self.isfocus = false
		e:stop_propagation()
	end
end

function M:on_touches_begin(e)
	if self:hit_test_point(e.info.x, e.info.y) then
		self.isfocus = true
		self:tofront()
		
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stop_propagation()
	end
end

function M:on_touches_move(e)
	if self.isfocus then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		
		self:translate(dx, dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stop_propagation()
	end
end

function M:on_touches_end(e)
	if self.isfocus then
		self.isfocus = false
		e:stop_propagation()
	end
end

function M:on_touches_cancel(e)
	if self.isfocus then
		self.isfocus = false
		e:stop_propagation()
	end
end

function M:on_transition_in_begin()
	print("scene1 - on_transition_in_begin")
end

function M:on_transition_in_end()
	print("scene1 - transition_in_end")
end

function M:on_transition_out_begin()
	print("scene1 - transition_out_begin")
end

function M:on_transition_out_end()
	print("scene1 - transition_out_end")
end

return M
