local Pattern = Pattern

local function D(name)
	return ("assets/themes/ios/" .. name)
end

local common = {
	fontFamily = "assets/fonts/Roboto-Regular.ttf",
	fontSize = 24,
	textPatternNormal = Pattern.color(1, 1, 1),
	textPatternPressed = Pattern.color(1, 1, 1),
	textPatternDisabled = Pattern.color(1, 1, 1),
}

return {
	button = {
		imageNormal = D("button-normal.9.png"),
		imagePressed = D("button-pressed.9.png"),
		imageDisabled = D("button-disabled.9.png"),
		fontFamily = common.fontFamily,
		fontSize = common.fontSize,
		textPatternNormal = common.textPatternNormal,
		textPatternPressed = common.textPatternPressed,
		textPatternDisabled = common.textPatternDisabled,
	},

	checkbox = {
		imageOnNormal = D("checkbox-on-normal.png"),
		imageOnPressed = D("checkbox-on-pressed.png"),
		imageOnDisabled = D("checkbox-on-disabled.png"),
		imageOffNormal = D("checkbox-off-normal.png"),
		imageOffPressed = D("checkbox-off-pressed.png"),
		imageOffDisabled = D("checkbox-off-disabled.png"),
	},

	radiobutton = {
		imageOnNormal = D("radiobutton-on-normal.png"),
		imageOnPressed = D("radiobutton-on-pressed.png"),
		imageOnDisabled = D("radiobutton-on-disabled.png"),
		imageOffNormal = D("radiobutton-off-normal.png"),
		imageOffPressed = D("radiobutton-off-pressed.png"),
		imageOffDisabled = D("radiobutton-off-disabled.png"),
	},

	stepper = {
		imageMinusNormal = D("stepper-minus-normal.png"),
		imageMinusPressed = D("stepper-minus-pressed.png"),
		imageMinusDisabled = D("stepper-minus-disabled.png"),
		imagePlusNormal = D("stepper-plus-normal.png"),
		imagePlusPressed = D("stepper-plus-pressed.png"),
		imagePlusDisabled = D("stepper-plus-disabled.png"),
	},

	slider = {
		imageTrack = D("slider-track.9.png"),
		imageBackground = D("slider-background.9.png"),
		imageThumbNormal = D("slider-thumb-normal.png"),
		imageThumbPressed = D("slider-thumb-pressed.png"),
		imageThumbDisabled = D("slider-thumb-disabled.png"),
	},
}
