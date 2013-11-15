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

-- Export all public APIs from sub-modules, squashed into a flat spacename

local MT = { __type='metalua.compiler.parser' }

local function new()
    local mod_names = { "common", "expr", "lexer", "meta", "misc", "stat", "table", "ext", "annot" }

    for name, _ in pairs(package.loaded) do
        local x = name :match '^metalua.compiler.parser.(.*)'
        if x then
            local found = false
            for _, y in pairs(mod_names) do
                if x==y then found=true; break end
            end
            --if found then print (">> found "..x)
            --else print(">> not found: "..x) end
        end
    end

    -- Unload parser modules
    for _, mod_name in ipairs(mod_names) do
        package.loaded["metalua.compiler.parser."..mod_name] = nil
    end

    local M = require 'metalua.compiler.parser.common'

    for _, mod_name in ipairs(mod_names) do
        -- TODO: expose sub-modules as nested tables? 
        -- Not sure: it might be confusing, will clash with API names, e.g. for expr
        local mod = require ("metalua.compiler.parser."..mod_name)
        assert (type (mod) == 'table')
        for api_name, val in pairs(mod) do
            assert(not M[api_name])
            M[api_name] = val
        end
    end

    -- TODO: remove or make somehow optional the 'ext' module

    return setmetatable(M, MT)
end

return { new = new }
