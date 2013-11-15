--------------------------------------------------------------------------------
--  Copyright (c) 2012 Sierra Wireless.
--  All rights reserved. This program and the accompanying materials
--  are made available under the terms of the Eclipse Public License v1.0
--  which accompanies this distribution, and is available at
--  http://www.eclipse.org/legal/epl-v10.html
--
--  Contributors:
--       Kevin KIN-FOO <kkinfoo@sierrawireless.com>
--           - initial API and implementation and initial documentation
--------------------------------------------------------------------------------
local M = {}

function M._internalcontent()
	return {
		content = nil,					-- block
		unknownglobalvars = {},			-- list of item
		tag = "MInternalContent"
	}
end

function M._block()
	return {
		content = {},					-- list of expr (identifier, index, call, invoke, block)
		localvars = {},					-- list of {var=item, scope ={min,max}}
		sourcerange = {min=0,max=0},
		tag = "MBlock"
	}
end

function M._identifier()
	return {
		definition = nil,				-- item
		sourcerange = {min=0,max=0},
		tag = "MIdentifier"
	}
end

function M._index(key, value)
	return {
		left= key,						-- expr (identifier, index, call, invoke, block)
		right= value,					-- string
		sourcerange = {min=0,max=0},
		tag = "MIndex"
	}
end

function M._call(funct)
	return {
		func = funct,					-- expr (identifier, index, call, invoke, block)
		sourcerange = {min=0,max=0},
		tag = "MCall"
	}
end

function M._invoke(name, expr)
	return {
		functionname = name,			-- string
		record = expr,					-- expr (identifier, index, call, invoke, block)
		sourcerange = {min=0,max=0},
		tag = "MInvoke"
	}
end

return M
