local sw, sh = stage:getSize()

local bgimg = Image.new("assets/images/background.png")
stage:addChild(DisplayImage.new(bgimg))

local mimg = Image.new(sw, sh)
	:save()
	:setSource(Pattern.image(bgimg:clone():blur(120)))
	:paint()
	:setSource(Pattern.image(Image.new("assets/images/water.png")))
	:paint()
	:restore()
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
		mimg:save():setOperator("clear"):arc(e.x, e.y, 30, 0, 2 * math.pi):fill():restore()
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
		mimg:save():setOperator("clear"):arc(e.x, e.y, 30, 0, 2 * math.pi):fill():restore()
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
