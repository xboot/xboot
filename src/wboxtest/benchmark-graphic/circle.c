/*
 * wboxtest/benchmark-graphic/circle.c
 */

#include <wboxtest.h>

struct wbt_circle_pdata_t
{
	struct window_t * w;

	ktime_t t1;
	ktime_t t2;
	int calls;
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

static void circle_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_circle_pdata_t * pdat = (struct wbt_circle_pdata_t *)data;
	struct surface_t * s = pdat->w->s;

	if(pdat)
	{
		srand(0);
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			int tx = wboxtest_random_int(0, surface_get_width(s));
			int ty = wboxtest_random_int(0, surface_get_height(s));
			int radius = wboxtest_random_int(0, min(surface_get_width(s), surface_get_height(s)));
			struct color_t c;
			color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
			int thickness = wboxtest_random_int(0, 50);
			surface_shape_save(s);
			surface_shape_circle(s, tx, ty, radius);
			surface_shape_set_source_color(s, &c);
			if(thickness > 0)
			{
				surface_shape_set_line_width(s, thickness);
				surface_shape_stroke(s);
			}
			else
			{
				surface_shape_fill(s);
			}
			surface_shape_restore(s);
			pdat->calls++;
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Counts: %g\r\n", (double)(pdat->calls * 1000.0) / ktime_ms_delta(pdat->t2, pdat->t1));
	}
}

static struct wboxtest_t wbt_circle = {
	.group	= "benchmark-graphic",
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
