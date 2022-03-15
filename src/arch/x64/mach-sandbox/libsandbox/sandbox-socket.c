#include <x.h>
#include <sandbox.h>

#define MAX_CLIENTS		(32)

struct sandbox_socket_listen_context_t {
	int fd;
	int clients[MAX_CLIENTS];
	struct sockaddr_in addr;
};

struct sandbox_socket_connect_context_t {
	int fd;
};

static inline uint32_t shash(const char * s)
{
	uint32_t v = 5381;
	if(s)
	{
		while(*s)
			v = (v << 5) + v + (*s++);
	}
	return v;
}

static int host_to_addr(const char * host, struct in_addr * addr)
{
	struct hostent he, * result;
	int herr, ret, bufsz = 512;
	char * buf = NULL;

	do {
		char * nbuf = (char *)realloc(buf, bufsz);
		if(nbuf == NULL)
		{
			free(buf);
			return ENOMEM;
		}
		buf = nbuf;
		ret = gethostbyname_r(host, &he, buf, bufsz, &result, &herr);
		bufsz <<= 1;
	} while(ret == ERANGE);
	if((ret == 0) && (result != NULL))
		*addr = *(struct in_addr *)he.h_addr;
	else if(result != &he)
		ret = herr;
	free(buf);

	return ret;
}

void * sandbox_socket_listen(const char * type, int port)
{
	struct sandbox_socket_listen_context_t * lctx;
	int namespace;
	int style;

	switch(shash(type))
	{
	case 0x0b88ad8c: /* "tcp" */
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	case 0x7c9e5f40: /* "tcp4" */
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	case 0x7c9e5f42: /* "tcp6" */
		namespace = AF_INET6;
		style = SOCK_STREAM;
		break;
	case 0x0b88b1ee: /* "udp" */
		namespace = AF_INET;
		style = SOCK_DGRAM;
		break;
	case 0x7c9eefe2: /* "udp4" */
		namespace = AF_INET;
		style = SOCK_DGRAM;
		break;
	case 0x7c9eefe4: /* "udp6" */
		namespace = AF_INET6;
		style = SOCK_DGRAM;
		break;
	default:
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	}
	lctx = malloc(sizeof(struct sandbox_socket_listen_context_t));
	if(lctx)
	{
		memset(lctx, 0, sizeof(struct sandbox_socket_listen_context_t));
		lctx->fd = socket(namespace, style, 0);
		if(lctx->fd < 0)
		{
			free(lctx);
			return NULL;
		}
		int opt = 1;
		if(setsockopt(lctx->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) != 0)
		{
			free(lctx);
			return NULL;
		}
		int flags = fcntl(lctx->fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(lctx->fd, F_SETFL, flags);
		lctx->addr.sin_family = namespace;
		lctx->addr.sin_addr.s_addr = htonl(INADDR_ANY);
		lctx->addr.sin_port = htons(port);
		if(bind(lctx->fd, (struct sockaddr *)&lctx->addr, sizeof(struct sockaddr_in)) < 0)
		{
			close(lctx->fd);
			free(lctx);
			return NULL;
		}
		if(listen(lctx->fd, 5) < 0)
		{
			close(lctx->fd);
			free(lctx);
			return NULL;
		}
		return lctx;
	}
	return NULL;
}

void * sandbox_socket_accept(void * l)
{
	struct sandbox_socket_listen_context_t * lctx = (struct sandbox_socket_listen_context_t *)l;
	struct sandbox_socket_connect_context_t * cctx;
	struct timeval tv;
	fd_set fds;
	int maxfd = lctx->fd;
	int fd;
	int i;

	FD_ZERO(&fds);
	FD_SET(lctx->fd, &fds);
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		fd = lctx->clients[i];
		if(fd > 0)
			FD_SET(fd, &fds);
		if(fd > maxfd)
			maxfd = fd;
	}
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
	if(select(maxfd + 1, &fds, NULL, NULL, &tv) > 0)
	{
		if(FD_ISSET(lctx->fd, &fds))
		{
			fd = accept(lctx->fd, (struct sockaddr *)NULL, NULL);
			if(fd >= 0)
			{
				for(i = 0; i < MAX_CLIENTS; i++)
				{
					if(lctx->clients[i] == 0)
					{
						lctx->clients[i] = fd;
						break;
					}
				}
				if(i >= MAX_CLIENTS)
					return NULL;
				cctx = malloc(sizeof(struct sandbox_socket_connect_context_t));
				if(cctx)
				{
					cctx->fd = fd;
					int flags = fcntl(cctx->fd, F_GETFL);
					flags |= O_NONBLOCK;
					fcntl(cctx->fd, F_SETFL, flags);
					return cctx;
				}
			}
		}
    }
	return NULL;
}

void * sandbox_socket_connect(const char * type, const char * host, int port)
{
	struct sandbox_socket_connect_context_t * cctx;
	struct sockaddr_in addr;
	int namespace;
	int style;

	switch(shash(type))
	{
	case 0x0b88ad8c: /* "tcp" */
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	case 0x7c9e5f40: /* "tcp4" */
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	case 0x7c9e5f42: /* "tcp6" */
		namespace = AF_INET6;
		style = SOCK_STREAM;
		break;
	case 0x0b88b1ee: /* "udp" */
		namespace = AF_INET;
		style = SOCK_DGRAM;
		break;
	case 0x7c9eefe2: /* "udp4" */
		namespace = AF_INET;
		style = SOCK_DGRAM;
		break;
	case 0x7c9eefe4: /* "udp6" */
		namespace = AF_INET6;
		style = SOCK_DGRAM;
		break;
	default:
		namespace = AF_INET;
		style = SOCK_STREAM;
		break;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = namespace;
	addr.sin_port = htons(port);
	if(host_to_addr(host, &addr.sin_addr) != 0)
		return NULL;
	cctx = malloc(sizeof(struct sandbox_socket_connect_context_t));
	if(cctx)
	{
		cctx->fd = socket(namespace, style, 0);
		if(connect(cctx->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			close(cctx->fd);
			free(cctx);
			return NULL;
		}
		int flags = fcntl(cctx->fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(cctx->fd, F_SETFL, flags);
		return cctx;
	}
	return NULL;
}

int sandbox_socket_read(void * c, void * buf, int count)
{
	struct sandbox_socket_connect_context_t * cctx = (struct sandbox_socket_connect_context_t *)c;
	int n;

	if((n = recv(cctx->fd, buf, count, 0)) > 0)
		return n;
	return 0;
}

int sandbox_socket_write(void * c, void * buf, int count)
{
	struct sandbox_socket_connect_context_t * cctx = (struct sandbox_socket_connect_context_t *)c;
	int n;

	if((n = send(cctx->fd, buf, count, 0)) > 0)
		return n;
	return 0;
}

int sandbox_socket_status(void * c)
{
	struct sandbox_socket_connect_context_t * cctx = (struct sandbox_socket_connect_context_t *)c;
	struct tcp_info info;
	int len = sizeof(info);

	getsockopt(cctx->fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
	if(info.tcpi_state == TCP_ESTABLISHED)
		return 1;
	return 0;
}

void sandbox_socket_close(void * c)
{
	struct sandbox_socket_connect_context_t * cctx = (struct sandbox_socket_connect_context_t *)c;

	if(cctx)
		close(cctx->fd);
}

void sandbox_socket_delete(void * l)
{
	struct sandbox_socket_listen_context_t * lctx = (struct sandbox_socket_listen_context_t *)l;
	int i;

	if(lctx)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if(lctx->clients[i] != 0)
				close(lctx->clients[i]);
		}
		close(lctx->fd);
	}
}
