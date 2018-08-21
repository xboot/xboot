/*
 * driver/stepper/stepper-unipolar-gpio.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <stepper/stepper.h>

enum stepper_mode_t {
	STEPPER_MODE_WAVE		= 0,
	STEPPER_MODE_FULLSTEP	= 1,
	STEPPER_MODE_HALFSTEP	= 2,
};

struct stepper_unipolar_gpio_pdata_t {
	struct timer_t timer;
	enum stepper_mode_t mode;
	int a;
	int acfg;
	int b;
	int bcfg;
	int c;
	int ccfg;
	int d;
	int dcfg;
	int dspeed;
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
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 1:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 2:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 0);
		break;

	case 3:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 1);
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
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 1:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 0);
		break;

	case 2:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 1);
		break;

	case 3:
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 1);
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
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 1:
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 2:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 0);
		break;

	case 3:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 1);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 0);
		break;

	case 4:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 0);
		break;

	case 5:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 1);
		gpio_set_value(pdat->d, 1);
		break;

	case 6:
		gpio_set_value(pdat->a, 0);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 1);
		break;

	case 7:
		gpio_set_value(pdat->a, 1);
		gpio_set_value(pdat->b, 0);
		gpio_set_value(pdat->c, 0);
		gpio_set_value(pdat->d, 1);
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
	gpio_set_value(pdat->a, 0);
	gpio_set_value(pdat->b, 0);
	gpio_set_value(pdat->c, 0);
	gpio_set_value(pdat->d, 0);
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
		pdat->speed = (speed > 0) ? speed : pdat->dspeed;
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
		if(pdat->step > 0)
		{
			pdat->busying = 1;
			timer_forward_now(&pdat->timer, ns_to_ktime(1000000000ULL / pdat->speed));
			return 1;
		}
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
	char * mode = dt_read_string(n, "drive-mode", NULL);
	int a = dt_read_int(n, "a-gpio", -1);
	int b = dt_read_int(n, "b-gpio", -1);
	int c = dt_read_int(n, "c-gpio", -1);
	int d = dt_read_int(n, "d-gpio", -1);

	if(!gpio_is_valid(a) || !gpio_is_valid(b) || !gpio_is_valid(c) || !gpio_is_valid(d))
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
	pdat->a = a;
	pdat->acfg = dt_read_int(n, "a-gpio-config", -1);
	pdat->b = b;
	pdat->bcfg = dt_read_int(n, "b-gpio-config", -1);
	pdat->c = c;
	pdat->ccfg = dt_read_int(n, "c-gpio-config", -1);
	pdat->d = d;
	pdat->dcfg = dt_read_int(n, "d-gpio-config", -1);
	pdat->dspeed = dt_read_int(n, "default-speed", 100);
	pdat->index = 0;
	pdat->enable = 0;
	pdat->dir = 0;
	pdat->step = 0;
	pdat->speed = 0;
	pdat->busying = 0;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = stepper_unipolar_gpio_enable;
	m->disable = stepper_unipolar_gpio_disable;
	m->move = stepper_unipolar_gpio_move;
	m->busying = stepper_unipolar_gpio_busying;
	m->priv = pdat;

	if(pdat->a >= 0)
	{
		if(pdat->acfg >= 0)
			gpio_set_cfg(pdat->a, pdat->acfg);
		gpio_set_pull(pdat->a, GPIO_PULL_UP);
		gpio_set_direction(pdat->a, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->a, 0);
	}
	if(pdat->b >= 0)
	{
		if(pdat->bcfg >= 0)
			gpio_set_cfg(pdat->b, pdat->bcfg);
		gpio_set_pull(pdat->b, GPIO_PULL_UP);
		gpio_set_direction(pdat->b, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->b, 0);
	}
	if(pdat->c >= 0)
	{
		if(pdat->ccfg >= 0)
			gpio_set_cfg(pdat->c, pdat->ccfg);
		gpio_set_pull(pdat->c, GPIO_PULL_UP);
		gpio_set_direction(pdat->c, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->c, 0);
	}
	if(pdat->d >= 0)
	{
		if(pdat->dcfg >= 0)
			gpio_set_cfg(pdat->d, pdat->dcfg);
		gpio_set_pull(pdat->d, GPIO_PULL_UP);
		gpio_set_direction(pdat->d, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->d, 0);
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
