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

local package = package

require 'metalua.table'
require 'metalua.string'

package.metalua_extension_prefix = 'metalua.extension.'

package.mpath = package.mpath or os.getenv 'LUA_MPATH' or
   './?.mlua;/usr/local/share/lua/5.1/?.mlua;'..
   '/usr/local/share/lua/5.1/?/init.mlua;'..
   '/usr/local/lib/lua/5.1/?.mlua;'..
   '/usr/local/lib/lua/5.1/?/init.mlua'


----------------------------------------------------------------------
-- resc(k) returns "%"..k if it's a special regular expression char,
-- or just k if it's normal.
----------------------------------------------------------------------
local regexp_magic = table.transpose{
   "^", "$", "(", ")", "%", ".", "[", "]", "*", "+", "-", "?" }
local function resc(k)
   return regexp_magic[k] and '%'..k or k
end

----------------------------------------------------------------------
-- Take a Lua module name, return the open file and its name,
-- or <false> and an error message.
----------------------------------------------------------------------
function package.findfile(name, path_string)
   local config_regexp = ("([^\n])\n"):rep(5):sub(1, -2)
   local dir_sep, path_sep, path_mark, execdir, igmark =
      package.config:strmatch (config_regexp)
   name = name:gsub ('%.', dir_sep)
   local errors = { }
   local path_pattern = string.format('[^%s]+', resc(path_sep))
   for path in path_string:gmatch (path_pattern) do
      --printf('path = %s, rpath_mark=%s, name=%s', path, resc(path_mark), name)
      local filename = path:gsub (resc (path_mark), name)
      --printf('filename = %s', filename)
      local file = io.open (filename, 'r')
      if file then return file, filename end
      table.insert(errors, string.format("\tno lua file %q", filename))
   end
   return false, '\n'..table.concat(errors, "\n")..'\n'
end

----------------------------------------------------------------------
-- Load a metalua source file.
----------------------------------------------------------------------
function package.metalua_loader (name)
   local file, filename_or_msg = package.findfile (name, package.mpath)
   if not file then return filename_or_msg end
   local luastring = file:read '*a'
   file:close()
   local mlc = require 'metalua.compiler'.new()
   return mlc :src_to_function (luastring, name)
end

----------------------------------------------------------------------
-- Placed after lua/luac loader, so precompiled files have
-- higher precedence.
----------------------------------------------------------------------
table.insert(package.loaders, package.metalua_loader)

----------------------------------------------------------------------
-- Load an extension.
----------------------------------------------------------------------
function extension (name, mlp)
    local complete_name = package.metalua_extension_prefix..name
    -- TODO: pass mlp around
    local extend_func = require (complete_name)
    local ast =extend_func(mlp)
    return ast
end

return package
