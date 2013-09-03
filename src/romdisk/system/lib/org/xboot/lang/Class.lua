local M = {}
setmetatable(M, M)

local function tableCopy(src, dst)
    dst = dst or {}
    for i, v in pairs(src) do
        dst[i] = v
    end
    return dst
end

function M:__call(...)
	local c = tableCopy(self)
	local b = {...}
	for i = #b, 1, -1 do
		tableCopy(b[i], c)
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
