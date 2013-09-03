local background = display_image:new("background.png")
runtime:add_child(background)

local dots = {
	display_image:new("1.png"),
	display_image:new("2.png"),
	display_image:new("3.png"),
	display_image:new("4.png"),
	display_image:new("5.png"),
}

local function on_touches_begin(d, e)
	local dot = dots[e.info.id]
	if dot then
		runtime:add_child(dot)
		dot:setxy(e.info.x - dot.width / 2 , e.info.y - dot.height / 2)
	end
end

local function on_touches_move(d, e)
	local dot = dots[e.info.id]
	if dot then
		dot:setxy(e.info.x - dot.width / 2 , e.info.y - dot.height / 2)
	end
end

local function on_touches_end(d, e)
	local dot = dots[e.info.id]
	if dot and runtime:contains(dot) then
		runtime:remove_child(dot)
	end
end

local function on_touches_cancel(d, e)
	local dot = dots[e.info.id]
	if dot and runtime:contains(dot) then
		runtime:removeChild(dot)
	end
end

runtime:add_event_listener(event.TOUCHES_BEGIN, on_touches_begin)
runtime:add_event_listener(event.TOUCHES_MOVE, on_touches_move)
runtime:add_event_listener(event.TOUCHES_END, on_touches_end)
runtime:add_event_listener(event.TOUCHES_CANCEL, on_touches_cancel)
