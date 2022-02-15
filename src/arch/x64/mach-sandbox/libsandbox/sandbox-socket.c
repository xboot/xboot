#include <x.h>
#include <sandbox.h>

struct sandbox_socket_listen_context_t {
	int fd;
	struct sockaddr_in addr;
};

struct sandbox_socket_connect_context_t {
	int fd;
};

void * sandbox_socket_listen(const char * type, const char * address)
{
	struct sandbox_socket_listen_context_t * lctx;

	lctx = malloc(sizeof(struct sandbox_socket_listen_context_t));
	if(lctx)
	{
		memset(lctx, 0, sizeof(struct sandbox_socket_listen_context_t));
		lctx->fd = socket(AF_INET, SOCK_STREAM, 0);
		if(lctx->fd < 0)
		{
			free(lctx);
			return NULL;
		}
		lctx->addr.sin_family = AF_INET;
		lctx->addr.sin_addr.s_addr = htonl(INADDR_ANY);
		lctx->addr.sin_port = htons(6666);
		if(bind(lctx->fd, (struct sockaddr *)&lctx->addr, sizeof(struct sockaddr_in)) < 0)
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

	cctx = malloc(sizeof(struct sandbox_socket_connect_context_t));
	if(cctx)
	{
		cctx->fd = accept(lctx->fd, (struct sockaddr *)NULL, NULL);
		if(cctx->fd < 0)
		{
			free(cctx);
			return NULL;
		}
		return cctx;
	}
	return NULL;
}

void * sandbox_socket_connect(const char * type, const char * address)
{
	struct sandbox_socket_connect_context_t * cctx;
	struct sockaddr_in addr;

	cctx = malloc(sizeof(struct sandbox_socket_connect_context_t));
	if(cctx)
	{
		cctx->fd = socket(AF_INET, SOCK_STREAM, 0);
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(6666);
		inet_pton(AF_INET, "192.168.0.100", &addr.sin_addr);
		if(connect(cctx->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			close(cctx->fd);
			free(cctx);
			return NULL;
		}
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

int sandbox_socket_close(void * c)
{
	struct sandbox_socket_connect_context_t * cctx = (struct sandbox_socket_connect_context_t *)c;

	if(cctx)
	{
		close(cctx->fd);
		return 1;
	}
	return 0;
}

int sandbox_socket_shutdown(void * l)
{
	struct sandbox_socket_listen_context_t * lctx = (struct sandbox_socket_listen_context_t *)l;

	if(lctx)
	{
		close(lctx->fd);
		return 1;
	}
	return 0;
}
