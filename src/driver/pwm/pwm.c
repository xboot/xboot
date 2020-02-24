/*
 * driver/pwm/pwm.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <pwm/pwm.h>

static ssize_t pwm_write_config(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int duty, period;
	int polarity = 0;
	if(sscanf(buf, "%d %d %d", &duty, &period, &polarity) >= 2)
		pwm_config(pwm, duty, period, polarity);
	return size;
}

static ssize_t pwm_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int enable = strtol(buf, NULL, 0);
	if(enable != 0)
		pwm_enable(pwm);
	else
		pwm_disable(pwm);
	return size;
}

struct pwm_t * search_pwm(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_PWM);
	if(!dev)
		return NULL;
	return (struct pwm_t *)dev->priv;
}

struct device_t * register_pwm(struct pwm_t * pwm, struct driver_t * drv)
{
	struct device_t * dev;

	if(!pwm || !pwm->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(pwm->name);
	dev->type = DEVICE_TYPE_PWM;
	dev->driver = drv;
	dev->priv = pwm;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "config", NULL, pwm_write_config, pwm);
	kobj_add_regular(dev->kobj, "enable", NULL, pwm_write_enable, pwm);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_pwm(struct pwm_t * pwm)
{
	struct device_t * dev;

	if(pwm && pwm->name)
	{
		dev = search_device(pwm->name, DEVICE_TYPE_PWM);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

void pwm_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	if(pwm && pwm->config)
	{
		polarity = (polarity != 0) ? 1 : 0;
		period = (period < 0) ? 1000 : period;
		duty = (duty < 0) ? 0 : duty;
		duty = (duty > period) ? period : duty;
		pwm->config(pwm, duty, period, polarity);
	}
}

void pwm_enable(struct pwm_t * pwm)
{
	if(pwm && pwm->enable)
		pwm->enable(pwm);
}

void pwm_disable(struct pwm_t * pwm)
{
	if(pwm && pwm->disable)
		pwm->disable(pwm);
}
