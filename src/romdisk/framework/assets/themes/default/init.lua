local function D(name)
	return ("assets/themes/default/" .. name)
end

return {
	button = {
		imageNormal = D("button-normal.9.png"),
		imagePressed = D("button-pressed.9.png"),
		imageDisabled = D("button-disabled.9.png"),
	},

	checkbox = {
		imageChecked = D("checkbox-checked.png"),
		imageUnchecked = D("checkbox-unchecked.png"),
	},

	radiobutton = {
		imageChecked = D("radiobutton-checked.png"),
		imageUnchecked = D("radiobutton-unchecked.png"),
	},
}
