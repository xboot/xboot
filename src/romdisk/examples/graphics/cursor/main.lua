local background = DisplayImage.new(Texture.new("background.png"))

local cursor = DisplayImage.new(Texture.new("cursor.png", 0, 0))
cursor:addEventListener(Event.MOUSE_DOWN, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.MOUSE_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.MOUSE_UP, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.TOUCHES_BEGIN, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.TOUCHES_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.TOUCHES_END, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener(Event.TOUCHES_CANCEL, function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)

runtime:addChild(background)
runtime:addChild(cursor)
