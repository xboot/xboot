local M = Class(DisplayObject)

function M:init()
	DisplayObject.init(self)

	self:addChild(DisplayImage.new("/romdisk/samples/images/scene1.png"))
	
	for i=1,5 do
		local img = DisplayImage.new("/romdisk/samples/images/" .. i ..".png", math.random(0, 800 - 100), math.random(0, 480 - 100))
		img.alpha = 1
	
		img:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, img)
		img:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, img)
		img:addEventListener(Event.MOUSE_UP, self.onMouseUp, img)
	
		img:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin, img)
		img:addEventListener(Event.TOUCH_MOVE, self.onTouchMove, img)
		img:addEventListener(Event.TOUCH_END, self.onTouchEnd, img)
		img:addEventListener(Event.TOUCH_CANCEL, self.onTouchCancel, img)
	
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

function M:onTouchCancel(e)
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
