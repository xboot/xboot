/*
 * wboxtest/graphic/plasma.c
 */

#include <wboxtest.h>

struct wbt_plasma_pdata_t
{
	struct window_t * w;
	struct color_t palette[256];
	float times;
};

static void * plasma_setup(struct wboxtest_t * wbt)
{
	struct wbt_plasma_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_plasma_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}
	for(int i = 0; i < 256; i++)
		color_set_hsva(&pdat->palette[i], i / 256.0, 1.0, 1.0, 1.0);
	pdat->times = 0;

	return pdat;
}

static void plasma_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_plasma_pdata_t * pdat = (struct wbt_plasma_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

#define DIST(a, b, c, d)	sqrtf((float)(((a) - (c)) * ((a) - (c)) + ((b) - (d)) * ((b) - (d))))

static void draw_plasma(struct window_t * w, void * o)
{
	struct wbt_plasma_pdata_t * pdat = (struct wbt_plasma_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct color_t * c;
	int x, y;

	pdat->times += 1.0;
	for(y = 0; y < surface_get_height(s); y++)
	{
		for(x = 0; x < surface_get_width(s); x++)
		{
			float value = sinf(DIST(x + pdat->times, y, 128.0, 128.0) / 8.0)
				+ sinf(DIST(x, y, 64.0, 64.0) / 8.0)
				+ sinf(DIST(x, y + pdat->times / 7, 192.0, 64) / 7.0)
				+ sinf(DIST(x, y, 192.0, 100.0) / 8.0);
			c = &pdat->palette[(unsigned int)((value + 4) * 32) & 0xff];
			surface_set_pixel(w->s, x, y, c);
		}
	}
}

static void plasma_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_plasma_pdata_t * pdat = (struct wbt_plasma_pdata_t *)data;
	int frame = 600;

	if(pdat)
	{
		frame = 600;
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_plasma);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_plasma = {
	.group	= "graphic",
	.name	= "plasma",
	.setup	= plasma_setup,
	.clean	= plasma_clean,
	.run	= plasma_run,
};

static __init void plasma_wbt_init(void)
{
	register_wboxtest(&wbt_plasma);
}

static __exit void plasma_wbt_exit(void)
{
	unregister_wboxtest(&wbt_plasma);
}

wboxtest_initcall(plasma_wbt_init);
wboxtest_exitcall(plasma_wbt_exit);
