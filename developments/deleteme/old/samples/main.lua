local button = require("button")
local scene1 = require("scene1")
local scene2 = require("scene2")
local scene_manager = require("scene_manager")
require("easing")

----------------------------------------------------------------------------------
local background = DisplayImage.new("/romdisk/samples/images/background.png")
stage:addChild(background)

local s1 = scene1.new()
local s2 = scene2.new()
s1:setanchor(800 / 2, 480 / 2)
s2:setanchor(800 / 2, 480 / 2)

local sm = scene_manager.new({
	["s1"] = s1,
	["s2"] = s2,
})
stage:addChild(sm)

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
local normal = DisplayImage.new("/romdisk/samples/images/button_normal.png")
local active = DisplayImage.new("/romdisk/samples/images/button_active.png")
local btn = button.new(normal, active)
btn:addEventListener("click", function(d, e)
	sm:changeScene(nextScene(), 1, scene_manager.moveFromLeft, easing.outBounce)
--	d:scale(1.1, 1.1)
--	d:setalpha(0.2)
end, btn)
btn:setPosition(40, 200)
btn:setanchor(btn.x + btn.width / 2, btn.y + btn.height / 2)
stage:addChild(btn)

local cursor = DisplayImage.new("/romdisk/samples/images/cursor.png", 0, 0)
cursor:addEventListener("mouse-down", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("mouse-move", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("mouse-up", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-begin", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-move", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
cursor:addEventListener("touch-end", function(d, e) d:setPosition(e.info.x, e.info.y) end, cursor)
stage:addChild(cursor)
