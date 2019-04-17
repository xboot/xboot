local background = DisplayImage.new(Texture.new("background.png"))

local cursor = DisplayImage.new(Texture.new("cursor.png", 0, 0))
cursor:addEventListener("mouse-down", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("mouse-move", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("mouse-up", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-begin", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-move", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-end", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)

stage:addChild(background)
stage:addChild(cursor)
