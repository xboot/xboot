/*
 * driver/stepper/stepper-pluse-dir.c
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

struct stepper_pluse_dir_pdata_t {
	struct timer_t timer;
	int pluse;
	int plusecfg;
	int pluseinv;
	int dir;
	int dircfg;
	int dirinv;
	int en;
	int encfg;
	int eninv;
	int dspeed;
	int enable;
	int step;
	int flag;
	int speed;
	int busying;
};

static void stepper_pluse_dir_enable(struct stepper_t * m)
{
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;
	if(pdat->en >= 0)
		gpio_set_value(pdat->en, pdat->eninv ? 0 : 1);
	pdat->enable = 1;
}

static void stepper_pluse_dir_disable(struct stepper_t * m)
{
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;
	if(pdat->en >= 0)
		gpio_set_value(pdat->en, pdat->eninv ? 1 : 0);
	pdat->enable = 0;
}

static void stepper_pluse_dir_move(struct stepper_t * m, int step, int speed)
{
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;
	if(pdat->enable && !pdat->busying)
	{
		if(pdat->dir >= 0)
		{
			if(step < 0)
				gpio_set_value(pdat->dir, pdat->dirinv ? 1 : 0);
			else
				gpio_set_value(pdat->dir, pdat->dirinv ? 0 : 1);
		}
		pdat->step = abs(step);
		pdat->speed = (speed > 0) ? speed : pdat->dspeed;
		pdat->busying = 1;
		timer_start_now(&pdat->timer, ns_to_ktime(500000000ULL / pdat->speed));
	}
}

static int stepper_pluse_dir_busying(struct stepper_t * m)
{
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;
	return pdat->busying;
}

static int stepper_pluse_dir_timer_function(struct timer_t * timer, void * data)
{
	struct stepper_t * m = (struct stepper_t *)(data);
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;

	if(pdat->enable && (pdat->step > 0))
	{
		pdat->flag = !pdat->flag;
		if(pdat->flag)
		{
			gpio_set_value(pdat->pluse, pdat->pluseinv ? 0 : 1);
		}
		else
		{
			gpio_set_value(pdat->pluse, pdat->pluseinv ? 1 : 0);
			pdat->step--;
		}

		if(pdat->step > 0)
		{
			pdat->busying = 1;
			timer_forward_now(&pdat->timer, ns_to_ktime(500000000ULL / pdat->speed));
			return 1;
		}
	}
	gpio_set_value(pdat->pluse, pdat->pluseinv ? 1 : 0);
	pdat->step = 0;
	pdat->busying = 0;
	return 0;
}

static struct device_t * stepper_pluse_dir_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct stepper_pluse_dir_pdata_t * pdat;
	struct stepper_t * m;
	struct device_t * dev;
	int pluse = dt_read_int(n, "pluse-gpio", -1);
	int dir = dt_read_int(n, "dir-gpio", -1);
	int en = dt_read_int(n, "enable-gpio", -1);

	if(!gpio_is_valid(pluse))
		return NULL;

	pdat = malloc(sizeof(struct stepper_pluse_dir_pdata_t));
	if(!pdat)
		return NULL;

	m = malloc(sizeof(struct stepper_t));
	if(!m)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, stepper_pluse_dir_timer_function, m);
	pdat->pluse = pluse;
	pdat->plusecfg = dt_read_int(n, "pluse-gpio-config", -1);
	pdat->pluseinv = dt_read_int(n, "pluse-gpio-inverted", 0);
	pdat->dir = dir;
	pdat->dircfg = dt_read_int(n, "dir-gpio-config", -1);
	pdat->dirinv = dt_read_int(n, "dir-gpio-inverted", 0);
	pdat->en = en;
	pdat->encfg = dt_read_int(n, "enable-gpio-config", -1);
	pdat->eninv = dt_read_int(n, "enable-gpio-inverted", 0);
	pdat->dspeed = dt_read_int(n, "default-speed", 100);
	pdat->enable = 0;
	pdat->step = 0;
	pdat->flag = 0;
	pdat->speed = 0;
	pdat->busying = 0;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = stepper_pluse_dir_enable;
	m->disable = stepper_pluse_dir_disable;
	m->move = stepper_pluse_dir_move;
	m->busying = stepper_pluse_dir_busying;
	m->priv = pdat;

	if(pdat->pluse >= 0)
	{
		if(pdat->plusecfg >= 0)
			gpio_set_cfg(pdat->pluse, pdat->plusecfg);
		gpio_set_pull(pdat->pluse, pdat->pluseinv ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_set_direction(pdat->pluse, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->pluse, pdat->pluseinv ? 1 : 0);
	}
	if(pdat->dir >= 0)
	{
		if(pdat->dircfg >= 0)
			gpio_set_cfg(pdat->dir, pdat->dircfg);
		gpio_set_pull(pdat->dir, pdat->dirinv ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_set_direction(pdat->dir, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->dir, pdat->dirinv ? 1 : 0);
	}
	if(pdat->en >= 0)
	{
		if(pdat->encfg >= 0)
			gpio_set_cfg(pdat->en, pdat->encfg);
		gpio_set_pull(pdat->en, pdat->eninv ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_set_direction(pdat->en, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->en, pdat->eninv ? 1 : 0);
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

static void stepper_pluse_dir_remove(struct device_t * dev)
{
	struct stepper_t * m = (struct stepper_t *)dev->priv;
	struct stepper_pluse_dir_pdata_t * pdat = (struct stepper_pluse_dir_pdata_t *)m->priv;

	if(m && unregister_stepper(m))
	{
		timer_cancel(&pdat->timer);

		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void stepper_pluse_dir_suspend(struct device_t * dev)
{
}

static void stepper_pluse_dir_resume(struct device_t * dev)
{
}

static struct driver_t stepper_pluse_dir = {
	.name		= "stepper-pluse-dir",
	.probe		= stepper_pluse_dir_probe,
	.remove		= stepper_pluse_dir_remove,
	.suspend	= stepper_pluse_dir_suspend,
	.resume		= stepper_pluse_dir_resume,
};

static __init void stepper_pluse_dir_driver_init(void)
{
	register_driver(&stepper_pluse_dir);
}

static __exit void stepper_pluse_dir_driver_exit(void)
{
	unregister_driver(&stepper_pluse_dir);
}

driver_initcall(stepper_pluse_dir_driver_init);
driver_exitcall(stepper_pluse_dir_driver_exit);
