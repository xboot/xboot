/*
 * wboxtest/benchmark-graphic/arc.c
 */

#include <wboxtest.h>

struct wbt_arc_pdata_t
{
	struct window_t * w;

	ktime_t t1;
	ktime_t t2;
	int calls;
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

static void arc_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_arc_pdata_t * pdat = (struct wbt_arc_pdata_t *)data;
	struct surface_t * s = pdat->w->s;

	if(pdat)
	{
		srand(0);
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			int x = wboxtest_random_int(0, surface_get_width(s));
			int y = wboxtest_random_int(0, surface_get_height(s));
			int radius = wboxtest_random_int(0, min(surface_get_width(s), surface_get_height(s)));
			int a1 = wboxtest_random_int(0, 360);
			int a2 = wboxtest_random_int(0, 360);
			struct color_t c;
			color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
			int thickness = wboxtest_random_int(0, 50);
			surface_shape_arc(s, NULL, x, y, radius, a1, a2, thickness, &c);
			pdat->calls++;
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Counts: %g\r\n", (double)(pdat->calls * 1000.0) / ktime_ms_delta(pdat->t2, pdat->t1));
	}
}

static struct wboxtest_t wbt_arc = {
	.group	= "benchmark-graphic",
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
