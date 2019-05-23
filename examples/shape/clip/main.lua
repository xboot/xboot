local M_PI = math.pi

local s = DisplayShape.new(stage:getSize())
    :arc(128.0, 128.0, 76.8, 0, 2 * M_PI)
    :clip()
    :newPath()
    :rectangle(0, 0, 256, 256)
    :fill()
    :setSourceColor(0, 1, 0)
    :moveTo(0, 0)
    :lineTo(256, 256)
    :moveTo(256, 0)
    :lineTo(0, 256)
    :setLineWidth(10.0)
    :stroke()

stage:addChild(s)
