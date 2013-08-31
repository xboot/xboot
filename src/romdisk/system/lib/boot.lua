package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;" .. package.path
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;" .. package.cpath

print = require("org.xboot.buildin.logger").print

local function loader()
	require("main")
end

local function handler(msg, layer)
	print((debug.traceback("ERROR: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local res, ret = xpcall(loader, handler)
	if not res then return -1 end
	return tonumber(ret) or -1
end
