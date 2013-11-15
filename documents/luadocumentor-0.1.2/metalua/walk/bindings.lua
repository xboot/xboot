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

local W = require 'metalua.treequery.walk'

local function bindings(ast)
	local cfg     = { }
	local locals  = { }
	local globals = { }

	function cfg.occurrence(binding_ctx, id_occ, ...)
		if binding_ctx then
			local binder = binding_ctx[2]
			local id_name = id_occ[1]
			if not locals[binder][id_name] then
				locals[binder][id_name] = {}
			end
			table.insert(locals[binder][id_name], id_occ)
		else
			local occ_name = id_occ[1]
			local t = globals[occ_name]
			if t then table.insert(t, id_occ) else globals[occ_name]={ id_occ } end
		end	
	end

	function cfg.binder(id, stat, ...)
		local id_name = id[1]
		if not locals[stat] then
			locals[stat] = {}
		end
		if not locals[stat][id_name] then
			locals[stat][id_name] = {}
		end
	end
	W.guess(cfg, ast)
	return locals, globals
end

return bindings
