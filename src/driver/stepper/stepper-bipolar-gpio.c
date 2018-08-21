/*
 * driver/stepper/stepper-bipolar-gpio.c
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

struct stepper_bipolar_gpio_pdata_t {
	struct timer_t timer;
	enum stepper_mode_t mode;
	int pa;
	int pacfg;
	int na;
	int nacfg;
	int pb;
	int pbcfg;
	int nb;
	int nbcfg;
	int dspeed;
	int index;
	int enable;
	int dir;
	int step;
	int speed;
	int busying;
};

static void stepper_wave(struct stepper_bipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 0);
		break;

	case 1:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 2:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 0);
		break;

	case 3:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	default:
		break;
	}
}

static void stepper_fullstep(struct stepper_bipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 1:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 2:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	case 3:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	default:
		break;
	}
}

static void stepper_halfstep(struct stepper_bipolar_gpio_pdata_t * pdat)
{
	switch(pdat->index)
	{
	case 0:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 0);
		break;

	case 1:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 2:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 3:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 1);
		gpio_set_value(pdat->nb, 0);
		break;

	case 4:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 0);
		break;

	case 5:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 1);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	case 6:
		gpio_set_value(pdat->pa, 0);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	case 7:
		gpio_set_value(pdat->pa, 1);
		gpio_set_value(pdat->na, 0);
		gpio_set_value(pdat->pb, 0);
		gpio_set_value(pdat->nb, 1);
		break;

	default:
		break;
	}
}

static void stepper_bipolar_gpio_enable(struct stepper_t * m)
{
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;
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

static void stepper_bipolar_gpio_disable(struct stepper_t * m)
{
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;
	gpio_set_value(pdat->pa, 0);
	gpio_set_value(pdat->na, 0);
	gpio_set_value(pdat->pb, 0);
	gpio_set_value(pdat->nb, 0);
	pdat->enable = 0;
}

static void stepper_bipolar_gpio_move(struct stepper_t * m, int step, int speed)
{
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;
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

static int stepper_bipolar_gpio_busying(struct stepper_t * m)
{
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;
	return pdat->busying;
}

static int stepper_bipolar_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct stepper_t * m = (struct stepper_t *)(data);
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;

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

static struct device_t * stepper_bipolar_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct stepper_bipolar_gpio_pdata_t * pdat;
	struct stepper_t * m;
	struct device_t * dev;
	char * mode = dt_read_string(n, "drive-mode", NULL);
	int pa = dt_read_int(n, "pa-gpio", -1);
	int na = dt_read_int(n, "na-gpio", -1);
	int pb = dt_read_int(n, "pb-gpio", -1);
	int nb = dt_read_int(n, "nb-gpio", -1);

	if(!gpio_is_valid(pa) || !gpio_is_valid(na) || !gpio_is_valid(pb) || !gpio_is_valid(nb))
		return NULL;

	pdat = malloc(sizeof(struct stepper_bipolar_gpio_pdata_t));
	if(!pdat)
		return NULL;

	m = malloc(sizeof(struct stepper_t));
	if(!m)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, stepper_bipolar_gpio_timer_function, m);
	if(strcasecmp(mode, "wave") == 0)
		pdat->mode = STEPPER_MODE_WAVE;
	else if(strcasecmp(mode, "fullstep") == 0)
		pdat->mode = STEPPER_MODE_FULLSTEP;
	else if(strcasecmp(mode, "halfstep") == 0)
		pdat->mode = STEPPER_MODE_HALFSTEP;
	else
		pdat->mode = STEPPER_MODE_WAVE;
	pdat->pa = pa;
	pdat->pacfg = dt_read_int(n, "pa-gpio-config", -1);
	pdat->na = na;
	pdat->nacfg = dt_read_int(n, "na-gpio-config", -1);
	pdat->pb = pb;
	pdat->pbcfg = dt_read_int(n, "pb-gpio-config", -1);
	pdat->nb = nb;
	pdat->nbcfg = dt_read_int(n, "nb-gpio-config", -1);
	pdat->dspeed = dt_read_int(n, "default-speed", 100);
	pdat->index = 0;
	pdat->enable = 0;
	pdat->dir = 0;
	pdat->step = 0;
	pdat->speed = 0;
	pdat->busying = 0;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = stepper_bipolar_gpio_enable;
	m->disable = stepper_bipolar_gpio_disable;
	m->move = stepper_bipolar_gpio_move;
	m->busying = stepper_bipolar_gpio_busying;
	m->priv = pdat;

	if(pdat->pa >= 0)
	{
		if(pdat->pacfg >= 0)
			gpio_set_cfg(pdat->pa, pdat->pacfg);
		gpio_set_pull(pdat->pa, GPIO_PULL_UP);
		gpio_set_direction(pdat->pa, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->pa, 0);
	}
	if(pdat->na >= 0)
	{
		if(pdat->nacfg >= 0)
			gpio_set_cfg(pdat->na, pdat->nacfg);
		gpio_set_pull(pdat->na, GPIO_PULL_UP);
		gpio_set_direction(pdat->na, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->na, 0);
	}
	if(pdat->pb >= 0)
	{
		if(pdat->pbcfg >= 0)
			gpio_set_cfg(pdat->pb, pdat->pbcfg);
		gpio_set_pull(pdat->pb, GPIO_PULL_UP);
		gpio_set_direction(pdat->pb, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->pb, 0);
	}
	if(pdat->nb >= 0)
	{
		if(pdat->nbcfg >= 0)
			gpio_set_cfg(pdat->nb, pdat->nbcfg);
		gpio_set_pull(pdat->nb, GPIO_PULL_UP);
		gpio_set_direction(pdat->nb, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->nb, 0);
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

static void stepper_bipolar_gpio_remove(struct device_t * dev)
{
	struct stepper_t * m = (struct stepper_t *)dev->priv;
	struct stepper_bipolar_gpio_pdata_t * pdat = (struct stepper_bipolar_gpio_pdata_t *)m->priv;

	if(m && unregister_stepper(m))
	{
		timer_cancel(&pdat->timer);

		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void stepper_bipolar_gpio_suspend(struct device_t * dev)
{
}

static void stepper_bipolar_gpio_resume(struct device_t * dev)
{
}

static struct driver_t stepper_bipolar_gpio = {
	.name		= "stepper-bipolar-gpio",
	.probe		= stepper_bipolar_gpio_probe,
	.remove		= stepper_bipolar_gpio_remove,
	.suspend	= stepper_bipolar_gpio_suspend,
	.resume		= stepper_bipolar_gpio_resume,
};

static __init void stepper_bipolar_gpio_driver_init(void)
{
	register_driver(&stepper_bipolar_gpio);
}

static __exit void stepper_bipolar_gpio_driver_exit(void)
{
	unregister_driver(&stepper_bipolar_gpio);
}

driver_initcall(stepper_bipolar_gpio_driver_init);
driver_exitcall(stepper_bipolar_gpio_driver_exit);
