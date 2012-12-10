local M = {}

M.LEVEL_NONE = 0
M.LEVEL_INFO = 1
M.LEVEL_WARN = 2
M.LEVEL_ERROR = 3
M.LEVEL_DEBUG = 4

M.level = M.LEVEL_DEBUG

function M.info(...)
	if M.level >= M.LEVEL_INFO then
		print("[log info]", ...)
	end
end

function M.warn(...)
	if M.level >= M.LEVEL_WARN then
		print("[log warn]", ...)
	end
end

function M.error(...)
	if M.level >= M.LEVEL_ERROR then
		print("[log error]", ...)
	end
end

function M.debug(...)
	if M.level >= M.LEVEL_DEBUG then
		print("[log debug]", ...)
	end
end

return M
