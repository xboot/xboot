/*
 * wboxtest/graphic/circle.c
 */

#include <wboxtest.h>

struct wbt_circle_pdata_t
{
	struct window_t * w;
};

static void * circle_setup(struct wboxtest_t * wbt)
{
	struct wbt_circle_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_circle_pdata_t));
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

static void circle_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_circle_pdata_t * pdat = (struct wbt_circle_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_circle(struct window_t * w, void * o)
{
	struct wbt_circle_pdata_t * pdat = (struct wbt_circle_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int tx, ty, radius;
	int thickness;

	tx = wboxtest_random_int(0, surface_get_width(s));
	ty = wboxtest_random_int(0, surface_get_height(s));
	radius = wboxtest_random_int(0, min(surface_get_width(s), surface_get_height(s)));
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_circle(s, NULL, tx, ty, radius, thickness, &c);
}

static void circle_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_circle_pdata_t * pdat = (struct wbt_circle_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_circle);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_circle = {
	.group	= "graphic",
	.name	= "circle",
	.setup	= circle_setup,
	.clean	= circle_clean,
	.run	= circle_run,
};

static __init void circle_wbt_init(void)
{
	register_wboxtest(&wbt_circle);
}

static __exit void circle_wbt_exit(void)
{
	unregister_wboxtest(&wbt_circle);
}

wboxtest_initcall(circle_wbt_init);
wboxtest_exitcall(circle_wbt_exit);
