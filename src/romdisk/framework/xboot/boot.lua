---
-- Builtin module
--
Json = require "builtin.json"
Stopwatch = require "builtin.stopwatch"
Base64 = require "builtin.base64"
Matrix = require "builtin.matrix"
Easing = require "builtin.easing"
Object = require "builtin.object"
Pattern = require "builtin.pattern"
Texture = require "builtin.texture"
Ninepatch = require "builtin.ninepatch"
Shape = require "builtin.shape"
Font = require "builtin.font"
Display = require "builtin.display"

---
-- External display module
--
DisplayObject = require "xboot.display.DisplayObject"
DisplayImage = require "xboot.display.DisplayImage"
DisplayImageMask = require "xboot.display.DisplayImageMask"
DisplayNinepatch = require "xboot.display.DisplayNinepatch"
DisplayShape = require "xboot.display.DisplayShape"
DisplayText = require "xboot.display.DisplayText"

---
-- External core module
--
Assets = require "xboot.core.Assets"
TexturePacker = require "xboot.core.TexturePacker"
Stage = require "xboot.core.Stage"

---
-- External timer module
--
Timer = require "xboot.timer.Timer"
TimerManager = require "xboot.timer.TimerManager"

---
-- External widget module
--
Widget = {
	Button = require "xboot.widget.Button",
	CheckBox = require "xboot.widget.CheckBox",
	RadioButton = require "xboot.widget.RadioButton",
	Stepper = require "xboot.widget.Stepper",
	Slider = require "xboot.widget.Slider",
}

assets = Assets.new()
timermanager = TimerManager.new()
require("main")
