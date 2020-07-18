/*
 * driver/camera/camera.c
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
#include <camera/camera.h>

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
			} while(ktime_before(ktime_get(), t));
		}
		else
		{
			return cam->capture(cam, frame);
		}
	}
	return 0;
}

int camera_ioctl(struct camera_t * cam, const char * cmd, void * arg)
{
	if(cam && cam->ioctl)
		return cam->ioctl(cam, cmd, arg);
	return -1;
}
