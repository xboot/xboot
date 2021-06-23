/*
 * driver/wifi/wifi.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <wifi/wifi.h>

static ssize_t wifi_read_status(struct kobj_t * kobj, void * buf, size_t size)
{
	struct wifi_t * wifi = (struct wifi_t *)kobj->priv;
	char * status;

	switch(wifi_status(wifi))
	{
	case WIFI_STATUS_DISCONNECTED:
		status = "disconnected";
		break;
	case WIFI_STATUS_CONNECTING:
		status = "connecting";
		break;
	case WIFI_STATUS_CONNECTED:
		status = "connected";
		break;
	default:
		status = "unkown";
		break;
	}
	return sprintf(buf, "%s", status);
}

struct wifi_t * search_wifi(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_WIFI);
	if(!dev)
		return NULL;
	return (struct wifi_t *)dev->priv;
}

struct wifi_t * search_first_wifi(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_WIFI);
	if(!dev)
		return NULL;
	return (struct wifi_t *)dev->priv;
}

struct device_t * register_wifi(struct wifi_t * wifi, struct driver_t * drv)
{
	struct device_t * dev;

	if(!wifi || !wifi->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(wifi->name);
	dev->type = DEVICE_TYPE_WIFI;
	dev->driver = drv;
	dev->priv = wifi;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "status", wifi_read_status, NULL, wifi);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_wifi(struct wifi_t * wifi)
{
	struct device_t * dev;

	if(wifi && wifi->name)
	{
		dev = search_device(wifi->name, DEVICE_TYPE_WIFI);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

bool_t wifi_connect(struct wifi_t * wifi, const char * ssid, const char * passwd)
{
	enum wifi_status_t status = wifi_status(wifi);

	if((status != WIFI_STATUS_DISCONNECTED) && (status != WIFI_STATUS_CONNECTING))
		return TRUE;
	else
	{
		if(wifi && wifi->connect)
			return wifi->connect(wifi, ssid, passwd);
	}
	return FALSE;
}

bool_t wifi_disconnect(struct wifi_t * wifi)
{
	enum wifi_status_t status = wifi_status(wifi);

	if(status == WIFI_STATUS_DISCONNECTED)
		return TRUE;
	else
	{
		if(wifi && wifi->disconnect)
			return wifi->disconnect(wifi);
	}
	return FALSE;
}

bool_t wifi_client_open(struct wifi_t * wifi, const char * ip, int port)
{
	enum wifi_status_t status = wifi_status(wifi);

	if(status == WIFI_STATUS_CLIENT_OPENED)
		return TRUE;
	else if(status == WIFI_STATUS_CONNECTED)
	{
		if(wifi && wifi->client_open)
			return wifi->client_open(wifi, ip, port);
	}
	return FALSE;
}

bool_t wifi_client_close(struct wifi_t * wifi)
{
	enum wifi_status_t status = wifi_status(wifi);

	if(status == WIFI_STATUS_CONNECTED)
		return TRUE;
	else if(status == WIFI_STATUS_CLIENT_OPENED)
	{
		if(wifi && wifi->client_close)
			return wifi->client_close(wifi);
	}
	return FALSE;
}

enum wifi_status_t wifi_status(struct wifi_t * wifi)
{
	if(wifi && wifi->status)
		return wifi->status(wifi);
	return WIFI_STATUS_DISCONNECTED;
}

int wifi_read(struct wifi_t * wifi, void * buf, int count)
{
	if(wifi && wifi->read)
		return wifi->read(wifi, buf, count);
	return 0;
}

int wifi_write(struct wifi_t * wifi, void * buf, int count)
{
	if(wifi && wifi->write)
		return wifi->write(wifi, buf, count);
	return 0;
}

int wifi_ioctl(struct wifi_t * wifi, const char * cmd, void * arg)
{
	if(wifi && wifi->ioctl)
		return wifi->ioctl(wifi, cmd, arg);
	return -1;
}
