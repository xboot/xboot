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

struct render_t * fb_create(struct fb_t * fb)
{
	struct render_t * render;
	void * pixels;

	pixels = memalign(4, LCD_WIDTH * LCD_HEIGHT * LCD_BPP / 8);
	if(!pixels)
		return NULL;

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
	render->alloc = render_sw_alloc;
	render->free = render_sw_free;
	render->fill = render_sw_fill;
	render->blit = render_sw_blit;
	render->scale = render_sw_scale;
	render->rotate = render_sw_rotate;

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		if(render->pixels)
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

static int fb_ioctl(struct fb_t * fb, int cmd, void * arg)
{
	static u8_t brightness = 0;
	u8_t * p;

	switch(cmd)
	{
	case IOCTL_SET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		brightness = (*p) & 0xff;
		return 0;

	case IOCTL_GET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		*p = brightness;
		return 0;

	default:
		break;
	}

	return -1;
}

static struct fb_t realview_fb = {
	.name		= "fb0",
	.init		= fb_init,
	.exit		= fb_exit,
	.create		= fb_create,
	.destroy	= fb_destroy,
	.present	= fb_present,
	.ioctl		= fb_ioctl,
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
