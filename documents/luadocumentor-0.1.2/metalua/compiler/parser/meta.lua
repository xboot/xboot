-------------------------------------------------------------------------------
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
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--
-- Summary: Meta-operations: AST quasi-quoting and splicing
--
-------------------------------------------------------------------------------

--------------------------------------------------------------------------------
--
-- Exported API:
-- * [mlp.splice_content()]
-- * [mlp.quote_content()]
--
--------------------------------------------------------------------------------

local gg       = require 'metalua.grammar.generator'
local mlp      = require 'metalua.compiler.parser.common'
local M        = { }

--------------------------------------------------------------------------------
-- External splicing: compile an AST into a chunk, load and evaluate
-- that chunk, and replace the chunk by its result (which must also be
-- an AST).
--------------------------------------------------------------------------------

function M.splice (ast)
    -- TODO: should there be one mlc per splice, or per parser instance?
    local mlc = require 'metalua.compiler'.new()
    local f = mlc :ast_to_function(ast, '=splice')
    local result=f(mlp)
    return result
end

--------------------------------------------------------------------------------
-- Going from an AST to an AST representing that AST
-- the only key being lifted in this version is ["tag"]
--------------------------------------------------------------------------------
function M.quote (t)
   --print("QUOTING:", table.tostring(t, 60,'nohash'))
   local cases = { }
   function cases.table (t)
      local mt = { tag = "Table" }
      --table.insert (mt, { tag = "Pair", quote "quote", { tag = "True" } })
      if t.tag == "Splice" then
         assert (#t==1, "Invalid splice")
         local sp = t[1]
         return sp
      elseif t.tag then
         table.insert (mt, { tag="Pair", M.quote "tag", M.quote(t.tag) })
      end
      for _, v in ipairs (t) do
         table.insert (mt, M.quote(v))
      end
      return mt
   end
   function cases.number (t) return { tag = "Number", t, quote = true } end
   function cases.string (t) return { tag = "String", t, quote = true } end
   function cases.boolean (t) return { tag = t and "True" or "False", t, quote = true } end
   local f = cases [type(t)]
   if f then return f(t) else error ("Cannot quote an AST containing "..tostring(t)) end
end

--------------------------------------------------------------------------------
-- when this variable is false, code inside [-{...}] is compiled and
-- avaluated immediately. When it's true (supposedly when we're
-- parsing data inside a quasiquote), [-{foo}] is replaced by
-- [`Splice{foo}], which will be unpacked by [quote()].
--------------------------------------------------------------------------------
M.in_a_quote = false

--------------------------------------------------------------------------------
-- Parse the inside of a "-{ ... }"
--------------------------------------------------------------------------------
function M.splice_content (lx)
   local parser_name = "expr"
   if lx:is_keyword (lx:peek(2), ":") then
      local a = lx:next()
      lx:next() -- skip ":"
      assert (a.tag=="Id", "Invalid splice parser name")
      parser_name = a[1]
  end
  local parser = require 'metalua.compiler.parser'.new()
  local ast = parser [parser_name](lx)
   if M.in_a_quote then
      --printf("SPLICE_IN_QUOTE:\n%s", _G.table.tostring(ast, "nohash", 60))
      return { tag="Splice", ast }
   else
      if parser_name == "expr" then ast = { { tag="Return", ast } }
      elseif parser_name == "stat"  then ast = { ast }
      elseif parser_name ~= "block" then
         error ("splice content must be an expr, stat or block") end
      --printf("EXEC THIS SPLICE:\n%s", _G.table.tostring(ast, "nohash", 60))
      return M.splice (ast)
   end
end

--------------------------------------------------------------------------------
-- Parse the inside of a "+{ ... }"
--------------------------------------------------------------------------------
function M.quote_content (lx)
   local parser
   if lx:is_keyword (lx:peek(2), ":") then -- +{parser: content }
      parser = mlp[mlp.id(lx)[1]]
      lx:next()
   else -- +{ content }
      parser = mlp.expr
   end

   local prev_iq = M.in_a_quote
   M.in_a_quote = true
   --print("IN_A_QUOTE")
   local content = parser (lx)
   local q_content = M.quote (content)
   M.in_a_quote = prev_iq
   return q_content
end

return M
