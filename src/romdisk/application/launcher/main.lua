local AppItem = require "AppItem"

local function spairs(t, order)
    local keys = {}
    for k in pairs(t) do keys[#keys + 1] = k end

    if order then
        table.sort(keys, function(a, b) return order(t, a, b) end)
    else
        table.sort(keys)
    end

    local i = 0
    return function()
        i = i + 1
        if keys[i] then
            return keys[i], t[keys[i]]
        end
    end
end

local sw, sh = stage:getSize()
local sv = DisplayScroll.new(sw, sh / 2, false, 0.92):setPosition(0, sh / 4)
local app = Application.new()

for k, v in spairs(Application.list()) do
    if app:getPath() ~= k then
        local item = AppItem.new(v)
            :setLayoutMargin(1, 0, 1, 0)
            :addEventListener("click", function(d, e) d:execute() end)
        sv:addItem(item)
    end
end
stage:addChild(sv)

