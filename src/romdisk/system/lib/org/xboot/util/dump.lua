local function dump(data, level, prefix)
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

return dump