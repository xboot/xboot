/*
 * driver/net/net.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <net/net.h>

static ssize_t net_read_type(struct kobj_t * kobj, void * buf, size_t size)
{
	struct net_t * net = (struct net_t *)kobj->priv;
	char type[256];

	if(net_ioctl(net, "net-get-type", type) >= 0)
		return sprintf(buf, "%s", type);
	return sprintf(buf, "unknown");
}

struct net_t * search_net(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_NET);
	if(!dev)
		return NULL;
	return (struct net_t *)dev->priv;
}

struct net_t * search_first_net(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_NET);
	if(!dev)
		return NULL;
	return (struct net_t *)dev->priv;
}

struct device_t * register_net(struct net_t * net, struct driver_t * drv)
{
	struct device_t * dev;

	if(!net || !net->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(net->name);
	dev->type = DEVICE_TYPE_NET;
	dev->driver = drv;
	dev->priv = net;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "type", net_read_type, NULL, net);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_net(struct net_t * net)
{
	struct device_t * dev;

	if(net && net->name)
	{
		dev = search_device(net->name, DEVICE_TYPE_NET);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

struct socket_listen_t * net_listen(struct net_t * net, const char * type, const char * address)
{
	if(net && type && address)
		return net->listen(net, type, address);
	return NULL;
}

struct socket_connect_t * net_accept(struct socket_listen_t * l)
{
	if(l && l->net)
		return l->net->accept(l);
	return NULL;
}

struct socket_connect_t * net_connect(struct net_t * net, const char * type, const char * address)
{
	if(net && type && address)
		return net->connect(net, type, address);
	return NULL;
}

int net_read(struct socket_connect_t * c, void * buf, int count)
{
	if(c && c->net && buf && (count > 0))
		return c->net->read(c, buf, count);
	return 0;
}

int net_write(struct socket_connect_t * c, void * buf, int count)
{
	if(c && c->net && buf && (count > 0))
		return c->net->write(c, buf, count);
	return 0;
}

void net_close(struct socket_connect_t * c)
{
	if(c && c->net)
		c->net->close(c);
}

void net_delete(struct socket_listen_t * l)
{
	if(l && l->net)
		l->net->delete(l);
}

int net_ioctl(struct net_t * net, const char * cmd, void * arg)
{
	if(net && net->ioctl)
		return net->ioctl(net, cmd, arg);
	return -1;
}
