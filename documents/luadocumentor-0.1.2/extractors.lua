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
require 'metalua.package'
local compiler = require 'metalua.compiler'
local mlc = compiler.new()
local Q = require 'metalua.treequery'

-- Enable to retrieve all Javadoc-like comments from C code
function M.c(code)
	if not code then return nil, 'No code provided' end
	local comments = {}
	-- Loop over comments stripping cosmetic '*'
	for comment in code:gmatch('%s*/%*%*+(.-)%*+/') do
		-- All Lua special comment are prefixed with an '-',
		-- so we also comment C comment to make them compliant
		table.insert(comments, '-'..comment)
	end
	return comments
end

-- Enable to retrieve "---" comments from Lua code
function M.lua( code )
	if not code then return nil, 'No code provided' end
	-- Get ast from file
	local status, ast = pcall(mlc.src_to_ast, mlc, code)
	--
	-- Detect parsing errors
	--
	if not status then
		return nil, 'There might be a syntax error.\n' .. ast
	end
	local status, error = pcall(compiler.check_ast, ast)
	if not status then
		return nil, 'An error occurred while parsing.\n'..error
	end

	--
	-- Extract commented nodes from AST
	--

	-- Function enabling commented node selection
	local function acceptcommentednode(node)
		return node.lineinfo and ( node.lineinfo.last.comments or node.lineinfo.first.comments )
	end

	-- Fetch commented node from AST
	local commentednodes = Q(ast):filter( acceptcommentednode ):list()

	-- Comment cache to avoid selecting same comment twice
	local commentcache = {}
	-- Will contain selected comments
	local comments = {}

	-- Loop over commented nodes
	for _, node in ipairs( commentednodes ) do

		-- A node can is relateds to comment before and after itself,
		-- the following gathers them.
		local commentlists = {}
		if node.lineinfo and node.lineinfo.first.comments then
			table.insert(commentlists,  node.lineinfo.first.comments)
		end
		if node.lineinfo and node.lineinfo.last.comments then
			table.insert(commentlists,  node.lineinfo.last.comments)
		end
		-- Now that we have comments before and fater the node,
		-- collect them in a single table
		for _, list in ipairs( commentlists ) do
			for _, commenttable in ipairs(list) do
				-- Only select special comments
				local firstcomment = #commenttable > 0 and #commenttable[1] > 0 and commenttable[1]
				if firstcomment:sub(1, 1) == '-' then
					for _, comment in ipairs( commenttable ) do
						-- Only comments which were not already collected
						if not commentcache[comment] then
							commentcache[comment] = true
							table.insert(comments, comment)
						end
					end
				end
			end
		end
	end
	return comments
end
return M
