local Leditem = require("Leditem")
local Led = require("Led")

-- create background
local background = DisplayImage:new(Texture:new("background.png"))
runtime:addChild(background)

-- create bgled
local lon = DisplayImage:new(Texture:new("bgled1.png"))
local loff = DisplayImage:new(Texture:new("bgled2.png"))
local bgled = Led:new(lon, loff)
bgled:setXY(500, 0)
runtime:addChild(bgled)

local bgledstate = false
local looptimer = Timer:new(0.4, 0, function(t, e)
	bgledstate = not bgledstate
	bgled:setState(bgledstate)
end)

function spairs(t, order)
    -- collect the keys
    local keys = {}
    for k in pairs(t) do keys[#keys+1] = k end

    -- if order function given, sort by it by passing the table and keys a, b,
    -- otherwise just sort the keys 
    if order then
        table.sort(keys, function(a,b) return order(t, a, b) end)
    else
        table.sort(keys)
    end

    -- return the iterator function
    local i = 0
    return function()
        i = i + 1
        if keys[i] then
            return keys[i], t[keys[i]]
        end
    end
end

local ledlist = buildin_hardware.led.list()
local x = 0
local y = 0
for k, v in spairs(ledlist) do
	x = x + 0
	y = y + 65
	local item = Leditem:new(v)
	item:setXY(x, y)
	runtime:addChild(item)
end
