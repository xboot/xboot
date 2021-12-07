/*
 * wboxtest/graphic/curve.c
 */

#include <wboxtest.h>

struct wbt_curve_pdata_t
{
	struct window_t * w;
};

static void * curve_setup(struct wboxtest_t * wbt)
{
	struct wbt_curve_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_curve_pdata_t));
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

static void curve_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_curve_pdata_t * pdat = (struct wbt_curve_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_curve(struct window_t * w, void * o)
{
	struct wbt_curve_pdata_t * pdat = (struct wbt_curve_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct point_t p[10];
	struct color_t c;
	int thickness;

	int n = wboxtest_random_int(4, 10);
	for(int i = 0; i < n; i++)
	{
		p[i].x = wboxtest_random_int(0, surface_get_width(s));
		p[i].y = wboxtest_random_int(0, surface_get_height(s));
	}
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);
	surface_shape_curve(s, NULL, &p[0], n, thickness, &c);
}

static void curve_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_curve_pdata_t * pdat = (struct wbt_curve_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_curve);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_curve = {
	.group	= "graphic",
	.name	= "curve",
	.setup	= curve_setup,
	.clean	= curve_clean,
	.run	= curve_run,
};

static __init void curve_wbt_init(void)
{
	register_wboxtest(&wbt_curve);
}

static __exit void curve_wbt_exit(void)
{
	unregister_wboxtest(&wbt_curve);
}

wboxtest_initcall(curve_wbt_init);
wboxtest_exitcall(curve_wbt_exit);
