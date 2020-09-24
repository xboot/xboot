/*
 * kernel/command/cmd-overview.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    overview [framebuffer] [input]\r\n");
}

static const char zh_CN[] = X({
	"Window Info": "窗口信息",
	"Button": "按钮",
});

static void xui_style_color(struct xui_context_t * ctx, struct color_t * c)
{
	struct region_t * r;
	unsigned int id;

	xui_push_id(ctx, &c, sizeof(struct color_t *));
	id = xui_get_id(ctx, &c, sizeof(struct color_t *));
	r = xui_layout_next(ctx);
	xui_control_update(ctx, id, r, 0);
	if((ctx->active == id) && (ctx->mouse.down & XUI_MOUSE_LEFT))
	{
		xui_open_popup(ctx, "!cpopup");
	}
	xui_draw_checkerboard(ctx, r->x, r->y, r->w, r->h);
	xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, c);
	if(xui_begin_popup(ctx, "!cpopup"))
	{
		int width = 300;
		int height = 220;
		int sw = (width - ctx->style.layout.spacing * 3) / 4;
		float h, s, v, a;
		xui_layout_row(ctx, 1, (int[]){ width }, height);
		xui_colorpicker(ctx, c);
		xui_layout_row(ctx, 4, (int[]){ sw, sw, sw, sw }, 0);
		xui_number_uchar_ex(ctx, &c->r, 0, 255, 1, "R : %.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
		xui_number_uchar_ex(ctx, &c->g, 0, 255, 1, "G : %.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
		xui_number_uchar_ex(ctx, &c->b, 0, 255, 1, "B : %.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
		xui_number_uchar_ex(ctx, &c->a, 0, 255, 1, "A : %.0f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT);
		color_get_hsva(c, &h, &s, &v, &a);
		if(xui_number_float_ex(ctx, &h, 0, 1, 0.01, "H : %.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT))
			color_set_hsva(c, h, s, v, a);
		if(xui_number_float_ex(ctx, &s, 0, 1, 0.01, "S : %.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT))
			color_set_hsva(c, h, s, v, a);
		if(xui_number_float_ex(ctx, &v, 0, 1, 0.01, "V : %.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT))
			color_set_hsva(c, h, s, v, a);
		if(xui_number_float_ex(ctx, &a, 0, 1, 0.01, "A : %.2f", XUI_NUMBER_PRIMARY | XUI_OPT_TEXT_LEFT))
			color_set_hsva(c, h, s, v, a);
		xui_end_popup(ctx);
	}
	xui_pop_id(ctx);
}

static void style_window(struct xui_context_t * ctx)
{
	if(xui_begin_window(ctx, "Style Window", &(struct region_t){ 20, 20, (ctx->screen.w - 60) / 2, ctx->screen.h - 40 }))
	{
		if(xui_begin_tree(ctx, "Color"))
		{
			if(xui_begin_tree(ctx, "Primary"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.primary.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.primary.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.primary.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.primary.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.primary.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.primary.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.primary.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.primary.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.primary.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Secondary"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.secondary.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.secondary.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.secondary.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.secondary.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.secondary.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.secondary.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.secondary.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.secondary.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.secondary.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Success"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.success.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.success.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.success.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.success.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.success.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.success.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.success.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.success.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.success.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Info"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.info.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.info.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.info.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.info.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.info.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.info.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.info.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.info.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.info.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Warning"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.warning.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.warning.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.warning.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.warning.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.warning.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.warning.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.warning.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.warning.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.warning.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Danger"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.danger.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.danger.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.danger.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.danger.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.danger.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.danger.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.danger.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.danger.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.danger.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "Light"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.light.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.light.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.light.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.light.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.light.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.light.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.light.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.light.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.light.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			if(xui_begin_tree(ctx, "dark"))
			{
				if(xui_begin_tree(ctx, "Normal"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.dark.normal.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.dark.normal.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.dark.normal.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Hover"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.dark.hover.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.dark.hover.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.dark.hover.border);
					xui_end_tree(ctx);
				}
				if(xui_begin_tree(ctx, "Focus"))
				{
					xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
					xui_label(ctx, "Background :");
					xui_style_color(ctx, &ctx->style.dark.active.background);
					xui_label(ctx, "Foreground :");
					xui_style_color(ctx, &ctx->style.dark.active.foreground);
					xui_label(ctx, "Border :");
					xui_style_color(ctx, &ctx->style.dark.active.border);
					xui_end_tree(ctx);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}
		if(xui_begin_tree(ctx, "Font"))
		{
			xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
			xui_label(ctx, "Color :");
			xui_style_color(ctx, &ctx->style.font.color);
			xui_label(ctx, "Size :");
			xui_number_int(ctx, &ctx->style.font.size, 8, 72, 1);
			xui_end_tree(ctx);
		}
		if(xui_begin_tree(ctx, "Layout"))
		{
			xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
			xui_label(ctx, "Width :");
			xui_number_int(ctx, &ctx->style.layout.width, 8, 256, 1);
			xui_label(ctx, "Height :");
			xui_number_int(ctx, &ctx->style.layout.height, 8, 72, 1);
			xui_label(ctx, "Padding :");
			xui_number_int(ctx, &ctx->style.layout.padding, 0, 32, 1);
			xui_label(ctx, "Spacing :");
			xui_number_int(ctx, &ctx->style.layout.spacing, 0, 32, 1);
			xui_label(ctx, "Indent :");
			xui_number_int(ctx, &ctx->style.layout.indent, 0, 32, 1);
			xui_end_tree(ctx);
		}
		xui_end_window(ctx);
	}
}

static void overview_window(struct xui_context_t * ctx)
{
	static const char * wcstr[] = {
		"Primary",
		"Secondary",
		"Success",
		"Info",
		"Warning",
		"Danger",
		"Light",
		"Dark",
	};

	if(xui_begin_window(ctx, "Overview Window", &(struct region_t){ 40 + (ctx->screen.w - 60) / 2, 20, (ctx->screen.w - 60) / 2, ctx->screen.h - 40 }))
	{
		struct xui_container_t * win = xui_get_container(ctx);
		win->region.w = max(win->region.w, 48);
		win->region.h = max(win->region.h, 48);

		if(xui_begin_tree(ctx, T("Window Info")))
		{
			struct xui_container_t * win = xui_get_container(ctx);
			xui_layout_row(ctx, 2, (int[]){ 100, -1 }, 0);
			xui_label(ctx, "Position :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.x, win->region.y));
			xui_label(ctx, "Size :");
			xui_label(ctx, xui_format(ctx, "%d, %d", win->region.w, win->region.h));
			xui_label(ctx, "Frame :");
			xui_label(ctx, xui_format(ctx, "%d", ctx->frame));
			xui_label(ctx, "Fps :");
			xui_label(ctx, xui_format(ctx, "%d", ctx->fps));
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, T("Button")))
		{
			if(xui_begin_tree(ctx, "Normal Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0, wcstr[i], (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0, wcstr[i], (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0, wcstr[i], (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0, wcstr[i], (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Normal Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0xf000 + i, NULL, (i << 8) | XUI_OPT_TEXT_CENTER);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0xf000 + i, NULL, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0xf000 + i, NULL, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Icon Button"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 40);
				for(int i = 0; i < 8; i++)
				{
					xui_button_ex(ctx, 0xf000 + i, NULL, (i << 8) | XUI_OPT_TEXT_CENTER | XUI_BUTTON_ROUNDED | XUI_BUTTON_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Checkbox"))
		{
			static int states[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			for(int i = 0; i < 8; i++)
			{
				xui_checkbox_ex(ctx, wcstr[i], &states[i], (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Radio"))
		{
			static enum option_t {
				PRIMARY,
				SECONDARY,
				SUCCESS,
				INFO,
				WARNING,
				DANGER,
				LIGHT,
				DARK,
			} op = PRIMARY;
			if(xui_radio_ex(ctx, wcstr[0], (op == PRIMARY), XUI_RADIO_PRIMARY))
				op = PRIMARY;
			if(xui_radio_ex(ctx, wcstr[1], (op == SECONDARY), XUI_RADIO_SECONDARY))
				op = SECONDARY;
			if(xui_radio_ex(ctx, wcstr[2], (op == SUCCESS), XUI_RADIO_SUCCESS))
				op = SUCCESS;
			if(xui_radio_ex(ctx, wcstr[3], (op == INFO), XUI_RADIO_INFO))
				op = INFO;
			if(xui_radio_ex(ctx, wcstr[4], (op == WARNING), XUI_RADIO_WARNING))
				op = WARNING;
			if(xui_radio_ex(ctx, wcstr[5], (op == DANGER), XUI_RADIO_DANGER))
				op = DANGER;
			if(xui_radio_ex(ctx, wcstr[6], (op == LIGHT), XUI_RADIO_LIGHT))
				op = LIGHT;
			if(xui_radio_ex(ctx, wcstr[7], (op == DARK), XUI_RADIO_DARK))
				op = DARK;
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Toggle"))
		{
			static int states[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			for(int i = 0; i < 8; i++)
			{
				xui_toggle_ex(ctx, &states[i], (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Slider"))
		{
			static double h[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_slider_ex(ctx, &h[i], 0, 100, 0, (i << 8) | XUI_SLIDER_HORIZONTAL);
			}
			static double v[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_slider_ex(ctx, &v[i], 0, 100, 0, (i << 8) | XUI_SLIDER_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Number"))
		{
			if(xui_begin_tree(ctx, "Normal Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Number"))
			{
				static double n[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
				xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_number_ex(ctx, &n[i], -1000, 1000, 1, "%.2f", (i << 8) | XUI_OPT_TEXT_LEFT | XUI_NUMBER_ROUNDED | XUI_NUMBER_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Collapse"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				if(xui_collapse_ex(ctx, 0xf000 + i, wcstr[i], (i << 8) | XUI_OPT_TEXT_LEFT))
					xui_label(ctx, xui_format(ctx, "This is %s", wcstr[i]));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Textedit"))
		{
			static char buf[128];
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			if(xui_textedit(ctx, buf, sizeof(buf)) & (1 << 1))
			{
				buf[0] = 0;
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Colorpicker"))
		{
			static struct color_t c = { 0xff, 0xff, 0xff, 0xff };
			xui_layout_row(ctx, 1, (int[]){ -1 }, 100);
			xui_colorpicker(ctx, &c);
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Badge"))
		{
			if(xui_begin_tree(ctx, "Normal Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8));
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_ROUNDED);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Outline Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_OUTLINE);
				}
				xui_end_tree(ctx);
			}

			if(xui_begin_tree(ctx, "Rounded Outline Badge"))
			{
				xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 0);
				for(int i = 0; i < 8; i++)
				{
					xui_badge_ex(ctx, wcstr[i], (i << 8) | XUI_BADGE_ROUNDED | XUI_BADGE_OUTLINE);
				}
				xui_end_tree(ctx);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Progress"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_progress_ex(ctx, (i + 1) * 10, (i << 8) | XUI_PROGRESS_HORIZONTAL);
			}
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_progress_ex(ctx, (i + 1) * 10, (i << 8) | XUI_PROGRESS_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Radialbar"))
		{
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 100);
			for(int i = 0; i < 8; i++)
			{
				xui_radialbar_ex(ctx, (i + 1) * 10, (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Spinner"))
		{
			xui_layout_row(ctx, 3, (int[]){ 100, 100, -1 }, 60);
			for(int i = 0; i < 8; i++)
			{
				xui_spinner_ex(ctx, (i << 8));
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Split"))
		{
			xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
			for(int i = 0; i < 8; i++)
			{
				xui_split_ex(ctx, (i << 8) | XUI_SPLIT_HORIZONTAL);
			}
			xui_layout_row(ctx, 8, (int[]){ 24, 24, 24, 24, 24, 24, 24, -1 }, 160);
			for(int i = 0; i < 8; i++)
			{
				xui_split_ex(ctx, (i << 8) | XUI_SPLIT_VERTICAL);
			}
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Label"))
		{
			xui_label_ex(ctx, "Label align left", XUI_OPT_TEXT_LEFT);
			xui_label_ex(ctx, "Label align center", XUI_OPT_TEXT_CENTER);
			xui_label_ex(ctx, "Label align right", XUI_OPT_TEXT_RIGHT);
			xui_label_ex(ctx, "Label align top", XUI_OPT_TEXT_TOP);
			xui_label_ex(ctx, "Label align bottom", XUI_OPT_TEXT_BOTTOM);
			xui_end_tree(ctx);
		}

		if(xui_begin_tree(ctx, "Text"))
		{
			xui_text(ctx, "This is a long text to show dynamic window changes on multiline text");
			xui_end_tree(ctx);
		}

		xui_end_window(ctx);
	}
}

static void overview(struct xui_context_t * ctx)
{
	xui_begin(ctx);
	style_window(ctx);
	overview_window(ctx);
	xui_end(ctx);
}

static void overview_task(struct task_t * task, void * data)
{
	struct task_data_t * td = (struct task_data_t *)data;
	struct xui_context_t * ctx;

	if(td)
	{
		ctx = xui_context_alloc(td->fb, td->input, td);
		if(ctx)
		{
			switch(shash(setting_get("language", NULL)))
			{
			case 0x10d87d65: /* "zh-CN" */
				xui_load_lang(ctx, zh_CN, sizeof(zh_CN));
				break;
			default:
				break;
			}
			xui_loop(ctx, overview);
			xui_context_free(ctx);
		}
		task_data_free(td);
	}
}

static int do_overview(int argc, char ** argv)
{
	const char * fb = (argc >= 2) ? argv[1] : NULL;
	const char * input = (argc >= 3) ? argv[2] : NULL;
	task_resume(task_create(NULL, "overview", overview_task, task_data_alloc(fb, input, NULL), 0, 0));
	return 0;
}

static struct command_t cmd_overview = {
	.name	= "overview",
	.desc	= "show overview application of xui",
	.usage	= usage,
	.exec	= do_overview,
};

static __init void overview_cmd_init(void)
{
	register_command(&cmd_overview);
}

static __exit void overview_cmd_exit(void)
{
	unregister_command(&cmd_overview);
}

command_initcall(overview_cmd_init);
command_exitcall(overview_cmd_exit);
