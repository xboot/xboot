local AppPager = require "AppPager"
local TaskScroll = require "TaskScroll"

local sw, sh = stage:getSize()
local bg = DisplayShape.new(sw, sh)
	:setSource(Pattern.image(Image.new("assets/images/bg.png")):setExtend("repeat"))
	:paint()
local pager = AppPager.new(sw, sh)
local bbg = nil
local scroll = nil
local showtask = false

local homekey = nil
stage:addEventListener("key-down", function(d, e)
	if e.key == 10 then
		if homekey then
			local delta = e.time - homekey
			if delta < 300000000 then
				homekey = nil
				if not showtask then
					bbg = DisplayImage.new(stage:snapshot():blur(32))
					scroll = TaskScroll.new(sw, sh / 2):setPosition(0, sh / 4)
					stage:removeChild(bg)
					stage:removeChild(pager)
					stage:addChild(bbg)
					stage:addChild(scroll)
					showtask = true
				end
				return
			end
		end
		homekey = e.time
		stage:addTimer(Timer.new(0.3, 1, function(t)
			if homekey then
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
		end))
	end
end)

stage:addChild(bg)
stage:addChild(pager)
stage:asHome(true)

