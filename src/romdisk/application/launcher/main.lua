local DisplayPager = require "DisplayPager"
local AppItem = require "AppItem"

local sw, sh = stage:getSize()

stage:addChild(DisplayShape.new(sw, sh)
    :setSource(Pattern.image(Image.new("assets/images/bg.png")):setExtend("repeat"))
    :paint())

local pw, ph = sw, sh
local launcher = Application.new()
local pager = DisplayPager.new(pw, ph, false)
local font = Font.new("assets/fonts/Roboto-Regular.ttf", 24)

local iw, ih = 128, 128
local col = (pw * 2 - iw) // (iw * 3)
local row = (ph * 2 - ih) // (ih * 3)
local num = col * row
local ox = (pw - col * iw) // (col + 1)
local oy = (ph - row * ih) // (row + 1)
local pages = {}
local c = 0
for k, v in pairs(Application.list()) do
    if launcher:getPath() ~= k then
        local index = c // num + 1
        if not pages[index] then
            pages[index] = DisplayObject.new(pw, ph)
            pager:addPage(pages[index])
        end

        local n = c % num
        local y = n // col
        local x = (n - y * col) % col
        local ix = (iw + ox) * x + ox
        local iy = (ih + oy) * y + oy

        local icon = AppItem.new(v)
            :setPosition(ix, iy)
            :addEventListener("click", function(d, e)
                d:execute()
                local path = d:getPath()
                for k, v in pairs(Window.list()) do
                    if k == path then
                        v:toFront()
                    end
                end
            end)
        pages[index]:addChild(icon)
     
        local label = DisplayText.new(font, Pattern.color(1, 1, 1), v:getName())
        local w, h = label:getSize()
        label:setPosition((iw - w) / 2 + ix, iy + ih + 4)
        pages[index]:addChild(label)

        c = c + 1
    end
end

stage:addChild(pager)
stage:showobj(false)