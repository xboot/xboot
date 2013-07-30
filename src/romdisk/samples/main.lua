----[[
local buildin_logger = require("org.xboot.buildin.logger")
local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")
local buildin_cairo = require("org.xboot.buildin.cairo")

local class = require("org.xboot.lang.class")
local timer = require("org.xboot.timer.timer")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local display_image = require("org.xboot.display.display_image")
--]]
----------------------------------------------------------------------------------
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
cursor:add_event_listener(event.MOUSE_DOWN, function(d, e) d:setxy(e.msg.x, e.msg.y) end, cursor)
cursor:add_event_listener(event.MOUSE_MOVE, function(d, e) d:setxy(e.msg.x, e.msg.y) end, cursor)
cursor:add_event_listener(event.MOUSE_UP, function(d, e) d:setxy(e.msg.x, e.msg.y) end, cursor)
runtime:add_child(cursor)

------------------- main --------------------------------
local tc = buildin_timecounter.new()
local cs1 = buildin_cairo.xboot_surface_create()
local cs2 = buildin_cairo.xboot_surface_create()
local cr1 = buildin_cairo.create(cs1)
local cr2 = buildin_cairo.create(cs2)
local index = false;

timer:new(1 / 30, 0, function(t, e)
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
	local msg = buildin_event.pump()	
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		runtime:dispatch(e)
	end

	timer:schedule(tc:delta())
end
