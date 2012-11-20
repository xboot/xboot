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

	local cwd = "/" --love.filesystem.getWorkingDirectory()
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

function dump(data, level, prefix)   
    if type(prefix) ~= "string" then   
        prefix = ""  
    end   
    if type(data) ~= "table" then   
        print(prefix .. tostring(data))   
    else  
        print(data)   
        if level ~= 0 then   
            local prefix_next = prefix .. "    "  
            print(prefix .. "{")   
            for k,v in pairs(data) do  
                io.stdout:write(prefix_next .. k .. " = ")
                if type(v) ~= "table" or (type(level) == "number" and level <= 1) then   
                    print(v)   
                else  
                    if level == nil then   
                        dump(v, nil, prefix_next)   
                    else  
                        dump(v, level - 1, prefix_next)   
                    end   
                end   
            end   
            print(prefix .. "}")   
        end   
    end   
end


print("==================================")
local seen={}
function globals_dump(t,i)
	seen[t]=true
	local s={}
	local n=0
	for k in pairs(t) do
		n=n+1 s[n]=k
	end
	table.sort(s)
	for k,v in ipairs(s) do
		print(i,v)
		v=t[v]
		if type(v)=="table" and not seen[v] then
			globals_dump(v,i.."\t")
		end
	end
end
--globals_dump(_G,"")
print("==================================")


local sample = require"xboot.sample"
dump(xboot)

return function()
	local ret = "110"
	print("--------")
	dump(xboot)
	print(sample.add(1.2, 3.1))
	print(sample.sub(1.2, 3.1))
	print("--------")
	return tonumber(ret) or 0
end


