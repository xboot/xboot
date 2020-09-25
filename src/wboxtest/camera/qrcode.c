/*
 * wboxtest/camera/qrcode.c
 */

#include <camera/camera.h>
#include <quirc.h>
#include <wboxtest.h>

struct wbt_qrcode_pdata_t
{
	struct window_t * w;
	struct camera_t * c;
	struct surface_t * s;
	struct video_frame_t frame;
	struct quirc * qr;
};

static void * qrcode_setup(struct wboxtest_t * wbt)
{
	struct wbt_qrcode_pdata_t * pdat;
	const char * name = NULL;

	pdat = malloc(sizeof(struct wbt_qrcode_pdata_t));
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

	if(!camera_start(pdat->c, VIDEO_FORMAT_MJPG, 640, 480))
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

	pdat->qr = quirc_new();
	if(!pdat->qr)
	{
		camera_stop(pdat->c);
		surface_free(pdat->s);
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	if(quirc_resize(pdat->qr, pdat->frame.width, pdat->frame.height) < 0)
	{
		quirc_destroy(pdat->qr);
		camera_stop(pdat->c);
		surface_free(pdat->s);
		window_free(pdat->w);
		free(pdat);
		return NULL;
	}

	return pdat;
}

static void qrcode_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)data;

	if(pdat)
	{
		quirc_destroy(pdat->qr);
		camera_stop(pdat->c);
		surface_free(pdat->s);
		window_free(pdat->w);
		free(pdat);
	}
}

static inline void surface_to_gray(struct surface_t * s, uint8_t * gray)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);

	for(i = 0; i < len; i++, p += 4)
		*(gray++) = (p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16;
}

static void draw_qrcode(struct window_t * w, void * o)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct matrix_t m;
	struct quirc_code code;
	struct quirc_data data;
	quirc_decode_error_t err;
	int i;

	if(camera_capture(pdat->c, &pdat->frame, 0))
	{
		video_frame_to_argb(&pdat->frame, pdat->s->pixels);
		surface_to_gray(pdat->s, quirc_begin(pdat->qr, NULL, NULL));
		quirc_end(pdat->qr);
		for(i = 0; i < quirc_count(pdat->qr); i++)
		{
			quirc_extract(pdat->qr, i, &code);
		    err = quirc_decode(&code, &data);
			if(err == QUIRC_ERROR_DATA_ECC)
			{
				quirc_flip(&code);
				err = quirc_decode(&code, &data);
			}
			if(err == QUIRC_SUCCESS)
				wboxtest_print(" %s\r\n", data.payload);
		}
	}
	matrix_init_identity(&m);
	matrix_init_translate(&m, (surface_get_width(s) - surface_get_width(pdat->s)) / 2, (surface_get_height(s) - surface_get_height(pdat->s)) / 2);
	surface_blit(s, NULL, &m, pdat->s, RENDER_TYPE_GOOD);
}

static void qrcode_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)data;
	int frame = 3600;

	assert_not_null(pdat);
	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_qrcode);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_qrcode = {
	.group	= "camera",
	.name	= "qrcode",
	.setup	= qrcode_setup,
	.clean	= qrcode_clean,
	.run	= qrcode_run,
};

static __init void qrcode_wbt_init(void)
{
	register_wboxtest(&wbt_qrcode);
}

static __exit void qrcode_wbt_exit(void)
{
	unregister_wboxtest(&wbt_qrcode);
}

wboxtest_initcall(qrcode_wbt_init);
wboxtest_exitcall(qrcode_wbt_exit);
