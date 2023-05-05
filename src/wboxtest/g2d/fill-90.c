/*
 * wboxtest/graphic/fill.c
 */

#include <g2d/g2d.h>
#include <wboxtest.h>

struct wbt_fill_pdata_t
{
	struct window_t * w;
	struct g2d_t * g2d;
};

static void * fill_setup(struct wboxtest_t * wbt)
{
	struct wbt_fill_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_fill_pdata_t));
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

	return pdat;
}

static void fill_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_fill_pdata_t * pdat = (struct wbt_fill_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_fill(struct window_t * w, void * o)
{
	struct wbt_fill_pdata_t * pdat = (struct wbt_fill_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct matrix_t m;
	struct color_t c;
	int tx, ty, tw, th;

	tx = wboxtest_random_int(0, surface_get_width(s));
	ty = wboxtest_random_int(0, surface_get_height(s));
	tw = wboxtest_random_int(0, surface_get_width(s));
	th = wboxtest_random_int(0, surface_get_height(s));
	matrix_init_translate(&m, tx, ty);
	matrix_rotate(&m, M_PI / 2);
	matrix_scale(&m, 1, 1);
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);

	assert_true(g2d_fill(pdat->g2d, s, NULL, &m, tw, th, &c));
}

static void fill_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_fill_pdata_t * pdat = (struct wbt_fill_pdata_t *)data;

	if(pdat)
		window_present(pdat->w, pdat, draw_fill);
}

static struct wboxtest_t wbt_fill = {
	.group	= "g2d",
	.name	= "fill-90",
	.setup	= fill_setup,
	.clean	= fill_clean,
	.run	= fill_run,
};

static __init void fill_wbt_init(void)
{
	register_wboxtest(&wbt_fill);
}

static __exit void fill_wbt_exit(void)
{
	unregister_wboxtest(&wbt_fill);
}

wboxtest_initcall(fill_wbt_init);
wboxtest_exitcall(fill_wbt_exit);
