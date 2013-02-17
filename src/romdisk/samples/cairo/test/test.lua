local M = {}

function M.test()
	local cairo = require "org.xboot.cairo"
	local M_PI = math.pi
	local PI = M_PI
	
--	local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, IMG_WD, IMG_HT)
--	local cr = cairo.create(cs)

	-- The operators to test, arranged as they are in the output image.
	local OPERATORS = {
		cairo.OPERATOR_ADD, cairo.OPERATOR_ATOP, cairo.OPERATOR_CLEAR, cairo.OPERATOR_DEST, cairo.OPERATOR_DEST_ATOP, cairo.OPERATOR_DEST_IN, cairo.OPERATOR_DEST_OUT,
		cairo.OPERATOR_DEST_OVER, cairo.OPERATOR_IN, cairo.OPERATOR_OUT, cairo.OPERATOR_OVER, cairo.OPERATOR_SATURATE, cairo.OPERATOR_SOURCE, cairo.OPERATOR_XOR,
	}

	local MARGIN = 15
	local OP_WD, OP_HT = 60, 90     -- height includes space for text
	local OPS_PER_ROW = 7
	local NUM_ROWS = #OPERATORS / OPS_PER_ROW
	local IMG_WD = OPS_PER_ROW * OP_WD + (OPS_PER_ROW + 1) * MARGIN
	local IMG_HT = NUM_ROWS * OP_HT + (NUM_ROWS + 1) * MARGIN

	local function draw (cr, cs, x, y, w, h, op)
		local first = cairo.surface_create_similar(cs, cairo.CONTENT_COLOR_ALPHA, w, h)
		local second = cairo.surface_create_similar(cs, cairo.CONTENT_COLOR_ALPHA, w, h)

		local first_cr = cairo.create(first)
		first_cr:set_source_rgb(0, 0, 0.4)
		first_cr:rectangle(x, y, 50, 50)
		first_cr:fill()

		local second_cr = cairo.create(second)
		second_cr:set_source_rgb(0.5, 0.5, 0)
		second_cr:rectangle(x + 10, y + 20, 50, 50)
		second_cr:fill()

		first_cr:set_operator(op)
		first_cr:set_source_surface(second, 0, 0)
		first_cr:paint()

		cr:set_source_surface(first, 0, 0)
		cr:paint()

		-- Put a label underneath each example with the name of the operator.
		cr:set_source_rgb(0, 0, 0)
		local label_width = 20;--cr:text_extents(op).width
		cr:move_to(x + (OP_WD - label_width) / 2, y + OP_HT - 3)    -- centered
		--cr:show_text(op)
	end

	local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, IMG_WD, IMG_HT)
	local cr = cairo.create(cs)
	
	-- White background.
	cr:set_source_rgb(1, 1, 1)
	cr:paint()

--	cr:select_font_face("Bitstream Vera Sans")
--	cr:set_font_size(12)

	local x, y = MARGIN, MARGIN
	for i, op in ipairs(OPERATORS) do
		draw(cr, cs, x, y, IMG_WD, IMG_HT, op)
		x = x + OP_WD + MARGIN
		if i % OPS_PER_ROW == 0 then x = MARGIN; y = y + OP_HT + MARGIN end
	end

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
