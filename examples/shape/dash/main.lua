local dashes = {
	50.0,  --/* ink */
	10.0,  --/* skip */
	10.0,  --/* ink */
	10.0,  --/* skip*/
};
local offset = -50.0;

local s = DisplayShape.new(stage:getSize())
	:setDash(dashes, offset)
	:setLineWidth(10)
	:moveTo(128.0, 25.6)
	:lineTo(230.4, 230.4)
	:relLineTo(-102.4, 0.0)
	:curveTo(51.2, 230.4, 51.2, 128.0, 128.0, 128.0)
	:stroke()

stage:addChild(s)
