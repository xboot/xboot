local button = require("button")
local scene1 = require("scene1")
local scene2 = require("scene2")
local scene_manager = require("scene_manager")
require("easing")

----------------------------------------------------------------------------------
local runtime = display_object:new("runtime")

local background = display_image:new("/romdisk/samples/images/background.png")
runtime:add_child(background)

local s1 = scene1:new()
local s2 = scene2:new()
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
end, btn)
btn:setxy(40, 200)
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

------------------- main --------------------------------
local tc = buildin_timecounter.new()
local cs1 = buildin_cairo.xboot_surface_create()
local cs2 = buildin_cairo.xboot_surface_create()
local cr1 = buildin_cairo.create(cs1)
local cr2 = buildin_cairo.create(cs2)
local index = false;

timer:new(1 / 60, 0, function(t, e)
	local cr
	index = not index
	if index then
		cr = cr2
	else
		cr = cr1
	end
	
	runtime:render(cr, event:new(event.ENTER_FRAME))
	
	if index then
		cs2:present()
	else
		cs1:present()
	end
end)

while true do
	local info = buildin_event.pump()	
	if info ~= nil then
		local e = event:new(info.type, info)
		runtime:dispatch(e)
	end

	timer:schedule(tc:delta())
end
