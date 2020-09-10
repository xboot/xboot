/*
 * wboxtest/graphic/noise.c
 */

#include <wboxtest.h>

struct wbt_noise_pdata_t
{
	struct window_t * w;
};

static void * noise_setup(struct wboxtest_t * wbt)
{
	struct wbt_noise_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_noise_pdata_t));
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

static void noise_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_noise_pdata_t * pdat = (struct wbt_noise_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_gray_noise(struct window_t * w, void * o)
{
	struct wbt_noise_pdata_t * pdat = (struct wbt_noise_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int x, y;
	unsigned char v;

	for(y = 0; y < surface_get_height(s); y++)
	{
		for(x = 0; x < surface_get_width(s); x++)
		{
			v = rand() & 0xff;
			color_init(&c, v, v, v, 255);
			surface_set_pixel(w->s, x, y, &c);
		}
	}
}

static void draw_color_noise(struct window_t * w, void * o)
{
	struct wbt_noise_pdata_t * pdat = (struct wbt_noise_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t c;
	int x, y;

	for(y = 0; y < surface_get_height(s); y++)
	{
		for(x = 0; x < surface_get_width(s); x++)
		{
			color_init(&c, rand() & 0xff, rand() & 0xff, rand() & 0xff, 255);
			surface_set_pixel(w->s, x, y, &c);
		}
	}
}

static void noise_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_noise_pdata_t * pdat = (struct wbt_noise_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		frame = 600;
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_gray_noise);
			while(ktime_before(ktime_get(), timeout));
		}

		frame = 600;
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_color_noise);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_noise = {
	.group	= "graphic",
	.name	= "noise",
	.setup	= noise_setup,
	.clean	= noise_clean,
	.run	= noise_run,
};

static __init void noise_wbt_init(void)
{
	register_wboxtest(&wbt_noise);
}

static __exit void noise_wbt_exit(void)
{
	unregister_wboxtest(&wbt_noise);
}

wboxtest_initcall(noise_wbt_init);
wboxtest_exitcall(noise_wbt_exit);
