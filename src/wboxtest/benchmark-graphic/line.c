/*
 * wboxtest/benchmark-graphic/line.c
 */

#include <wboxtest.h>

struct wbt_line_pdata_t
{
	struct window_t * w;

	ktime_t t1;
	ktime_t t2;
	int calls;
};

static void * line_setup(struct wboxtest_t * wbt)
{
	struct wbt_line_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_line_pdata_t));
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

static void line_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_line_pdata_t * pdat = (struct wbt_line_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void line_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_line_pdata_t * pdat = (struct wbt_line_pdata_t *)data;
	struct surface_t * s = pdat->w->s;

	if(pdat)
	{
		srand(0);
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			struct point_t p0, p1;
			p0.x = wboxtest_random_int(0, surface_get_width(s));
			p0.y = wboxtest_random_int(0, surface_get_height(s));
			p1.x = wboxtest_random_int(0, surface_get_width(s));
			p1.y = wboxtest_random_int(0, surface_get_height(s));
			struct color_t c;
			color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
			int thickness = wboxtest_random_int(0, 50);
			surface_shape_save(s);
			surface_shape_move_to(s, p0.x, p0.y);
			surface_shape_line_to(s, p1.x, p1.y);
			surface_shape_set_source_color(s, &c);
			surface_shape_set_line_width(s, thickness > 0 ? thickness : 1);
			surface_shape_stroke(s);
			surface_shape_restore(s);
			pdat->calls++;
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Counts: %g\r\n", (double)(pdat->calls * 1000.0) / ktime_ms_delta(pdat->t2, pdat->t1));
	}
}

static struct wboxtest_t wbt_line = {
	.group	= "benchmark-graphic",
	.name	= "line",
	.setup	= line_setup,
	.clean	= line_clean,
	.run	= line_run,
};

static __init void line_wbt_init(void)
{
	register_wboxtest(&wbt_line);
}

static __exit void line_wbt_exit(void)
{
	unregister_wboxtest(&wbt_line);
}

wboxtest_initcall(line_wbt_init);
wboxtest_exitcall(line_wbt_exit);
