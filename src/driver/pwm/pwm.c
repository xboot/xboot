/*
 * driver/pwm/pwm.c
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
#include <pwm/pwm.h>

static ssize_t pwm_read_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%d", pwm->__enable ? 1 : 0);
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

static ssize_t pwm_read_duty(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%u", pwm->__duty);
}

static ssize_t pwm_write_duty(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int duty = strtol(buf, NULL, 0);
	pwm_config(pwm, duty, pwm->__period, pwm->__polarity);
	return size;
}

static ssize_t pwm_read_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%u", pwm->__period);
}

static ssize_t pwm_write_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int period = strtol(buf, NULL, 0);
	pwm_config(pwm, pwm->__duty, period, pwm->__polarity);
	return size;
}

static ssize_t pwm_read_polarity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%d", pwm->__polarity ? 1 : 0);
}

static ssize_t pwm_write_polarity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int polarity = strtol(buf, NULL, 0);
	pwm_config(pwm, pwm->__duty, pwm->__period, (polarity != 0) ? 1 : 0);
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

bool_t register_pwm(struct device_t ** device, struct pwm_t * pwm)
{
	struct device_t * dev;

	if(!pwm || !pwm->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	pwm->__enable = -1;
	pwm->__duty = -1;
	pwm->__period = -1;
	pwm->__polarity = -1;

	dev->name = strdup(pwm->name);
	dev->type = DEVICE_TYPE_PWM;
	dev->priv = pwm;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "enable", pwm_read_enable, pwm_write_enable, pwm);
	kobj_add_regular(dev->kobj, "duty", pwm_read_duty, pwm_write_duty, pwm);
	kobj_add_regular(dev->kobj, "period", pwm_read_period, pwm_write_period, pwm);
	kobj_add_regular(dev->kobj, "polarity", pwm_read_polarity, pwm_write_polarity, pwm);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_pwm(struct pwm_t * pwm)
{
	struct device_t * dev;

	if(!pwm || !pwm->name)
		return FALSE;

	dev = search_device(pwm->name, DEVICE_TYPE_PWM);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void pwm_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	if(pwm && pwm->config)
	{
		polarity = (polarity != 0) ? 1 : 0;
		period = (period < 0) ? 1000 : period;
		duty = (duty < 0) ? 0 : duty;
		duty = (duty > period) ? period : duty;

		if((pwm->__duty != duty) || (pwm->__period != period) || (pwm->__polarity != polarity))
		{
			pwm->config(pwm, duty, period, polarity);
			pwm->__duty = duty;
			pwm->__period = period;
			pwm->__polarity = polarity;
		}
	}
}

void pwm_enable(struct pwm_t * pwm)
{
	if(pwm && pwm->enable && (pwm->__enable != 1))
	{
		pwm->enable(pwm);
		pwm->__enable = 1;
	}
}

void pwm_disable(struct pwm_t * pwm)
{
	if(pwm && pwm->disable && (pwm->__enable != 0))
	{
		pwm->disable(pwm);
		pwm->__enable = 0;
	}
}
