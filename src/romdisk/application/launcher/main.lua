local DisplayPager = require "DisplayPager"
local AppItem = require "AppItem"
local AppPage = require "AppPage"

local sw, sh = stage:getSize()
local pager = DisplayPager.new(sw, sh, false)
local launcher = Application.new()

local pages = {}
local c = 0
for k, v in pairs(Application.list()) do
    if launcher:getPath() ~= k then
        local index = c // 8 + 1
        if not pages[index] then
            pages[index] = DisplayObject.new(sw, sh)
            pager:addPage(pages[index])
        end
        local page = AppItem.new(v)
            :addEventListener("click", function(d, e)
                d:execute()
                local path = d._app:getPath()
                for k, v in pairs(Window.list()) do
                    if k == path then
                        v:toFront()
                    end
                end
            end)

        local i = c % 8
        if i < 4 then
            page:setPosition(i * (128 + 25.6) + 25.6, 74.6)
        else
            i = i % 4
            page:setPosition(i * (128 + 25.6) + 25.6, 277)
        end
        pages[index]:addChild(page)
        c = c + 1
    end
end

stage:addChild(pager)
stage:showobj(false)
