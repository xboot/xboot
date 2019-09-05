local sw, sh = stage:getSize()

local bgimg = Image.new("assets/images/background.png")
stage:addChild(DisplayImage.new(bgimg))

local mimg = bgimg:extend(sw, sh, "repeat"):blur(120):blit(Matrix.new(), Image.new("assets/images/water.png"))
local mask = DisplayImage.new(mimg)
stage:addChild(mask)

local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		e.stop = true
	end
end

local function onMouseMove(self, e)
	if self.touchid == -1 then
		mimg:clear(Color.new({0, 0, 0, 0}), e.x - 20, e.y - 20, 40, 40)
		mask:markDirty()
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
		e.stop = true
	end
end

local function onTouchMove(self, e)
	if self.touchid == e.id then
		mimg:clear(Color.new({0, 0, 0, 0}), e.x - 20, e.y - 20, 40, 40)
		mask:markDirty()
		e.stop = true
	end
end

local function onTouchEnd(self, e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

mask:addEventListener("mouse-down", onMouseDown)
mask:addEventListener("mouse-move", onMouseMove)
mask:addEventListener("mouse-up", onMouseUp)
mask:addEventListener("touch-begin", onTouchBegin)
mask:addEventListener("touch-move", onTouchMove)
mask:addEventListener("touch-end", onTouchEnd)
