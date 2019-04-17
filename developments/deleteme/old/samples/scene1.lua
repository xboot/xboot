local M = Class(DisplayObject)

function M:init()
	DisplayObject.init(self)

	self:addChild(DisplayImage.new("/romdisk/samples/images/scene1.png"))
	
	for i=1,5 do
		local img = DisplayImage.new("/romdisk/samples/images/" .. i ..".png", math.random(0, 800 - 100), math.random(0, 480 - 100))
		img.alpha = 1
	
		img:addEventListener("mouse-down", self.onMouseDown, img)
		img:addEventListener("mouse-move", self.onMouseMove, img)
		img:addEventListener("mouse-up", self.onMouseUp, img)
	
		img:addEventListener("touch-begin", self.onTouchBegin, img)
		img:addEventListener("touch-move", self.onTouchMove, img)
		img:addEventListener("touch-end", self.onTouchEnd, img)
	
		self:addChild(img)
	end
	
	self:addEventListener("enterBegin", self.on_transition_in_begin, self)
	self:addEventListener("enterEnd", self.on_transition_in_end, self)
	self:addEventListener("exitBegin", self.on_transition_out_begin, self)
	self:addEventListener("exitEnd", self.on_transition_out_end, self)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.isfocus = true
		self:tofront()
		
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.isfocus then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		
		self:translate(dx, dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.isfocus then
		self.isfocus = false
		e:stopPropagation()
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.isfocus = true
		self:tofront()
		
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchMove(e)
	if self.isfocus then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		
		self:translate(dx, dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchEnd(e)
	if self.isfocus then
		self.isfocus = false
		e:stopPropagation()
	end
end

function M:on_transition_in_begin()
	print("scene1 - on_transition_in_begin")
end

function M:on_transition_in_end()
	print("scene1 - transition_in_end")
end

function M:on_transition_out_begin()
	print("scene1 - transition_out_begin")
end

function M:on_transition_out_end()
	print("scene1 - transition_out_end")
end

return M
