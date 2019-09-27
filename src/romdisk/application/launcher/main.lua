local AppPager = require "AppPager"
local TaskScroll = require "TaskScroll"

local sw, sh = stage:getSize()

local bg = DisplayImage.new(Image.new("assets/images/bg.png"):extend(sw, sh, "repeat"))
local pager = AppPager.new(sw, sh)
local bbg = nil
local scroll = nil
local showtask = false

stage:addEventListener("key-down", function(d, e)
	if e.key == 10 then
		if not showtask then
			bbg = DisplayImage.new(stage:snapshot():blur(32))
			scroll = TaskScroll.new(sw, sh / 2):setPosition(0, sh / 4)
			stage:removeChild(bg)
			stage:removeChild(pager)
			stage:addChild(bbg)
			stage:addChild(scroll)
			showtask = true
			scroll:addEventListener("execute", function(d, e)
				if showtask then
					stage:removeChild(bbg)
					stage:removeChild(scroll)
					stage:addChild(bg)
					stage:addChild(pager)
					bbg = nil
					scroll = nil
					showtask = false
				end
			end)
		end
	elseif e.key == 11 then
		if showtask then
			stage:removeChild(bbg)
			stage:removeChild(scroll)
			stage:addChild(bg)
			stage:addChild(pager)
			bbg = nil
			scroll = nil
			showtask = false
		end
	end
end)

stage:addChild(bg)
stage:addChild(pager)
stage:setLauncher(true)
