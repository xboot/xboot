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

--------------------------------------------------------------------------------
--
-- Non-Lua syntax extensions
--
--------------------------------------------------------------------------------

local gg        = require 'metalua.grammar.generator'
local mlp       = require 'metalua.compiler.parser.common'
local mlp_lexer = require 'metalua.compiler.parser.lexer'
local mlp_expr  = require 'metalua.compiler.parser.expr'
local mlp_stat  = require 'metalua.compiler.parser.stat'
local mlp_misc  = require 'metalua.compiler.parser.misc'

local expr = mlp_expr.expr

local M = { }

--------------------------------------------------------------------------------
-- Algebraic Datatypes
--------------------------------------------------------------------------------
local function adt (lx)
   local node = mlp_misc.id (lx)
   local tagval = node[1]
   local tagkey = {tag="Pair", {tag="String", "tag"}, {tag="String", tagval} }
   if lx:peek().tag == "String" or lx:peek().tag == "Number" then
      return { tag="Table", tagkey, lx:next() }
   elseif lx:is_keyword (lx:peek(), "{") then
      local x = mlp.table (lx)
      table.insert (x, 1, tagkey)
      return x
   else return { tag="Table", tagkey } end
end

M.adt = gg.sequence{ "`", adt, builder = unpack }

expr.primary :add(M.adt)

--------------------------------------------------------------------------------
-- Anonymous lambda
--------------------------------------------------------------------------------
M.lambda_expr = gg.sequence{
   "|", mlp_expr.func_params_content, "|", expr,
   builder = function (x)
      local li = x[2].lineinfo
      return { tag="Function", x[1],
               { {tag="Return", x[2], lineinfo=li }, lineinfo=li } }
   end }

-- In an earlier version, lambda_expr took an expr_list rather than an expr
-- after the 2nd bar. However, it happened to be much more of a burden than an
-- help, So finally I disabled it. If you want to return several results,
-- use the long syntax.
--------------------------------------------------------------------------------
-- local lambda_expr = gg.sequence{
--    "|", func_params_content, "|", expr_list,
--    builder= function (x)
--       return {tag="Function", x[1], { {tag="Return", unpack(x[2]) } } } end }

expr.primary :add (M.lambda_expr)

--------------------------------------------------------------------------------
-- Allows to write "a `f` b" instead of "f(a, b)". Taken from Haskell.
-- This is not part of Lua 5.1 syntax, so it's added to the expression
-- afterwards, so that it's easier to disable.
--------------------------------------------------------------------------------
function M.expr_in_backquotes (lx) return expr(lx, 35) end

expr.infix :add{ name = "infix function",
   "`", M.expr_in_backquotes, "`", prec = 35, assoc="left",
   builder = function(a, op, b) return {tag="Call", op[1], a, b} end }


--------------------------------------------------------------------------------
-- table.override assignment
--------------------------------------------------------------------------------

mlp_lexer.lexer:add "<-"
mlp_stat.stat.assignments["<-"] = function (a, b)
   assert( #a==1 and #b==1, "No multi-args for '<-'")
   return { tag="Call", { tag="Index", { tag="Id", "table" },
                                       { tag="String", "override" } },
                        a[1], b[1]}
end

--------------------------------------------------------------------------------
-- C-style op+assignments
-- TODO: no protection against side-effects in LHS vars.
--------------------------------------------------------------------------------
local function op_assign(kw, op)
   local function rhs(a, b)
      return { tag="Op", op, a, b }
   end
   local function f(a,b)
       if #a ~= #b then return gg.parse_error "assymetric operator+assignment" end
       local right = { }
       local r = { tag="Set", a, right }
       for i=1, #a do right[i] = { tag="Op", op, a[i], b[i] } end
       return r
   end
   mlp.lexer:add (kw)
   mlp.stat.assignments[kw] = f
end

local ops = { add='+='; sub='-='; mul='*='; div='/=' }
for ast_op_name, keyword in pairs(ops) do op_assign(keyword, ast_op_name) end

return M