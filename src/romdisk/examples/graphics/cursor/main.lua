local background = display_image:new("background.png")

local cursor = display_image:new("cursor.png", 0, 0)
cursor:add_event_listener(event.MOUSE_DOWN, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.MOUSE_MOVE, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.MOUSE_UP, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_BEGIN, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_MOVE, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_END, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_CANCEL, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)

runtime:add_child(background)
runtime:add_child(cursor)
