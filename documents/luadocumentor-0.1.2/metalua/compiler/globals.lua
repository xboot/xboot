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
-- Override Lua's default compilation functions, so that they support Metalua
-- rather than only plain Lua
--------------------------------------------------------------------------------

local mlc = require 'metalua.compiler'

local M = { }

-- Original versions
local original_lua_versions = {
    load       = load,
    loadfile   = loadfile,
    loadstring = loadstring,
    dofile     = dofile }

local lua_loadstring = loadstring
local lua_loadfile = loadfile

function M.loadstring(str, name)
   if type(str) ~= 'string' then error 'string expected' end
   if str:match '^\027LuaQ' then return lua_loadstring(str) end
   local n = str:match '^#![^\n]*\n()'
   if n then str=str:sub(n, -1) end
   -- FIXME: handle erroneous returns (return nil + error msg)
   return mlc.new():src_to_function(str, name)
end

function M.loadfile(filename)
   local f, err_msg = io.open(filename, 'rb')
   if not f then return nil, err_msg end
   local success, src = pcall( f.read, f, '*a')
   pcall(f.close, f)
   if success then return M.loadstring (src, '@'..filename)
   else return nil, src end
end

function M.load(f, name)
   while true do
      local x = f()
      if not x then break end
      assert(type(x)=='string', "function passed to load() must return strings")
      table.insert(acc, x)
   end
   return M.loadstring(table.concat(x))
end

function M.dostring(src)
   local f, msg = M.loadstring(src)
   if not f then error(msg) end
   return f()
end

function M.dofile(name)
   local f, msg = M.loadfile(name)
   if not f then error(msg) end
   return f()
end

-- Export replacement functions as globals
for name, f in pairs(M) do _G[name] = f end

-- To be done *after* exportation
M.lua = original_lua_versions

return M