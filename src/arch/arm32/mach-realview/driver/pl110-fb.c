/*
 * driver/pl110-fb.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <pl110-fb.h>

#define CLCD_TIM0		(0x000)
#define CLCD_TIM1		(0x004)
#define CLCD_TIM2		(0x008)
#define CLCD_TIM3		(0x00c)
#define CLCD_UBAS		(0x010)
#define CLCD_LBAS		(0x014)
#define CLCD_CNTL		(0x018)
#define CLCD_IMSC		(0x01c)
#define CLCD_RIS		(0x020)
#define CLCD_MIS		(0x024)
#define CLCD_ICR		(0x028)
#define CLCD_UCUR		(0x02c)
#define CLCD_LCUR		(0x030)
#define CLCD_PALETTE	(0x200)

#define CNTL_LCDEN		(1 << 0)
#define CNTL_LCDBPP1	(0 << 1)
#define CNTL_LCDBPP2	(1 << 1)
#define CNTL_LCDBPP4	(2 << 1)
#define CNTL_LCDBPP8	(3 << 1)
#define CNTL_LCDBPP16	(4 << 1)
#define CNTL_LCDBPP24	(5 << 1)
#define CNTL_LCDBW		(1 << 4)
#define CNTL_LCDTFT		(1 << 5)
#define CNTL_LCDMONO8	(1 << 6)
#define CNTL_LCDDUAL	(1 << 7)
#define CNTL_BGR		(1 << 8)
#define CNTL_BEBO		(1 << 9)
#define CNTL_BEPO		(1 << 10)
#define CNTL_LCDPWR		(1 << 11)

struct pl110_fb_pdata_t {
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bpp;
	int h_fp;
	int h_bp;
	int h_sw;
	int v_fp;
	int v_bp;
	int v_sw;
	struct led_t * backlight;
	virtual_addr_t virt;
};

static void fb_init(struct fb_t * fb)
{
	struct pl110_fb_pdata_t * pdat = (struct pl110_fb_pdata_t *)fb->priv;

	write32(pdat->virt + CLCD_TIM0, (pdat->h_bp<<24) | (pdat->h_fp<<16) | (pdat->h_sw<<8) | ((pdat->width/16-1)<<2));
	write32(pdat->virt + CLCD_TIM1, (pdat->v_bp<<24) | (pdat->v_fp<<16) | (pdat->v_sw<<10) | ((pdat->height-1)<<0));
	write32(pdat->virt + CLCD_TIM2, (1<<26) | ((pdat->width/16-1)<<16) | (1<<5) | (1<<0));
	write32(pdat->virt + CLCD_TIM3, (0<<0));
	write32(pdat->virt + CLCD_IMSC, 0x0);
	write32(pdat->virt + CLCD_CNTL, CNTL_LCDBPP24 | CNTL_LCDTFT | CNTL_BGR);
	write32(pdat->virt + CLCD_CNTL, (read32(pdat->virt + CLCD_CNTL) | CNTL_LCDEN | CNTL_LCDPWR));
}

static void fb_exit(struct fb_t * fb)
{
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct pl110_fb_pdata_t * pdat = (struct pl110_fb_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct pl110_fb_pdata_t * pdat = (struct pl110_fb_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct pl110_fb_pdata_t * pdat = (struct pl110_fb_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * (pdat->bpp / 8);
	pixels = dma_zalloc(pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = pdat->width;
	render->height = pdat->height;
	render->pitch = (pdat->width * (pdat->bpp / 8) + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		sw_render_destroy_data(render);
		dma_free(render->pixels);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct pl110_fb_pdata_t * pdat = (struct pl110_fb_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		write32(pdat->virt + CLCD_UBAS, ((u32_t)render->pixels));
		write32(pdat->virt + CLCD_LBAS, ((u32_t)render->pixels + pdat->width * pdat->height * (pdat->bpp / 8)));
	}
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t pl110_register_framebuffer(struct resource_t * res)
{
	struct pl110_fb_data_t * rdat = (struct pl110_fb_data_t *)res->data;
	struct pl110_fb_pdata_t * pdat;
	struct fb_t * fb;
	char name[64];

	pdat = malloc(sizeof(struct pl110_fb_pdata_t));
	if(!pdat)
		return FALSE;

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->width = rdat->width;
	pdat->height = rdat->height;
	pdat->xdpi = rdat->xdpi;
	pdat->ydpi = rdat->ydpi;
	pdat->bpp = rdat->bpp;
	pdat->h_fp = rdat->h_fp;
	pdat->h_bp = rdat->h_bp;
	pdat->h_sw = rdat->h_sw;
	pdat->v_fp = rdat->v_fp;
	pdat->v_bp = rdat->v_bp;
	pdat->v_sw = rdat->v_sw;
	pdat->backlight = search_led(rdat->backlight);
	pdat->virt = phys_to_virt(rdat->phys);

	fb->name = strdup(name);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->xdpi = pdat->xdpi;
	fb->ydpi = pdat->ydpi;
	fb->bpp = pdat->bpp;
	fb->init = fb_init,
	fb->exit = fb_exit,
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->suspend = fb_suspend,
	fb->resume = fb_resume,
	fb->priv = pdat;

	if(register_framebuffer(fb))
		return TRUE;

	free(fb->priv);
	free(fb->name);
	free(fb);
	return FALSE;
}

static bool_t pl110_unregister_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb = search_framebuffer(name);
	if(!fb)
		return FALSE;

	if(!unregister_framebuffer(fb))
		return FALSE;

	free(fb->priv);
	free(fb->name);
	free(fb);
	return TRUE;
}

static __init void pl110_fb_init(void)
{
	resource_for_each("pl110-fb", pl110_register_framebuffer);
}

static __exit void pl110_fb_exit(void)
{
	resource_for_each("pl110-fb", pl110_unregister_framebuffer);
}

postdevice_initcall(pl110_fb_init);
postdevice_exitcall(pl110_fb_exit);
