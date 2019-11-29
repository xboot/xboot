/*
 * wboxtest/graphic/rectangle.c
 */

#include <wboxtest.h>

struct wbt_rectangle_pdata_t
{
	struct window_t * w;
	struct color_t c;
};

static void * rectangle_setup(struct wboxtest_t * wbt)
{
	struct wbt_rectangle_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_rectangle_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL, pdat);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}
	color_init(&pdat->c, 255, 255, 255, 255);

	return pdat;
}

static void rectangle_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_rectangle_pdata_t * pdat = (struct wbt_rectangle_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_rectangle(struct window_t * w, void * o)
{
	struct wbt_rectangle_pdata_t * pdat = (struct wbt_rectangle_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int tx, ty, tw, th, radius;
	int thickness;

	tx = wboxtest_random_int(0, surface_get_width(s));
	ty = wboxtest_random_int(0, surface_get_height(s));
	tw = wboxtest_random_int(0, surface_get_width(s));
	th = wboxtest_random_int(0, surface_get_height(s));
	radius = wboxtest_random_int(0, 20);
	color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
	thickness = wboxtest_random_int(0, 50);

	surface_shape_rectangle(s, NULL, tx, ty, tw, th, radius, thickness, &c);
}

static void rectangle_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_rectangle_pdata_t * pdat = (struct wbt_rectangle_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, &pdat->c, pdat, draw_rectangle);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_rectangle = {
	.group	= "graphic",
	.name	= "rectangle",
	.setup	= rectangle_setup,
	.clean	= rectangle_clean,
	.run	= rectangle_run,
};

static __init void rectangle_wbt_init(void)
{
	register_wboxtest(&wbt_rectangle);
}

static __exit void rectangle_wbt_exit(void)
{
	unregister_wboxtest(&wbt_rectangle);
}

wboxtest_initcall(rectangle_wbt_init);
wboxtest_exitcall(rectangle_wbt_exit);
