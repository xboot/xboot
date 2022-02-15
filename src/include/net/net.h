#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct socket_listen_t {
	struct net_t * net;
	void * priv;
};

struct socket_connect_t {
	struct net_t * net;
	void * priv;
};

struct net_t
{
	char * name;

	struct socket_listen_t * (*listen)(struct net_t * net, const char * type, const char * address);
	struct socket_connect_t * (*accept)(struct socket_listen_t * l);
	struct socket_connect_t * (*connect)(struct net_t * net, const char * type, const char * address);
	int (*read)(struct socket_connect_t * c, void * buf, int count);
	int (*write)(struct socket_connect_t * c, void * buf, int count);
	int (*close)(struct socket_connect_t * c);
	int (*shutdown)(struct socket_listen_t * l);
	int (*ioctl)(struct net_t * net, const char * cmd, void * arg);

	void * priv;
};

static inline struct socket_listen_t * socket_listen_alloc(struct net_t * net, void * data)
{
	struct socket_listen_t * l = malloc(sizeof(struct socket_listen_t));

	if(l)
	{
		l->net = net;
		l->priv = data;
		return l;
	}
	return NULL;
}

static inline void socket_listen_free(struct socket_listen_t * l)
{
	if(l)
		free(l);
}

static inline struct socket_connect_t * socket_connect_alloc(struct net_t * net, void * data)
{
	struct socket_connect_t * c = malloc(sizeof(struct socket_connect_t));

	if(c)
	{
		c->net = net;
		c->priv = data;
		return c;
	}
	return NULL;
}

static inline void socket_connect_free(struct socket_connect_t * c)
{
	if(c)
		free(c);
}

struct net_t * search_net(const char * name);
struct net_t * search_first_net(void);
struct device_t * register_net(struct net_t * net, struct driver_t * drv);
void unregister_net(struct net_t * net);

struct socket_listen_t * net_listen(struct net_t * net, const char * type, const char * address);
struct socket_connect_t * net_accept(struct socket_listen_t * l);
struct socket_connect_t * net_connect(struct net_t * net, const char * type, const char * address);
int net_read(struct socket_connect_t * c, void * buf, int count);
int net_write(struct socket_connect_t * c, void * buf, int count);
int net_close(struct socket_connect_t * c);
int net_shutdown(struct socket_listen_t * l);
int net_ioctl(struct net_t * net, const char * cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __NET_H__ */
