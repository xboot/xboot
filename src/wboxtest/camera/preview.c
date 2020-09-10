/*
 * wboxtest/camera/preview.c
 */

#include <camera/camera.h>
#include <wboxtest.h>

struct wbt_preview_pdata_t
{
	struct window_t * w;
	struct camera_t * c;
	struct surface_t * s;
	struct video_frame_t frame;
};

static void * preview_setup(struct wboxtest_t * wbt)
{
	struct wbt_preview_pdata_t * pdat;
	const char * name = NULL;

	pdat = malloc(sizeof(struct wbt_preview_pdata_t));
	if(!pdat)
		return NULL;

	pdat->w = window_alloc(NULL, NULL);
	if(!pdat->w)
	{
		free(pdat);
		return NULL;
	}

	pdat->c = name ? search_camera(name) : search_first_camera();
	if(!pdat->c)
	{
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	if(!camera_start(pdat->c, VIDEO_FORMAT_MJPG, 320, 240))
	{
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	if(!camera_capture(pdat->c, &pdat->frame, 3000))
	{
		camera_stop(pdat->c);
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	pdat->s = surface_alloc(pdat->frame.width, pdat->frame.height, NULL);
	if(!pdat->s)
	{
		camera_stop(pdat->c);
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	return pdat;
}

static void preview_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_preview_pdata_t * pdat = (struct wbt_preview_pdata_t *)data;

	if(pdat)
	{
		camera_stop(pdat->c);
		surface_free(pdat->s);
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_preview(struct window_t * w, void * o)
{
	struct wbt_preview_pdata_t * pdat = (struct wbt_preview_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct matrix_t m;

	if(camera_capture(pdat->c, &pdat->frame, 0))
		video_frame_to_argb(&pdat->frame, pdat->s->pixels);
	matrix_init_identity(&m);
	matrix_init_translate(&m, (surface_get_width(s) - surface_get_width(pdat->s)) / 2, (surface_get_height(s) - surface_get_height(pdat->s)) / 2);
	surface_blit(s, NULL, &m, pdat->s, RENDER_TYPE_GOOD);
}

static void preview_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_preview_pdata_t * pdat = (struct wbt_preview_pdata_t *)data;
	int frame = 1200;

	assert_not_null(pdat);
	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_preview);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_preview = {
	.group	= "camera",
	.name	= "preview",
	.setup	= preview_setup,
	.clean	= preview_clean,
	.run	= preview_run,
};

static __init void preview_wbt_init(void)
{
	register_wboxtest(&wbt_preview);
}

static __exit void preview_wbt_exit(void)
{
	unregister_wboxtest(&wbt_preview);
}

wboxtest_initcall(preview_wbt_init);
wboxtest_exitcall(preview_wbt_exit);
