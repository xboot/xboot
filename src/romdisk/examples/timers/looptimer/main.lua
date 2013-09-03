local looptimer = Timer:new(0.6, 0, function(t, e)
	print("time interval:" .. e.time .. ", iteration count:" .. e.count)
end)
