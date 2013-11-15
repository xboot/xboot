--------------------------------------------------------------------------------
-- Copyright (c) 2006-2013 Fabien Fleutot and others.
--
-- All rights reserved.
--
-- This program and the accompanying materials are made available
-- under the terms of the Eclipse Public License v1.0 which
-- accompanies this distribution, and is available at
-- http://www.eclipse.org/legal/epl-v10.html
--
-- This program and the accompanying materials are also made available
-- under the terms of the MIT public license which accompanies this
-- distribution, and is available at http://www.lua.org/license.html
--
-- Contributors:
--     Fabien Fleutot - API and implementation
--
--------------------------------------------------------------------------------

--*-lua-*-----------------------------------------------------------------------
--
-- Convert between various code representation formats. Some atomic
-- converters are written in extenso, others are composed automatically
-- by chaining the atomic ones together in a closure.
--
-- Supported formats are:
--
-- * luafile:    the name of a file containing sources.
-- * luastring:  these sources as a single string.
-- * lexstream:  a stream of lexemes.
-- * ast:        an abstract syntax tree.
-- * proto:      a (Yueliang) struture containing a high level 
--               representation of bytecode. Largely based on the 
--               Proto structure in Lua's VM
-- * luacstring: a string dump of the function, as taken by 
--               loadstring() and produced by string.dump().
-- * function:   an executable lua function in RAM.
--
--------------------------------------------------------------------------------

require 'checks'
require 'metalua.table'

local M  = { }

--------------------------------------------------------------------------------
-- Order of the transformations. if 'a' is on the left of 'b', then a 'a' can
-- be transformed into a 'b' (but not the other way around).
-- M.sequence goes for numbers to format names, M.order goes from format
-- names to numbers.
--------------------------------------------------------------------------------
M.sequence = {
	'srcfile',  'src', 'lexstream', 'ast', 'proto', 'bytecode', 'function' }

local arg_types = {
	srcfile    = { 'string', '?string' },
	src        = { 'string', '?string' },
	lexstream  = { 'lexer.stream', '?string' },
	ast        = { 'table', '?string' },
	proto      = { 'table', '?string' },
	bytecode   = { 'string', '?string' },
}

M.order = table.transpose(M.sequence)

local function check_ast(kind, ast)
	if not ast then return check_ast('block', kind) end
	assert(type(ast)=='table', "wrong AST type")
	local cfg = {}
	local function error2ast(error_node, ...)
		if not error_node.earlier then
			if error_node.tag=='Error' then
				cfg.errorfound = true
				cfg.errormsg = error_node[1]

				-- Try to extract error position in source
				local li = error_node.lineinfo and error_node.lineinfo.first

				-- Fill positions if undefined or not narrow enough
				if li and ( not cfg.positions or cfg.positions.offset < li.offset ) then
					cfg.positions = {
						column = li.column,
						line   = li.line,
						offset = li.offset
					}
				end
			else
				-- This block is for dealing with errors which are not error
				-- nodes. It would be soooo nice to get rid of it.
				-- TODO: Try to remove this bug when issue #20 is fixed
				local li
				for _, n in ipairs{ error_node, ... } do
					if n.lineinfo then
						li = n.lineinfo
						cfg.errorfound = true
						break
					end
				end
				local posmsg
				if li then
					local column = li.first.column
					local line   = li.first.line
					local offset = li.first.offset
					posmsg = string.format("line %d, char %d, offset %d",
					line, column, offset)
					cfg.positions = {
						column = column,
						line   = line,
						offset = offset
					}
				else
					posmsg = "unknown source position"     
				end
				local msg = "Invalid node "..
				(error_node.tag and "tag "..tostring(error_node.tag) or "without tag")..
				(posmsg and " at "..posmsg or "")
				cfg.errormsg = msg
			end
		end
	end
	local f = require 'metalua.treequery.walk' [kind]
	cfg.malformed=error2ast
	cfg.unknown=  error2ast
	cfg.error=    error2ast
	f(cfg, ast)
	return cfg.errorfound == nil, cfg.errormsg, cfg.positions
end

M.check_ast = check_ast

local function find_error(ast, nested)
	checks('table', '?table')
	nested = nested or { }
	if nested[ast] then return "Cyclic AST" end
	nested[ast]=true
	if ast.tag=='Error' then
		local pos = tostring(ast.lineinfo.first)
		return pos..": "..ast[1]
	end
	for _, item in ipairs(ast) do
		if type(item)=='table' then
			local err=find_error(item)
			if err then return err end
		end
	end
	nested[ast]=nil
	return nil
end

local CONV = { } -- conversion metatable __index

function CONV :srcfile_to_src(x, name)
	checks('metalua.compiler', 'string', '?string')
	name = name or '@'..x
	local f, msg = io.open (x, 'rb')
	if not f then error(msg) end
	local r, msg = f :read '*a'
	if not r then error("Cannot read file '"..x.."': "..msg) end
	f :close()
	return r, name
end

function CONV :src_to_lexstream(src, name)
	checks('metalua.compiler', 'string', '?string')
	local r = self.parser.lexer :newstream (src, name)
	return r, name
end

function CONV :lexstream_to_ast(lx, name)
	checks('metalua.compiler', 'lexer.stream', '?string')
	local r = self.parser.chunk(lx)
	r.source = name
	return r, name
end

function CONV :ast_to_proto(ast, name)
	checks('metalua.compiler', 'table', '?string')
	--table.print(ast, 'nohash', 1) io.flush()
	local err = find_error(ast)
	if err then  error(err) end
	local f = require 'metalua.compiler.bytecode.compile'.ast_to_proto
	return f(ast, name), name
end

function CONV :proto_to_bytecode(proto, name)
	local bc = require 'metalua.compiler.bytecode'
	return bc.proto_to_bytecode(proto), name
end

function CONV :bytecode_to_function(bc, name)
	checks('metalua.compiler', 'string', '?string')
	return loadstring(bc, name)
end

-- Create all sensible combinations
for i=1,#M.sequence do
	local src = M.sequence[i]
	for j=i+2, #M.sequence do
		local dst = M.sequence[j]
		local dst_name = src.."_to_"..dst
		local my_arg_types = arg_types[src]
		local functions = { }
		for k=i, j-1 do
			local name =  M.sequence[k].."_to_"..M.sequence[k+1]
			local f = assert(CONV[name], name)
			table.insert (functions, f)
		end
		CONV[dst_name] = function(self, a, b)
			checks('metalua.compiler', unpack(my_arg_types))
			for _, f in ipairs(functions) do
				a, b = f(self, a, b)
			end
			return a, b
		end
		--printf("Created M.%s out of %s", dst_name, table.concat(n, ', '))
	end
end


--------------------------------------------------------------------------------
-- This one goes in the "wrong" direction, cannot be composed.
--------------------------------------------------------------------------------
function CONV :function_to_bytecode(...) return string.dump(...) end

function CONV :ast_to_src(...)
	require 'metalua.package' -- ast_to_string isn't written in plain lua
	return require 'metalua.compiler.ast_to_src' (...)
end

local MT = { __index=CONV, __type='metalua.compiler' }

function M.new() 
	local parser = require 'metalua.compiler.parser' .new()
	local self = { parser = parser }
	setmetatable(self, MT)
	return self
end

return M