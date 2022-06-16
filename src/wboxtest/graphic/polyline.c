/*
 * wboxtest/graphic/polyline.c
 */

#include <wboxtest.h>

struct wbt_polyline_pdata_t
{
	struct window_t * w;
};

static void * polyline_setup(struct wboxtest_t * wbt)
{
	struct wbt_polyline_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_polyline_pdata_t));
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

static void polyline_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_polyline_pdata_t * pdat = (struct wbt_polyline_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_polyline(struct window_t * w, void * o)
{
	struct wbt_polyline_pdata_t * pdat = (struct wbt_polyline_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct point_t p[10];
	struct color_t c;
	int thickness;

	int n = wboxtest_random_int(2, 10);
	for(int i = 0; i < n; i++)
	{
		p[i].x = wboxtest_random_int(0, surface_get_width(s));
		p[i].y = wboxtest_random_int(0, surface_get_height(s));
	}
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_save(s);
	surface_shape_move_to(s, p[0].x, p[0].y);
	for(int i = 1; i < n; i++)
		surface_shape_line_to(s, p[i].x, p[i].y);
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
}

static void polyline_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_polyline_pdata_t * pdat = (struct wbt_polyline_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_polyline);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_polyline = {
	.group	= "graphic",
	.name	= "polyline",
	.setup	= polyline_setup,
	.clean	= polyline_clean,
	.run	= polyline_run,
};

static __init void polyline_wbt_init(void)
{
	register_wboxtest(&wbt_polyline);
}

static __exit void polyline_wbt_exit(void)
{
	unregister_wboxtest(&wbt_polyline);
}

wboxtest_initcall(polyline_wbt_init);
wboxtest_exitcall(polyline_wbt_exit);
