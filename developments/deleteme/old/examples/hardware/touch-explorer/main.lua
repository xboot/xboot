local background = DisplayImage.new(Texture.new("background.png"))
stage:addChild(background)

local dots = {
	DisplayImage.new(Texture.new("1.png")),
	DisplayImage.new(Texture.new("2.png")),
	DisplayImage.new(Texture.new("3.png")),
	DisplayImage.new(Texture.new("4.png")),
	DisplayImage.new(Texture.new("5.png")),
}

local function onTouchBegin(d, e)
	local dot = dots[e.info.id]
	if dot then
		stage:addChild(dot)
		dot:setPosition(e.info.x - dot:getWidth() / 2 , e.info.y - dot:getHeight() / 2)
	end
end

local function onTouchMove(d, e)
	local dot = dots[e.info.id]
	if dot then
		dot:setPosition(e.info.x - dot:getWidth() / 2 , e.info.y - dot:getHeight() / 2)
	end
end

local function onTouchEnd(d, e)
	local dot = dots[e.info.id]
	if dot and stage:contains(dot) then
		stage:removeChild(dot)
	end
end

stage:addEventListener("touch-begin", onTouchBegin)
stage:addEventListener("touch-move", onTouchMove)
stage:addEventListener("touch-end", onTouchEnd)
