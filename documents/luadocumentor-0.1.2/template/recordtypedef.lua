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
return [[#
# --
# -- Descriptions
# --
#if _recordtypedef.shortdescription and #_recordtypedef.shortdescription > 0 then
	$( format( _recordtypedef.shortdescription ) )
#end
#if _recordtypedef.description and #_recordtypedef.description > 0 then
	$( format( _recordtypedef.description ) )
#end
#--
#-- Describe usage
#--
#if _recordtypedef.metadata and _recordtypedef.metadata.usage then
	$( applytemplate(_recordtypedef.metadata.usage, i) )
#end
# --
# -- Describe type fields
# --
#if not isempty( _recordtypedef.fields ) then
	<h$(i)>Field(s)</h$(i)>
#	for name, item in sortedpairs( _recordtypedef.fields ) do
		$( applytemplate(item, i) )
#	end
#end ]]
