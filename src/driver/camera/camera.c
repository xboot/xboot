/*
 * driver/camera/camera.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <camera/camera.h>

static ssize_t camera_read_gain(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_gain(cam));
}

static ssize_t camera_write_gain(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_gain(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_exposure(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_exposure(cam));
}

static ssize_t camera_write_exposure(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_exposure(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_white_balance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_white_balance(cam));
}

static ssize_t camera_write_white_balance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_white_balance(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_focus(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_focus(cam));
}

static ssize_t camera_write_focus(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_focus(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_mirror(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_mirror(cam));
}

static ssize_t camera_write_mirror(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_mirror(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_saturation(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_saturation(cam));
}

static ssize_t camera_write_saturation(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_saturation(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_brightness(cam));
}

static ssize_t camera_write_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_brightness(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_contrast(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_contrast(cam));
}

static ssize_t camera_write_contrast(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_contrast(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_hue(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_hue(cam));
}

static ssize_t camera_write_hue(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_hue(cam, strtol(buf, NULL, 0));
	return size;
}

static ssize_t camera_read_sharpness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	return sprintf(buf, "%d", camera_get_sharpness(cam));
}

static ssize_t camera_write_sharpness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct camera_t * cam = (struct camera_t *)kobj->priv;
	camera_set_sharpness(cam, strtol(buf, NULL, 0));
	return size;
}

struct camera_t * search_camera(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_CAMERA);
	if(!dev)
		return NULL;
	return (struct camera_t *)dev->priv;
}

struct camera_t * search_first_camera(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_CAMERA);
	if(!dev)
		return NULL;
	return (struct camera_t *)dev->priv;
}

struct device_t * register_camera(struct camera_t * cam, struct driver_t * drv)
{
	struct device_t * dev;

	if(!cam || !cam->name)
		return NULL;

	if(!cam->start || !cam->stop || !cam->capture)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(cam->name);
	dev->type = DEVICE_TYPE_CAMERA;
	dev->driver = drv;
	dev->priv = cam;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "gain", camera_read_gain, camera_write_gain, cam);
	kobj_add_regular(dev->kobj, "exposure", camera_read_exposure, camera_write_exposure, cam);
	kobj_add_regular(dev->kobj, "white-balance", camera_read_white_balance, camera_write_white_balance, cam);
	kobj_add_regular(dev->kobj, "focus", camera_read_focus, camera_write_focus, cam);
	kobj_add_regular(dev->kobj, "mirror", camera_read_mirror, camera_write_mirror, cam);
	kobj_add_regular(dev->kobj, "saturation", camera_read_saturation, camera_write_saturation, cam);
	kobj_add_regular(dev->kobj, "brightness", camera_read_brightness, camera_write_brightness, cam);
	kobj_add_regular(dev->kobj, "contrast", camera_read_contrast, camera_write_contrast, cam);
	kobj_add_regular(dev->kobj, "hue", camera_read_hue, camera_write_hue, cam);
	kobj_add_regular(dev->kobj, "sharpness", camera_read_sharpness, camera_write_sharpness, cam);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_camera(struct camera_t * cam)
{
	struct device_t * dev;

	if(cam && cam->name)
	{
		dev = search_device(cam->name, DEVICE_TYPE_CAMERA);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

int camera_start(struct camera_t * cam, enum video_format_t fmt, int width, int height)
{
	if(cam && cam->start)
		return cam->start(cam, fmt, width, height);
	return 0;
}

int camera_stop(struct camera_t * cam)
{
	if(cam && cam->stop)
		return cam->stop(cam);
	return 0;
}

int camera_capture(struct camera_t * cam, struct video_frame_t * frame, int timeout)
{
	if(cam && cam->capture)
	{
		if(timeout > 0)
		{
			ktime_t t = ktime_add_ms(ktime_get(), timeout);
			do {
				if(cam->capture(cam, frame))
					return 1;
				msleep(1);
			} while(ktime_before(ktime_get(), t));
		}
		else
		{
			return cam->capture(cam, frame);
		}
	}
	return 0;
}
