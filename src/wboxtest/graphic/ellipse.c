/*
 * wboxtest/graphic/ellipse.c
 */

#include <wboxtest.h>

struct wbt_ellipse_pdata_t
{
	struct window_t * w;
};

static void * ellipse_setup(struct wboxtest_t * wbt)
{
	struct wbt_ellipse_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_ellipse_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}
	return pdat;
}

static void ellipse_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_ellipse_pdata_t * pdat = (struct wbt_ellipse_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_ellipse(struct window_t * w, void * o)
{
	struct wbt_ellipse_pdata_t * pdat = (struct wbt_ellipse_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int tx, ty, tw, th;
	int thickness;

	tx = wboxtest_random_int(0, surface_get_width(s));
	ty = wboxtest_random_int(0, surface_get_height(s));
	tw = wboxtest_random_int(0, surface_get_width(s));
	th = wboxtest_random_int(0, surface_get_height(s));
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_ellipse(s, NULL, tx, ty, tw, th, thickness, &c);
}

static void ellipse_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_ellipse_pdata_t * pdat = (struct wbt_ellipse_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_ellipse);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_ellipse = {
	.group	= "graphic",
	.name	= "ellipse",
	.setup	= ellipse_setup,
	.clean	= ellipse_clean,
	.run	= ellipse_run,
};

static __init void ellipse_wbt_init(void)
{
	register_wboxtest(&wbt_ellipse);
}

static __exit void ellipse_wbt_exit(void)
{
	unregister_wboxtest(&wbt_ellipse);
}

wboxtest_initcall(ellipse_wbt_init);
wboxtest_exitcall(ellipse_wbt_exit);
