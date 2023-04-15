local CAMERA = require("hardware.camera")

local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onMouseMove(self, e)
	if self.touchid == -1 then	
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onMouseUp(self, e)
	if self.touchid == -1 then
		self.touchid = nil
		e.stop = true
	end
end

local function onTouchBegin(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onTouchMove(self, e)
	if self.touchid == e.id then
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onTouchEnd(self, e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new("assets/images/bg.png"):extend(sw, sh, "repeat")))

local cam = CAMERA.new()
if cam then
	if cam:start(640, 480, "mjpg") then
		local cw, ch = cam:getSize()

		local cimg = Image.new(cw, ch)
			:shapeSave()
			:shapeSetLineWidth(1)
			:shapeSetSourceColor(Color.new("red"))
			:shapeRectangle(0, 0, cw, ch)
			:shapeStroke()
			:shapeRestore()
		
		local view = DisplayImage.new(cimg)
			:setPosition(math.random(0, sw - 64), math.random(0, sh - 64))
			:setAnchor(0.5, 0.5)
		
		view:addEventListener("mouse-down", onMouseDown)
		view:addEventListener("mouse-move", onMouseMove)
		view:addEventListener("mouse-up", onMouseUp)
		view:addEventListener("touch-begin", onTouchBegin)
		view:addEventListener("touch-move", onTouchMove)
		view:addEventListener("touch-end", onTouchEnd)
		
		stage:addTimer(Timer.new(1 / 30, 0, function(t)
			if cam:capture(cimg) then
				view:markDirty()
			end
		end))

		stage:addChild(view)
	end
end
