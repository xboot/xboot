-- Make sure xboot table exists
if not xboot then xboot = {} end

-- Setting package path
package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;" .. package.path
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;" .. package.path

-- Loader function
local function loader()
	require("main")
end

-- Message handler
local function handler(msg, layer)
	print((debug.traceback("ERROR: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local res, ret = xpcall(loader, handler)
	if not res then return -1 end
	return tonumber(ret) or -1
end

