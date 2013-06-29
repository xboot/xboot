local M = {}
setmetatable(M, {__index = table})

function M.copy(src, dst)
	dst = dst or {}
	for i, v in pairs(src) do
		dst[i] = v
	end
	return dst
end

function M.deepcopy(src, dst)
	dst = dst or {}
	for k, v in pairs(src) do
		if type(v) == "table" then
			dst[k] = M.deepcopy(v)
		else
			dst[k] = v
		end
	end
	return dst
end

function M.indexof(tab, val)
	for i, v in ipairs(tab) do
		if v == val then
			return i
		end
	end
	return 0
end

function M.keyof(tab, val)
	for k, v in pairs(tab) do
		if v == val then
			return k
		end
	end
	return nil
end

return M
