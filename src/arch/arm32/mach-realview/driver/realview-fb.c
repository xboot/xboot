/*
 * driver/realview-fb.c
 *
 * realview framebuffer drivers. prime cell lcd controller (pl110)
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <fb/fb.h>
#include <realview/reg-lcd.h>

#define	LCD_WIDTH		(800)
#define	LCD_HEIGHT		(480)
#define	LCD_BPP			(32)

#define HBP				(2)
#define HFP				(2)
#define HSW				(2)
#define VBP				(2)
#define VFP				(2)
#define VSW				(2)
#define	REGS_TIM0		( (HBP<<24) | (HFP<<16) | (HSW<<8) | ((LCD_WIDTH/16-1)<<2) )
#define	REGS_TIM1		( (VBP<<24) | (VFP<<16) | (VSW<<10) | ((LCD_HEIGHT-1)<<0) )
#define	REGS_TIM2		( (1<<26) | ((LCD_WIDTH/16-1)<<16) | (1<<5) | (1<<0) )
#define	REGS_TIM3		( (0<<0) )

static void fb_init(struct fb_t * fb)
{
	/* initial lcd controller */
	writel(REALVIEW_CLCD_TIM0, REGS_TIM0);
	writel(REALVIEW_CLCD_TIM1, REGS_TIM1);
	writel(REALVIEW_CLCD_TIM2, REGS_TIM2);
	writel(REALVIEW_CLCD_TIM3, REGS_TIM3);

	/* disable all lcd interrupts */
	writel(REALVIEW_CLCD_IMSC, 0x0);

	/* set lcd controller register */
	writel(REALVIEW_CLCD_CNTL, REALVIEW_CNTL_LCDBPP24 | REALVIEW_CNTL_LCDTFT | REALVIEW_CNTL_BGR);

	/* enable lcd output */
	writel(REALVIEW_CLCD_CNTL, (readl(REALVIEW_CLCD_CNTL) | REALVIEW_CNTL_LCDEN | REALVIEW_CNTL_LCDPWR));
}

static void fb_exit(struct fb_t * fb)
{
	return;
}

static int fb_xcursor(struct fb_t * fb, int ox)
{
	static int xpos = 0;

	if(ox == 0)
		return xpos;

	xpos = xpos + ox;
	if(xpos < 0)
		xpos = 0;
	if(xpos > LCD_WIDTH - 1)
		xpos = LCD_WIDTH - 1;

	return xpos;
}

static int fb_ycursor(struct fb_t * fb, int oy)
{
	static int ypos = 0;

	if(oy == 0)
		return ypos;

	ypos = ypos + oy;
	if(ypos < 0)
		ypos = 0;
	if(ypos > LCD_HEIGHT - 1)
		ypos = LCD_HEIGHT - 1;

	return ypos;
}

static int fb_backlight(struct fb_t * fb, int brightness)
{
	static int level = 0;

	if( (brightness < 0) || (brightness > 255) )
		return level;

	level = brightness;
	return level;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct render_t * render;
	void * pixels;
	size_t size;

	size = LCD_WIDTH * LCD_HEIGHT * LCD_BPP / 8;
	pixels = memalign(4, size);
	if(!pixels)
		return NULL;
	memset(pixels, 0, size);

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = LCD_WIDTH;
	render->height = LCD_HEIGHT;
	render->pitch = (LCD_WIDTH * 4 + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_priv_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		sw_render_destroy_priv_data(render);
		free(render->pixels);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	void * pixels = render->pixels;

	if(pixels)
	{
		writel(REALVIEW_CLCD_UBAS, ((u32_t)pixels));
		writel(REALVIEW_CLCD_LBAS, ((u32_t)pixels + LCD_WIDTH * LCD_HEIGHT * LCD_BPP / 8));
	}
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static struct fb_t realview_fb = {
	.name		= "fb0",
	.init		= fb_init,
	.exit		= fb_exit,
	.xcursor	= fb_xcursor,
	.ycursor	= fb_ycursor,
	.backlight	= fb_backlight,
	.create		= fb_create,
	.destroy	= fb_destroy,
	.present	= fb_present,
	.suspend	= fb_suspend,
	.resume		= fb_resume,
};

static __init void realview_fb_init(void)
{
	if(register_framebuffer(&realview_fb))
		LOG("Register framebuffer driver '%s'", realview_fb.name);
	else
		LOG("Failed to register framebuffer driver '%s'", realview_fb.name);
}

static __exit void realview_fb_exit(void)
{
	if(unregister_framebuffer(&realview_fb))
		LOG("Unregister framebuffer driver '%s'", realview_fb.name);
	else
		LOG("Failed to unregister framebuffer driver '%s'", realview_fb.name);
}

device_initcall(realview_fb_init);
device_exitcall(realview_fb_exit);
