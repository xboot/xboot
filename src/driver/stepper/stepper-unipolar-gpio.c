/*
 * driver/stepper/stepper-unipolar-gpio.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>
#include <stepper/stepper.h>

enum stepper_mode_t {
	STEPPER_MODE_WAVE		= 0,
	STEPPER_MODE_FULLSTEP	= 1,
	STEPPER_MODE_HALFSTEP	= 2,
};

struct stepper_unipolar_gpio_pdata_t {
	struct timer_t timer;
	enum stepper_mode_t mode;
	int c0;
	int c0cfg;
	int c1;
	int c1cfg;
	int c2;
	int c2cfg;
	int c3;
	int c3cfg;
	int index;
	int enable;
	int dir;
	int step;
	int speed;
	int busying;
};

static void stepper_wave(struct stepper_unipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 1:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 2:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 3:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 1);
		break;

	default:
		break;
	}
}

static void stepper_fullstep(struct stepper_unipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 1:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 2:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 1);
		break;

	case 3:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 1);
		break;

	default:
		break;
	}
}

static void stepper_halfstep(struct stepper_unipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 1:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 2:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 3:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 1);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 4:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 0);
		break;

	case 5:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 1);
		gpio_direction_output(pdat->c3, 1);
		break;

	case 6:
		gpio_direction_output(pdat->c0, 0);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 1);
		break;

	case 7:
		gpio_direction_output(pdat->c0, 1);
		gpio_direction_output(pdat->c1, 0);
		gpio_direction_output(pdat->c2, 0);
		gpio_direction_output(pdat->c3, 1);
		break;

	default:
		break;
	}
}

static void stepper_unipolar_gpio_enable(struct stepper_t * m)
{
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;
	switch(pdat->mode)
	{
	case STEPPER_MODE_WAVE:
		stepper_wave(pdat);
		break;
	case STEPPER_MODE_FULLSTEP:
		stepper_fullstep(pdat);
		break;
	case STEPPER_MODE_HALFSTEP:
		stepper_halfstep(pdat);
		break;
	default:
		break;
	}
	pdat->enable = 1;
}

static void stepper_unipolar_gpio_disable(struct stepper_t * m)
{
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;
	gpio_direction_output(pdat->c0, 0);
	gpio_direction_output(pdat->c1, 0);
	gpio_direction_output(pdat->c2, 0);
	gpio_direction_output(pdat->c3, 0);
	pdat->enable = 0;
}

static void stepper_unipolar_gpio_move(struct stepper_t * m, int step, int speed)
{
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;
	if(pdat->enable && !pdat->busying)
	{
		if(step < 0)
			pdat->dir = 0;
		else
			pdat->dir = 1;
		pdat->step = abs(step);
		pdat->speed = speed;
		pdat->busying = 1;
		timer_start_now(&pdat->timer, ns_to_ktime(1000000000ULL / pdat->speed));
	}
}

static int stepper_unipolar_gpio_busying(struct stepper_t * m)
{
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;
	return pdat->busying;
}

static int stepper_unipolar_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct stepper_t * m = (struct stepper_t *)(data);
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;

	if(pdat->enable && (pdat->step-- > 0))
	{
		switch(pdat->mode)
		{
		case STEPPER_MODE_WAVE:
			pdat->index = (pdat->index + 4 + (pdat->dir ? 1 : -1)) % 4;
			stepper_wave(pdat);
			break;
		case STEPPER_MODE_FULLSTEP:
			pdat->index = (pdat->index + 4 + (pdat->dir ? 1 : -1)) % 4;
			stepper_fullstep(pdat);
			break;
		case STEPPER_MODE_HALFSTEP:
			pdat->index = (pdat->index + 8 + (pdat->dir ? 1 : -1)) % 8;
			stepper_halfstep(pdat);
			break;
		default:
			break;
		}
		pdat->busying = 1;
		timer_forward_now(&pdat->timer, ns_to_ktime(1000000000ULL / pdat->speed));
		return 1;
	}
	pdat->step = 0;
	pdat->busying = 0;
	return 0;
}

static struct device_t * stepper_unipolar_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct stepper_unipolar_gpio_pdata_t * pdat;
	struct stepper_t * m;
	struct device_t * dev;
	char * mode = dt_read_string(n, "mode", NULL);
	int c0 = dt_read_int(n, "c0-gpio", -1);
	int c1 = dt_read_int(n, "c1-gpio", -1);
	int c2 = dt_read_int(n, "c2-gpio", -1);
	int c3 = dt_read_int(n, "c3-gpio", -1);

	if(!gpio_is_valid(c0) || !gpio_is_valid(c1) || !gpio_is_valid(c2) || !gpio_is_valid(c3))
		return NULL;

	pdat = malloc(sizeof(struct stepper_unipolar_gpio_pdata_t));
	if(!pdat)
		return NULL;

	m = malloc(sizeof(struct stepper_t));
	if(!m)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, stepper_unipolar_gpio_timer_function, m);
	if(strcasecmp(mode, "wave") == 0)
		pdat->mode = STEPPER_MODE_WAVE;
	else if(strcasecmp(mode, "fullstep") == 0)
		pdat->mode = STEPPER_MODE_FULLSTEP;
	else if(strcasecmp(mode, "halfstep") == 0)
		pdat->mode = STEPPER_MODE_HALFSTEP;
	else
		pdat->mode = STEPPER_MODE_WAVE;
	pdat->c0 = c0;
	pdat->c0cfg = dt_read_int(n, "c0-gpio-config", -1);
	pdat->c1 = c1;
	pdat->c1cfg = dt_read_int(n, "c1-gpio-config", -1);
	pdat->c2 = c2;
	pdat->c2cfg = dt_read_int(n, "c2-gpio-config", -1);
	pdat->c3 = c3;
	pdat->c3cfg = dt_read_int(n, "c3-gpio-config", -1);
	pdat->index = 0;
	pdat->enable = 0;
	pdat->dir = 0;
	pdat->step = 0;
	pdat->speed = 0;
	pdat->busying = 0;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = stepper_unipolar_gpio_enable,
	m->disable = stepper_unipolar_gpio_disable,
	m->move = stepper_unipolar_gpio_move,
	m->busying = stepper_unipolar_gpio_busying,
	m->priv = pdat;

	if(pdat->c0 >= 0)
	{
		if(pdat->c0cfg >= 0)
			gpio_set_cfg(pdat->c0, pdat->c0cfg);
		gpio_set_pull(pdat->c0, GPIO_PULL_UP);
		gpio_direction_output(pdat->c0, 0);
	}
	if(pdat->c1 >= 0)
	{
		if(pdat->c1cfg >= 0)
			gpio_set_cfg(pdat->c1, pdat->c1cfg);
		gpio_set_pull(pdat->c1, GPIO_PULL_UP);
		gpio_direction_output(pdat->c1, 0);
	}
	if(pdat->c2 >= 0)
	{
		if(pdat->c2cfg >= 0)
			gpio_set_cfg(pdat->c2, pdat->c2cfg);
		gpio_set_pull(pdat->c2, GPIO_PULL_UP);
		gpio_direction_output(pdat->c2, 0);
	}
	if(pdat->c3 >= 0)
	{
		if(pdat->c3cfg >= 0)
			gpio_set_cfg(pdat->c3, pdat->c3cfg);
		gpio_set_pull(pdat->c3, GPIO_PULL_UP);
		gpio_direction_output(pdat->c3, 0);
	}

	if(!register_stepper(&dev, m))
	{
		timer_cancel(&pdat->timer);

		free_device_name(m->name);
		free(m->priv);
		free(m);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void stepper_unipolar_gpio_remove(struct device_t * dev)
{
	struct stepper_t * m = (struct stepper_t *)dev->priv;
	struct stepper_unipolar_gpio_pdata_t * pdat = (struct stepper_unipolar_gpio_pdata_t *)m->priv;

	if(m && unregister_stepper(m))
	{
		timer_cancel(&pdat->timer);

		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void stepper_unipolar_gpio_suspend(struct device_t * dev)
{
}

static void stepper_unipolar_gpio_resume(struct device_t * dev)
{
}

static struct driver_t stepper_unipolar_gpio = {
	.name		= "stepper-unipolar-gpio",
	.probe		= stepper_unipolar_gpio_probe,
	.remove		= stepper_unipolar_gpio_remove,
	.suspend	= stepper_unipolar_gpio_suspend,
	.resume		= stepper_unipolar_gpio_resume,
};

static __init void stepper_unipolar_gpio_driver_init(void)
{
	register_driver(&stepper_unipolar_gpio);
}

static __exit void stepper_unipolar_gpio_driver_exit(void)
{
	unregister_driver(&stepper_unipolar_gpio);
}

driver_initcall(stepper_unipolar_gpio_driver_init);
driver_exitcall(stepper_unipolar_gpio_driver_exit);
