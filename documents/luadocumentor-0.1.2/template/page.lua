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
return
[[<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
#if _page.headers and #_page.headers > 0 then
	<head>
#	for _, header in ipairs(_page.headers) do
		$(header)
#	end
	</head>
#end
<body>
<div id="container">
<div id="product">
	<div id="product_logo"></div>
	<div id="product_name"><big><b></b></big></div>
	<div id="product_description"></div>
</div>
<div id="main">
# --
# -- Generating lateral menu
# --
	<div id="navigation">
#	local index = 'index'
#	if _page.modules then
		<h2>Modules</h2>
#		-- Check if an index is defined
#		if _page.modules [ index ] then
#			local module = _page.modules [ index ]
			<ul><li>
#			if module ~= _page.currentmodule then
				<a href="$( linkto(module) )">$(module.name)</a>
#			else
				$(module.name)
#			end
			</li></ul>
#		end
#
		<ul>
#		-- Generating links for all modules
#		for _, module in sortedpairs( _page.modules ) do
#			--  Except for current one
#			if module.name ~= index then
#				if module ~= _page.currentmodule then
					<li><a href="$( linkto(module) )">$(module.name)</a></li>
#				else
					<li>$(module.name)</li>
#				end
#			end
#		end
		</ul>
#	end
	</div>
	$( applytemplate(_page.currentmodule) )
</div>
</body>
</html>
]]
