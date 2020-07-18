/*
 * driver/cam-sandbox.c
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
#include <sandbox.h>

struct cam_sandbox_pdata_t {
	char * path;
	void * ctx;

	enum video_format_t fmt;
	int width;
	int height;
};

static int cam_start(struct camera_t * cam, enum video_format_t fmt, int width, int height)
{
	struct cam_sandbox_pdata_t * pdat = (struct cam_sandbox_pdata_t *)cam->priv;

	pdat->fmt = fmt;
	pdat->width = width;
	pdat->height = height;
	if((pdat->ctx = sandbox_cam_start(pdat->path, (int *)&pdat->fmt, &pdat->width, &pdat->height)))
		return 1;
	return 0;
}

static int cam_stop(struct camera_t * cam)
{
	struct cam_sandbox_pdata_t * pdat = (struct cam_sandbox_pdata_t *)cam->priv;
	if(pdat->ctx)
		sandbox_cam_stop(pdat->ctx);
	return 1;
}

static int cam_capture(struct camera_t * cam, struct video_frame_t * frame)
{
	struct cam_sandbox_pdata_t * pdat = (struct cam_sandbox_pdata_t *)cam->priv;
	if(pdat->ctx)
	{
		frame->buflen = sandbox_cam_capture(pdat->ctx, &frame->buf);
		if(frame->buflen > 0)
		{
			frame->fmt = pdat->fmt;
			frame->width = pdat->width;
			frame->height = pdat->height;
			return 1;
		}
	}
	return 0;
}

static int cam_ioctl(struct camera_t * cam, const char * cmd, void * arg)
{
	switch(shash(cmd))
	{
	case 0x440a6553: /* "camera-set-gain" */
		break;
	case 0x30f46ac7: /* "camera-get-gain" */
		break;
	case 0x8531e7af: /* "camera-set-exposure" */
		break;
	case 0x39c98723: /* "camera-get-exposure" */
		break;
	case 0x2d64af48: /* "camera-set-white-balance" */
		break;
	case 0x4b2e173c: /* "camera-get-white-balance" */
		break;
	case 0xc54c8c54: /* "camera-set-focus" */
		break;
	case 0x4f774048: /* "camera-get-focus" */
		break;
	case 0x7ecdcd0f: /* "camera-set-mirror" */
		break;
	case 0x4e4eff83: /* "camera-get-mirror" */
		break;
	case 0xd5d73dfe: /* "camera-set-saturation" */
		break;
	case 0x0ed48a72: /* "camera-get-saturation" */
		break;
	case 0xdae4842d: /* "camera-set-brightness" */
		break;
	case 0x13e1d0a1: /* "camera-get-brightness" */
		break;
	case 0xf3916322: /* "camera-set-contrast" */
		break;
	case 0xa8290296: /* "camera-get-contrast" */
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * cam_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cam_sandbox_pdata_t * pdat;
	struct camera_t * cam;
	struct device_t * dev;
	char * path = dt_read_string(n, "device", NULL);

	if(!sandbox_cam_exist(path))
		return NULL;

	pdat = malloc(sizeof(struct cam_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	cam = malloc(sizeof(struct camera_t));
	if(!cam)
	{
		free(pdat);
		return NULL;
	}

	pdat->path = strdup(path);
	pdat->ctx = NULL;

	cam->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	cam->start = cam_start;
	cam->stop = cam_stop;
	cam->capture = cam_capture;
	cam->ioctl = cam_ioctl;
	cam->priv = pdat;

	if(!(dev = register_camera(cam, drv)))
	{
		free(pdat->path);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
		return NULL;
	}
	return dev;
}

static void cam_sandbox_remove(struct device_t * dev)
{
	struct camera_t * cam = (struct camera_t *)dev->priv;
	struct cam_sandbox_pdata_t * pdat = (struct cam_sandbox_pdata_t *)cam->priv;

	if(cam)
	{
		unregister_camera(cam);
		free(pdat->path);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
	}
}

static void cam_sandbox_suspend(struct device_t * dev)
{
}

static void cam_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t cam_sandbox = {
	.name		= "cam-sandbox",
	.probe		= cam_sandbox_probe,
	.remove		= cam_sandbox_remove,
	.suspend	= cam_sandbox_suspend,
	.resume		= cam_sandbox_resume,
};

static __init void cam_sandbox_driver_init(void)
{
	register_driver(&cam_sandbox);
}

static __exit void cam_sandbox_driver_exit(void)
{
	unregister_driver(&cam_sandbox);
}

driver_initcall(cam_sandbox_driver_init);
driver_exitcall(cam_sandbox_driver_exit);
