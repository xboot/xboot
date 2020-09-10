/*
 * wboxtest/graphic/line.c
 */

#include <wboxtest.h>

struct wbt_line_pdata_t
{
	struct window_t * w;
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

static void draw_line(struct window_t * w, void * o)
{
	struct wbt_line_pdata_t * pdat = (struct wbt_line_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct point_t p0, p1;
	struct color_t c;
	int thickness;

	p0.x = wboxtest_random_int(0, surface_get_width(s));
	p0.y = wboxtest_random_int(0, surface_get_height(s));
	p1.x = wboxtest_random_int(0, surface_get_width(s));
	p1.y = wboxtest_random_int(0, surface_get_height(s));
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_line(s, NULL, &p0, &p1, thickness, &c);
}

static void line_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_line_pdata_t * pdat = (struct wbt_line_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_line);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_line = {
	.group	= "graphic",
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
