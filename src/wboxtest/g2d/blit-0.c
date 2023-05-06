/*
 * wboxtest/g2d/bilt-0.c
 */

#include <wboxtest.h>

struct wbt_blit_pdata_t
{
	struct window_t * w;
	struct g2d_t * g2d;
	struct surface_t * logo;
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

	pdat->g2d = search_first_g2d();
	if(!pdat->g2d)
	{
		window_free(pdat->w);
		free(pdat);
	}

	pdat->logo = surface_logo();
	if(!pdat->logo)
	{
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}
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
	int tx, ty;

	tx = wboxtest_random_int(0, surface_get_width(s));
	ty = wboxtest_random_int(0, surface_get_height(s));
	matrix_init_translate(&m, tx, ty);

	assert_true(g2d_blit(pdat->g2d, s, NULL, &m, logo));
}

static void blit_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_blit_pdata_t * pdat = (struct wbt_blit_pdata_t *)data;

	if(pdat)
		window_present(pdat->w, pdat, draw_blit);
}

static struct wboxtest_t wbt_blit = {
	.group	= "g2d",
	.name	= "blit-0",
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
