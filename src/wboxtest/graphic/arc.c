/*
 * wboxtest/graphic/arc.c
 */

#include <wboxtest.h>

struct wbt_arc_pdata_t
{
	struct window_t * w;
	struct color_t c;
};

static void * arc_setup(struct wboxtest_t * wbt)
{
	struct wbt_arc_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_arc_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}
	color_init(&pdat->c, 255, 255, 255, 255);

	return pdat;
}

static void arc_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_arc_pdata_t * pdat = (struct wbt_arc_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_arc(struct window_t * w, void * o)
{
	struct wbt_arc_pdata_t * pdat = (struct wbt_arc_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int x, y, radius, a1, a2;
	int thickness;

	x = wboxtest_random_int(0, surface_get_width(s));
	y = wboxtest_random_int(0, surface_get_height(s));
	radius = wboxtest_random_int(0, min(surface_get_width(s), surface_get_height(s)));
	a1 = wboxtest_random_int(0, 360);
	a2 = wboxtest_random_int(0, 360);
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_arc(s, NULL, x, y, radius, a1, a2, thickness, &c);
}

static void arc_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_arc_pdata_t * pdat = (struct wbt_arc_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_arc);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_arc = {
	.group	= "graphic",
	.name	= "arc",
	.setup	= arc_setup,
	.clean	= arc_clean,
	.run	= arc_run,
};

static __init void arc_wbt_init(void)
{
	register_wboxtest(&wbt_arc);
}

static __exit void arc_wbt_exit(void)
{
	unregister_wboxtest(&wbt_arc);
}

wboxtest_initcall(arc_wbt_init);
wboxtest_exitcall(arc_wbt_exit);
