local M = {}
setmetatable(M, M)

local function table_copy(src, dest)
    dest = dest or {}
    for i, v in pairs(src) do
        dest[i] = v
    end
    return dest
end

function M:__call(...)
	local c = table_copy(self)
	local b = {...}
	for i = #b, 1, -1 do
		table_copy(b[i], c)
	end
	
	c.__call = function(self, ...)
		return self:new(...)
	end
	
	return setmetatable(c, c)
end

function M:new(...)
	local o = {__index = self}
	setmetatable(o, o)
	
	if o.init then
		o:init(...)
	end
	
	o.new = nil
	o.init = nil
	
	return o
end

return M
