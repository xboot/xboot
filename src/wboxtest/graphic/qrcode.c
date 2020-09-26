/*
 * wboxtest/graphic/qrcode.c
 */

#include <qrcgen.h>
#include <wboxtest.h>

struct wbt_qrcode_pdata_t
{
	struct window_t * w;
};

static void * qrcode_setup(struct wboxtest_t * wbt)
{
	struct wbt_qrcode_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_qrcode_pdata_t));
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

static void qrcode_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)data;

	if(pdat)
	{
		window_free(pdat->w);
		free(pdat);
	}
}

static void draw_gray_qrcode(struct window_t * w, void * o)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)o;
	struct surface_t * s = pdat->w->s;
	struct surface_t * qr;
	struct matrix_t m;
	char txt[128];

	sprintf(txt, "xboot qrcode(%d)", rand());
	qr = surface_alloc_qrcode(txt, 16);
	if(qr)
	{
		matrix_init_identity(&m);
		matrix_init_translate(&m, (surface_get_width(s) - surface_get_width(qr)) / 2, (surface_get_height(s) - surface_get_height(qr)) / 2);
		surface_blit(s, NULL, &m, qr, RENDER_TYPE_GOOD);
		surface_free(qr);
	}
}

static void qrcode_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_qrcode_pdata_t * pdat = (struct wbt_qrcode_pdata_t *)data;
	int frame = 3600;

	if(pdat)
	{
		while(frame-- > 0)
		{
			ktime_t timeout = ktime_add_ms(ktime_get(), 16);
			window_present(pdat->w, pdat, draw_gray_qrcode);
			while(ktime_before(ktime_get(), timeout));
		}
	}
}

static struct wboxtest_t wbt_qrcode = {
	.group	= "graphic",
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
