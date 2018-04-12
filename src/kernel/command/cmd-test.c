/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>
#include <cairo-xboot.h>
#include <cairo-ft.h>
#include <shell/ctrlc.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <lvgl/lvgl.h>

struct lvgui_pdata_t {
	lv_disp_drv_t disp;
	lv_indev_drv_t indev;
	struct framebuffer_t * fb;
	struct timer_t timer;
};
struct lvgui_pdata_t lvgui;

static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * c)
{
	lv_color_t * pixels = lvgui.fb->alone->pixels;
    int32_t y, w = x2 - x1 + 1;

	for(y = y1; y <= y2; y++)
	{
		memcpy(&pixels[y * LV_HOR_RES + x1], c, w * sizeof(lv_color_t));
		c += w;
	}
	lvgui.fb->present(lvgui.fb, lvgui.fb->alone);
	lv_flush_ready();
}

#if USE_LV_GPU
static void ex_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
	int32_t i;

	for(i = 0; i < length; i++)
	{
		dest[i] = lv_color_mix(dest[i], src[i], opa);
	}
}

static void ex_mem_fill(lv_color_t * dest, uint32_t length, lv_color_t color)
{
	int32_t i;

	for(i = 0; i < length; i++)
	{
		dest[i] = color;
	}
}
#endif

static bool ex_tp_read(lv_indev_data_t *data)
{
	static int down = 0;
	struct event_t e;

	if(pump_event(runtime_get()->__event_base, &e))
	{
		switch(e.type)
		{
		case EVENT_TYPE_MOUSE_UP:
			data->point.x = e.e.mouse_up.x;
			data->point.y = e.e.mouse_up.y;
			data->state = LV_INDEV_STATE_REL;
			down = 0;
			return false;

		case EVENT_TYPE_MOUSE_DOWN:
			data->point.x = e.e.mouse_down.x;
			data->point.y = e.e.mouse_down.y;
			data->state = LV_INDEV_STATE_PR;
			down = 1;
			return false;

		case EVENT_TYPE_MOUSE_MOVE:
			if(down)
			{
				data->point.x = e.e.mouse_move.x;
				data->point.y = e.e.mouse_move.y;
				data->state = LV_INDEV_STATE_PR;
				return true;
			}
			return false;

		case EVENT_TYPE_TOUCH_BEGIN:
			data->point.x = e.e.touch_begin.x;
			data->point.y = e.e.touch_begin.y;
			data->state = LV_INDEV_STATE_PR;
			return false;

		case EVENT_TYPE_TOUCH_MOVE:
			data->point.x = e.e.touch_move.x;
			data->point.y = e.e.touch_move.y;
			//data->state = LV_INDEV_STATE_PR;
			return true;

		case EVENT_TYPE_TOUCH_END:
			data->point.x = e.e.touch_end.x;
			data->point.y = e.e.touch_end.y;
			data->state = LV_INDEV_STATE_REL;
			return false;

		default:
			break;
		}
	}
	return false;
}

static int lvgui_timer_function(struct timer_t * timer, void * data)
{
	timer_forward_now(timer, ms_to_ktime(1));
	lv_tick_inc(1);
	return 1;
}

void do_init_lvgui(void)
{
	struct lvgui_pdata_t * pdat = &lvgui;

	lv_init();

	lv_disp_drv_init(&pdat->disp);
	pdat->disp.disp_flush = ex_disp_flush;
	pdat->disp.disp_fill = NULL;
	pdat->disp.disp_map = NULL;
#if USE_LV_GPU
	pdat->disp.mem_blend = ex_mem_blend;
	pdat->disp.mem_fill = ex_mem_fill;
#endif
    lv_disp_drv_register(&pdat->disp);

    lv_indev_drv_init(&pdat->indev);
    pdat->indev.type = LV_INDEV_TYPE_POINTER;
    pdat->indev.read = ex_tp_read;
    lv_indev_drv_register(&pdat->indev);

    pdat->fb = search_first_framebuffer();
	timer_init(&pdat->timer, lvgui_timer_function, pdat);
	timer_start_now(&pdat->timer, ms_to_ktime(1));
}

void do_exit_lvgui(void)
{
	struct lvgui_pdata_t * pdat = &lvgui;
	timer_cancel(&pdat->timer);
}

void lv_tutorial_hello_world(void)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static int do_test(int argc, char ** argv)
{
	do_init_lvgui();
	lv_tutorial_hello_world();

	while(1)
	{
		lv_task_handler();
		mdelay(1);
		if(ctrlc())
			break;
	}
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
