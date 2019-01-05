/*
 * driver/input/key-gpio-matrix.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>
#include <input/input.h>
#include <input/keyboard.h>

#define MAX_ROWS	(16)
#define MAX_COLS	(16)

struct gpio_pin_t {
	int gpio;
	int gpiocfg;
};

struct gpio_matrix_keymap_t {
	int row;
	int col;
	int keycode;
};

struct key_gpio_matrix_pdata_t {
	struct timer_t timer;
	struct gpio_pin_t * rows;
	int nrows;
	struct gpio_pin_t * cols;
	int ncols;
	struct gpio_matrix_keymap_t * maps;
	int nmaps;
	int active_low;
	int drive_inactive_cols;
	int col_scan_delay_us;
	int interval;
	uint32_t last_state[MAX_COLS];
};

static inline void activate_col(struct key_gpio_matrix_pdata_t * pdat, int col, int on)
{
	int level = !pdat->active_low;

	if(on)
	{
		gpio_direction_output(pdat->cols[col].gpio, level);
		if(pdat->col_scan_delay_us)
			udelay(pdat->col_scan_delay_us);
	}
	else
	{
		gpio_set_value(pdat->cols[col].gpio, !level);
		if(!pdat->drive_inactive_cols)
			gpio_direction_input(pdat->cols[col].gpio);
	}
}

static inline void activate_all_cols(struct key_gpio_matrix_pdata_t * pdat, int on)
{
	int level = !pdat->active_low;
	int col;

	if(on)
	{
		for(col = 0; col < pdat->ncols; col++)
		{
			gpio_direction_output(pdat->cols[col].gpio, level);
		}
	}
	else
	{
		for(col = 0; col < pdat->ncols; col++)
		{
			gpio_set_value(pdat->cols[col].gpio, !level);
			if(!pdat->drive_inactive_cols)
				gpio_direction_input(pdat->cols[col].gpio);
		}
	}
}

static inline int row_asserted(struct key_gpio_matrix_pdata_t * pdat, int row)
{
	return gpio_get_value(pdat->rows[row].gpio) ? !pdat->active_low : pdat->active_low;
}

static inline int key_gpio_matrix_get_keycode(struct key_gpio_matrix_pdata_t * pdat, int row, int col)
{
	struct gpio_matrix_keymap_t * map = NULL;
	int i;

	for(i = 0; i < pdat->nmaps; i++)
	{
		map = &pdat->maps[i];
		if((map->row == row) && (map->col == col))
			return map->keycode;
	}
	return ((col & 0xffff) << 16) | ((row & 0xffff) << 0);
}

static int key_gpio_matrix_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_gpio_matrix_pdata_t * pdat = (struct key_gpio_matrix_pdata_t *)input->priv;
	uint32_t state[MAX_COLS];
	uint32_t changed;
	int row, col, keycode;

	memset(state, 0, sizeof(uint32_t) * pdat->ncols);
	activate_all_cols(pdat, 0);

	for(col = 0; col < pdat->ncols; col++)
	{
		activate_col(pdat, col, 1);
		for(row = 0; row < pdat->nrows; row++)
			state[col] |= row_asserted(pdat, row) ? (1 << row) : 0;
		activate_col(pdat, col, 0);
	}

	for(col = 0; col < pdat->ncols; col++)
	{
		changed = pdat->last_state[col] ^ state[col];
		if(changed == 0)
			continue;

		for(row = 0; row < pdat->nrows; row++)
		{
			if((changed & (1 << row)) == 0)
				continue;

			keycode = key_gpio_matrix_get_keycode(pdat, row, col);
			if(state[col] & (1 << row))
				push_event_key_down(input, keycode);
			else
				push_event_key_up(input, keycode);
		}
	}
	memcpy(pdat->last_state, state, sizeof(uint32_t) * pdat->ncols);
	activate_all_cols(pdat, 1);

	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static int key_gpio_matrix_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_gpio_matrix_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_gpio_matrix_pdata_t * pdat;
	struct gpio_pin_t * rows;
	struct gpio_pin_t * cols;
	struct gpio_matrix_keymap_t * maps;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	int active_low = dt_read_bool(n, "gpio-active-low", 1);
	int nrows, ncols, nmaps;
	int i;

	if((nrows = dt_read_array_length(n, "rows")) <= 0)
		return NULL;

	if((ncols = dt_read_array_length(n, "cols")) <= 0)
		return NULL;

	if((nmaps = dt_read_array_length(n, "keymaps")) <= 0)
		return NULL;

	if((nrows > MAX_ROWS) || (ncols > MAX_COLS))
		return NULL;

	pdat = malloc(sizeof(struct key_gpio_matrix_pdata_t));
	if(!pdat)
		return NULL;

	rows = malloc(sizeof(struct gpio_pin_t) * nrows);
	if(!rows)
	{
		free(pdat);
		return NULL;
	}

	cols = malloc(sizeof(struct gpio_pin_t) * ncols);
	if(!cols)
	{
		free(rows);
		free(pdat);
		return NULL;
	}

	maps = malloc(sizeof(struct gpio_matrix_keymap_t) * nmaps);
	if(!maps)
	{
		free(rows);
		free(cols);
		free(pdat);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(rows);
		free(cols);
		free(maps);
		free(pdat);
		return NULL;
	}

	for(i = 0; i < ncols; i++)
	{
		dt_read_array_object(n, "cols", i, &o);
		cols[i].gpio = dt_read_int(&o, "gpio", -1);
		cols[i].gpiocfg = dt_read_int(&o, "gpio-config", -1);

		if(cols[i].gpiocfg >= 0)
			gpio_set_cfg(cols[i].gpio, cols[i].gpiocfg);
		gpio_set_pull(cols[i].gpio, GPIO_PULL_NONE);
		gpio_direction_output(cols[i].gpio, !active_low);
	}

	for(i = 0; i < nrows; i++)
	{
		dt_read_array_object(n, "rows", i, &o);
		rows[i].gpio = dt_read_int(&o, "gpio", -1);
		rows[i].gpiocfg = dt_read_int(&o, "gpio-config", -1);

		if(rows[i].gpiocfg >= 0)
			gpio_set_cfg(rows[i].gpio, rows[i].gpiocfg);
		gpio_set_pull(rows[i].gpio, GPIO_PULL_NONE);
		gpio_direction_input(rows[i].gpio);
	}

	for(i = 0; i < nmaps; i++)
	{
		dt_read_array_object(n, "keymaps", i, &o);
		maps[i].row = dt_read_int(&o, "row", -1);
		maps[i].col = dt_read_int(&o, "col", -1);
		maps[i].keycode = dt_read_int(&o, "key-code", 0);
	}

	timer_init(&pdat->timer, key_gpio_matrix_timer_function, input);
	pdat->rows = rows;
	pdat->nrows = nrows;
	pdat->cols = cols;
	pdat->ncols = ncols;
	pdat->maps = maps;
	pdat->nmaps = nmaps;
	pdat->active_low = active_low;
	pdat->drive_inactive_cols = dt_read_bool(n, "drive-inactive-cols", 1);
	pdat->col_scan_delay_us = dt_read_int(n, "col-scan-delay-us", 0);
	pdat->interval = dt_read_int(n, "poll-interval-ms", 100);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = key_gpio_matrix_ioctl;
	input->priv = pdat;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));

	if(!register_input(&dev, input))
	{
		timer_cancel(&pdat->timer);
		free(pdat->rows);
		free(pdat->cols);
		free(pdat->maps);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void key_gpio_matrix_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_matrix_pdata_t * pdat = (struct key_gpio_matrix_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		timer_cancel(&pdat->timer);
		free(pdat->rows);
		free(pdat->cols);
		free(pdat->maps);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_gpio_matrix_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_matrix_pdata_t * pdat = (struct key_gpio_matrix_pdata_t *)input->priv;

	timer_cancel(&pdat->timer);
}

static void key_gpio_matrix_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_matrix_pdata_t * pdat = (struct key_gpio_matrix_pdata_t *)input->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
}

static struct driver_t key_gpio_matrix = {
	.name		= "key-gpio-matrix",
	.probe		= key_gpio_matrix_probe,
	.remove		= key_gpio_matrix_remove,
	.suspend	= key_gpio_matrix_suspend,
	.resume		= key_gpio_matrix_resume,
};

static __init void key_gpio_matrix_driver_init(void)
{
	register_driver(&key_gpio_matrix);
}

static __exit void key_gpio_matrix_driver_exit(void)
{
	unregister_driver(&key_gpio_matrix);
}

driver_initcall(key_gpio_matrix_driver_init);
driver_exitcall(key_gpio_matrix_driver_exit);
