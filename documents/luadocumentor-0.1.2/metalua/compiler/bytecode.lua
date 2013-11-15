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

local compile = require 'metalua.compiler.bytecode.compile'
local ldump   = require 'metalua.compiler.bytecode.ldump'

local M = { }

M.ast_to_proto      = compile.ast_to_proto
M.proto_to_bytecode = ldump.dump_string
M.proto_to_file     = ldump.dump_file

return M