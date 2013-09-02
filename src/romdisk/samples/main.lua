local button = require("button")
local scene1 = require("scene1")
local scene2 = require("scene2")
local scene_manager = require("scene_manager")
require("easing")

----------------------------------------------------------------------------------
local background = display_image:new("/romdisk/samples/images/background.png")
runtime:add_child(background)

local s1 = scene1:new()
local s2 = scene2:new()
s1:setanchor(800 / 2, 480 / 2)
s2:setanchor(800 / 2, 480 / 2)

local sm = scene_manager:new({
	["s1"] = s1,
	["s2"] = s2,
})
runtime:add_child(sm)

local scenes = {"s1", "s2"}
local currentScene = 1
local function nextScene()
	local next = scenes[currentScene]

	currentScene = currentScene + 1
	if currentScene > #scenes then
		currentScene = 1
	end
	
	return next
end

-- create the up and down sprites for the button
local normal = display_image:new("/romdisk/samples/images/button_normal.png")
local active = display_image:new("/romdisk/samples/images/button_active.png")
local btn = button:new(normal, active)
btn:add_event_listener("click", function(d, e)
	sm:changeScene(nextScene(), 1, scene_manager.moveFromLeft, easing.outBounce)
--	d:scale(1.1, 1.1)
--	d:setalpha(0.2)
end, btn)
btn:setxy(40, 200)
btn:setanchor(btn.x + btn.width / 2, btn.y + btn.height / 2)
runtime:add_child(btn)

local cursor = display_image:new("/romdisk/samples/images/cursor.png", 0, 0)
cursor:add_event_listener(event.MOUSE_DOWN, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.MOUSE_MOVE, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.MOUSE_UP, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_BEGIN, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_MOVE, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_END, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
cursor:add_event_listener(event.TOUCHES_CANCEL, function(d, e) d:setxy(e.info.x, e.info.y) end, cursor)
runtime:add_child(cursor)
