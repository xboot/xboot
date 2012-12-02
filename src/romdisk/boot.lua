--[[
--]]

-- Make sure xboot table exists.
if not xboot then xboot = {} end

-- Used for setup:
xboot.path = {}
--xboot.arg = {}

-- Unparsed arguments:
--argv = {}

-- Replace any \ with /.
function xboot.path.normalslashes(p)
	return string.gsub(p, "\\", "/")
end

-- Makes sure there is a slash at the end
-- of a path.
function xboot.path.endslash(p)
	if string.sub(p, string.len(p)-1) ~= "/" then
		return p .. "/"
	else
		return p
	end
end

-- Checks whether a path is absolute or not.
function xboot.path.abs(p)

	local tmp = xboot.path.normalslashes(p)

	-- Path is absolute if it starts with a "/".
	if string.find(tmp, "/") == 1 then
		return true
	end

	-- Path is absolute if it starts with a
	-- letter followed by a colon.
	if string.find(tmp, "%a:") == 1 then
		return true
	end

	-- Relative.
	return false

end

-- Converts any path into a full path.
function xboot.path.getfull(p)

	if xboot.path.abs(p) then
		return xboot.path.normalslashes(p)
	end

	local cwd = "/" --xboot.filesystem.getWorkingDirectory()
	cwd = xboot.path.normalslashes(cwd)
	cwd = xboot.path.endslash(cwd)

	-- Construct a full path.
	local full = cwd .. xboot.path.normalslashes(p)

	-- Remove trailing /., if applicable
	return full:match("(.-)/%.$") or full
end

-- Returns the leaf of a full path.
function xboot.path.leaf(p)
	local a = 1
	local last = p

	while a do
		a = string.find(p, "/", a+1)

		if a then
			last = string.sub(p, a+1)
		end
	end

	return last
end

package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;" .. package.path
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;" .. package.path

local sample = require "xboot.sample"
local framerate = require "xboot.framerate"
local logger = require "logger"
local dump = require "dump"

function xboot.boot()
	dump(logger)
end

function xboot.init()
end

function xboot.run()
	if xboot.load then xboot.load(arg) end
	local fr = framerate.new()
	while true do
		fr:step()
		if xboot.update then xboot.update(fr:getdelta()) end
		if xboot.draw then xboot.draw() end
		fr:sleep(0.1)
		fr.sleep(fr, 0.1)
		--logger.error(fr:getfps())
		--print(fr:getfps())
	end
end

local debug = debug
local function error_printer(msg, layer)
	print((debug.traceback("Error: " .. tostring(msg), 1+(layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local result = xpcall(xboot.boot, error_printer)
	if not result then return 1 end
	local result = xpcall(xboot.init, error_printer)
	if not result then return 1 end
	local result, retval = xpcall(xboot.run, error_printer)
	if not result then return 1 end
	return tonumber(retval) or 0
end
