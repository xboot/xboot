local Transition = {}

function Transition.interval(t, interval, default, transition)
	if t >= interval[1] and t<=interval[2] then
           transition((t - interval[1])/(interval[2]-interval[1])) -- linear mapping of interval to 0,1
        else
           transition(default)
	end
end

function Transition.outToRight(scene, t, width)
	scene:setX(   t  * width)
end

function Transition.inFromRight(scene, t, width)
	scene:setX((1-t) * width)
end

function Transition.outToLeft(scene, t, width)
	scene:setX(  -t  * width)
end

function Transition.inFromLeft(scene, t, width)
	scene:setX((t-1) * width)
end

function Transition.outToBottom(scene, t, height)
	scene:setY(   t  * height)
end

function Transition.inFromBottom(scene, t, height)
	scene:setY((1-t) * height)
end

function Transition.outToTop(scene, t, height)
	scene:setY(  -t  * height)
end

function Transition.inFromTop(scene, t, height)
	scene:setY((t-1) * height)
end

function Transition.fadeIn(scene, t)
	scene:setalpha(t)
end

function Transition.fadeOut(scene, t)
	scene:setalpha(1-t)
end

function Transition.shade(scene, t)
--	scene:setColorTransform(1-t, 1-t, 1-t)
end

function Transition.unshade(scene, t)
--	scene:setColorTransform(t, t, t)
end

function Transition.horizontalShrink(scene, t, width)
	scene:setscalex(1-t)
	scene:setX(t * width/2)
end

function Transition.horizontalExpand(scene, t, width)
	scene:setscalex(t)
	scene:setX((1-t) * width/2)
end

function Transition.verticalShrink(scene, t, height)
	scene:setscaley(1-t)
	scene:setY(t * height/2)
end

function Transition.verticalExpand(scene, t, height)
	scene:setscaley(t)
	scene:setY((1-t) * height/2)
end

function Transition.rotate(scene, t, startAngle, stopAngle)
	scene:setrotate((stopAngle - startAngle) * t + startAngle)
end

local M = Class(DisplayObject)
local application_width = 800
local application_height = 480

function M.moveFromRight(scene1, scene2, t)
	local width = application_width
	Transition.outToLeft(scene1, t, width)
	Transition.inFromRight(scene2, t, width)
end

function M.moveFromLeft(scene1, scene2, t)
	local width = application_width
	Transition.outToRight(scene1, t, width)
	Transition.inFromLeft(scene2, t, width)
end

function M.overFromRight(scene1, scene2, t)
	local width = application_width
	Transition.inFromRight(scene2, t, width)
end

function M.overFromLeft(scene1, scene2, t)
	local width = application_width
	Transition.inFromLeft(scene2, t, width)
end

function M.moveFromRightWithFade(scene1, scene2, t)
	local width = application_width
        M.moveFromRight(scene1, scene2, t)
        Transition.fadeOut(scene1, t)
end

function M.moveFromLeftWithFade(scene1, scene2, t)
	local width = application_width
        M.moveFromLeft(scene1, scene2, t)
        Transition.fadeOut(scene1, t)
end

function M.overFromRightWithFade(scene1, scene2, t)
	local width = application_width
        Transition.fadeOut(scene1, t)
	Transition.inFromRight(scene2, t, width)
end

function M.overFromLeftWithFade(scene1, scene2, t)
	local width = application_width
        Transition.fadeOut(scene1, t)
	Transition.inFromLeft(scene2, t, width)
end

function M.moveFromBottom(scene1, scene2, t)
	local height = application_height
	Transition.outToTop(scene1, t, height)
	Transition.inFromBottom(scene2, t, height)
end

function M.moveFromTop(scene1, scene2, t)
	local height = application_height
	Transition.outToBottom(scene1, t, height)
	Transition.inFromTop(scene2, t, height)
end

function M.overFromBottom(scene1, scene2, t)
	local height = application_height
	Transition.inFromBottom(scene2, t, height)
end

function M.overFromTop(scene1, scene2, t)
	local height = application_height
	Transition.inFromTop(scene2, t, height)
end

function M.moveFromBottomWithFade(scene1, scene2, t)
	local height = application_height
        M.moveFromBottom(scene1, scene2, t)
        Transition.fadeOut(scene1, t)
end

function M.moveFromTopWithFade(scene1, scene2, t)
	local height = application_height
        M.moveFromTop(scene1, scene2, t)
        Transition.fadeOut(scene1, t)
end

function M.overFromBottomWithFade(scene1, scene2, t)
	local height = application_height
	Transition.fadeOut(scene1, t)
	Transition.inFromBottom(scene2, t, height)
end

function M.overFromTopWithFade(scene1, scene2, t)
	local height = application_height
	Transition.fadeOut(scene1, t)
	Transition.inFromTop(scene2, t, height)
end

function M.fade(scene1, scene2, t)
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.fadeOut(scene1, t) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.fadeIn (scene2, t) end)
end

function M.crossfade(scene1, scene2, t)
	Transition.fadeOut(scene1, t)
	Transition.fadeIn(scene2, t)
end

function M.flip(scene1, scene2, t)
	local width = application_width
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.horizontalShrink(scene1, t, width) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.horizontalExpand(scene2, t, width) end)
end

function M.flipWithFade(scene1, scene2, t)
	local width = application_width
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.horizontalShrink(scene1, t, width) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.horizontalExpand(scene2, t, width) end)
        M.fade(scene1, scene2, t)
end

function M.flipWithShade(scene1, scene2, t)
	local width = application_width
        M.flip(scene1, scene2, t)
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.shade(scene1, t) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.unshade(scene2, t) end)
end

function M.zoomOutZoomIn(scene1, scene2, t)
	local width  = application_width
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.horizontalShrink(scene1, t, width) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.horizontalExpand(scene2, t, width) end)

	local height = application_height
	Transition.interval(t, { 0.0, 0.5 }, 1, function(t) Transition.verticalShrink(scene1, t, height) end)
	Transition.interval(t, { 0.5, 1.0 }, 0, function(t) Transition.verticalExpand(scene2, t, height) end)
end

function M.rotatingZoomOutZoomIn(scene1, scene2, t)
	M.zoomOutZoomIn(scene1, scene2, t)
	Transition.rotate(scene1, t, 0, 720)
	Transition.rotate(scene2, t, 0, 720)
end

local function dispatchEvent(dispatcher, name)
	if dispatcher:has_event_listener(name) then
		dispatcher:dispatchEvent(Event.new(name))
	end
end

local function defaultEase(ratio)
	return ratio
end

function M:init(scenes)
	DisplayObject.init(self)

	self.scenes = scenes
	self.tweening = false
--	self.transitionEventCatcher = Sprite.new()
	self:addEventListener("enter-frame", self.onEnterFrame, self)
end

local stopwatch = Stopwatch.new()

function M:changeScene(scene, duration, transition, ease, options)
	self.eventFilter = options and options.eventFilter

	if self.tweening then
		return
	end
	
	if self.scene1 == nil then
		self.scene1 = self.scenes[scene].new(options and options.userData)
		self:addChild(self.scene1)
		dispatchEvent(self, "transitionBegin")
		dispatchEvent(self.scene1, "enterBegin")
		dispatchEvent(self, "transitionEnd")
		dispatchEvent(self.scene1, "enterEnd")
		return
	end

	self.duration = duration
	self.transition = transition
	self.ease = ease or defaultEase

	self.scene2 = self.scenes[scene].new(options and options.userData)
	self.scene2:setVisible(false)
	self:addChild(self.scene2)
		
	self.time = 0
	self.currentTimer = stopwatch:elapsed()
	self.tweening = true
end

function M:filterTransitionEvents(event)
	event:stopPropagation()
end

function M:onTransitionBegin()
--[[
	if self.eventFilter then
		stage:addChild(self.transitionEventCatcher)
		for i,event in ipairs(self.eventFilter) do
			self.transitionEventCatcher:addEventListener(event, self.filterTransitionEvents, self)
		end
	end
]]
end

function M:onTransitionEnd()
--[[
	if self.eventFilter then
        	for i,event in ipairs(self.eventFilter) do
			self.transitionEventCatcher:removeEventListener(event, self.filterTransitionEvents, self)
		end
		self.transitionEventCatcher:removeFromParent()
	end
]]
end

function M:onEnterFrame(event)
	if not self.tweening then
		return
	end

	if self.time == 0 then
		self:onTransitionBegin()
		self.scene2:setVisible(true)
		dispatchEvent(self, "transitionBegin")
		dispatchEvent(self.scene1, "exitBegin")
		dispatchEvent(self.scene2, "enterBegin")
	end
		
	local timer = stopwatch:elapsed()
	local deltaTime = timer - self.currentTimer
	self.currentTimer = timer

	local t = (self.duration == 0) and 1 or (self.time / self.duration)

	self.transition(self.scene1, self.scene2, self.ease(t), t)

	if self.time == self.duration then
		dispatchEvent(self, "transitionEnd")
		dispatchEvent(self.scene1, "exitEnd")
		dispatchEvent(self.scene2, "enterEnd")
		self:onTransitionEnd()

		self:removeChild(self.scene1)
		self.scene1 = self.scene2
		self.scene2 = nil
		self.tweening = false

		collectgarbage()
	end

	self.time = self.time + deltaTime
	
	if self.time > self.duration then
		self.time = self.duration
	end

end

return M
