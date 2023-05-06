/*
 * wboxtest/graphic/blit.c
 */

#include <wboxtest.h>

struct wbt_blit_pdata_t
{
	struct window_t * w;
	struct surface_t * logo;
	float minx, miny;
	float maxx, maxy;
	float x, y;
	float vx, vy;
};

static inline struct surface_t * surface_logo(void)
{
	struct xfs_context_t * ctx = xfs_alloc("/private/framework", 0);
	struct surface_t * s = surface_alloc_from_xfs(ctx, "assets/images/logo.png");
	xfs_free(ctx);
	return s;
}

static void * blit_setup(struct wboxtest_t * wbt)
{
	struct wbt_blit_pdata_t * pdat;
	pdat = malloc(sizeof(struct wbt_blit_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}

	pdat->logo = surface_logo();
	if(!pdat->logo)
	{
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	pdat->minx = 0;
	pdat->miny = 0;
	pdat->maxx = window_get_width(pdat->w) - surface_get_width(pdat->logo);
	pdat->maxy = window_get_height(pdat->w) - surface_get_height(pdat->logo);
	pdat->x = wboxtest_random_float(pdat->minx, pdat->maxx);
	pdat->y = wboxtest_random_float(pdat->miny, pdat->maxy);
	pdat->vx = wboxtest_random_float(-10, 10);
	pdat->vy = wboxtest_random_float(-10, 10);

	return pdat;
}

static void blit_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_blit_pdata_t * pdat = (struct wbt_blit_pdata_t *)data;

	if(pdat)
	{
		surface_free(pdat->logo);
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_blit(struct window_t * w, void * o)
{
	struct wbt_blit_pdata_t * pdat = (struct wbt_blit_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct surface_t * logo = pdat->logo;
	struct matrix_t m;

	pdat->x += pdat->vx;
	pdat->y += pdat->vy;
	if(pdat->x > pdat->maxx)
	{
		pdat->x = pdat->maxx;
		pdat->vx = -pdat->vx;
	}
	else if(pdat->y > pdat->maxy)
	{
		pdat->y = pdat->maxy;
		pdat->vy = -pdat->vy;
	}
	if(pdat->x < pdat->minx)
	{
		pdat->x = pdat->minx;
		pdat->vx = -pdat->vx;
	}
	else if(pdat->y < pdat->miny)
	{
		pdat->y = pdat->miny;
		pdat->vy = -pdat->vy;
	}
	matrix_init_translate(&m, pdat->x, pdat->y);
	surface_blit(s, NULL, &m, logo);
}

static void blit_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_blit_pdata_t * pdat = (struct wbt_blit_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_blit);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_blit = {
	.group	= "graphic",
	.name	= "blit",
	.setup	= blit_setup,
	.clean	= blit_clean,
	.run	= blit_run,
};

static __init void blit_wbt_init(void)
{
	register_wboxtest(&wbt_blit);
}

static __exit void blit_wbt_exit(void)
{
	unregister_wboxtest(&wbt_blit);
}

wboxtest_initcall(blit_wbt_init);
wboxtest_exitcall(blit_wbt_exit);
