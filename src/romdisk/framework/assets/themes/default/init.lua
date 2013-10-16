local function D(name)
	return ("assets/themes/default/" .. name)
end

return {
	checkbox = {
		width = 33,
		height = 33,
		imageChecked = D("checkbox-checked.png"),
		imageUnchecked = D("checkbox-unchecked.png"),
	},
	
	radiobutton = {
		width = 33,
		height = 33,
		imageChecked = D("radiobutton-checked.png"),
		imageUnchecked = D("radiobutton-unchecked.png"),
	},
}
