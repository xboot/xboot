#include <interface.h>

uint64_t interface_time(struct interface_t * iface)
{
	if(iface && iface->time)
		return iface->time(iface);
	return 0;
}

ssize_t interface_read(struct interface_t * iface, void * buf, size_t len)
{
	if(iface && iface->read)
		return iface->read(iface, buf, len);
	return 0;
}

ssize_t interface_write(struct interface_t * iface, void * buf, size_t len)
{
	if(iface && iface->write)
		return iface->write(iface, buf, len);
	return 0;
}

struct serial_ctx_t {
	int fd;
};

static uint64_t serial_time(struct interface_t * iface)
{
	struct timeval time;

	gettimeofday(&time, 0);
	return (uint64_t)(time.tv_sec * 1000 + time.tv_usec / 1000);
}

static ssize_t serial_read(struct interface_t * iface, void * buf, size_t len)
{
	struct serial_ctx_t * ctx = (struct serial_ctx_t *)iface->priv;
	return read(ctx->fd, buf, len);
}

static ssize_t serial_write(struct interface_t * iface, void * buf, size_t len)
{
	struct serial_ctx_t * ctx = (struct serial_ctx_t *)iface->priv;
	return write(ctx->fd, buf, len);
}

static speed_t serial_baud(int baud)
{
	switch(baud)
	{
	case 0:
		return B0;
	case 50:
		return B50;
	case 75:
		return B75;
	case 110:
		return B110;
	case 134:
		return B134;
	case 150:
		return B150;
	case 200:
		return B200;
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 1800:
		return B1800;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	case 2000000:
		return B2000000;
	case 2500000:
		return B2500000;
	case 3000000:
		return B3000000;
	case 3500000:
		return B3500000;
	case 4000000:
		return B4000000;
	default:
		return B115200;
	}
}

struct interface_t * interface_serial_alloc(const char * device, int baud)
{
	struct interface_t * iface;
	struct serial_ctx_t * ctx;
	struct termios cfg;
	speed_t speed = serial_baud(baud);
	int fd;

	fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
		return 0;

	if(tcgetattr(fd, &cfg))
	{
		close(fd);
		return 0;
	}
	cfmakeraw(&cfg);
	cfsetispeed(&cfg, speed);
	cfsetospeed(&cfg, speed);

	cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 5;
	if(tcsetattr(fd, TCSANOW, &cfg))
	{
		close(fd);
		return 0;
	}

	iface = malloc(sizeof(struct interface_t));
	if(!iface)
		return 0;

	ctx = malloc(sizeof(struct serial_ctx_t));
	if(!ctx)
	{
		free(iface);
		return 0;
	}

	ctx->fd = fd;
	iface->time = serial_time;
	iface->read = serial_read;
	iface->write = serial_write;
	iface->priv = ctx;

	return iface;
}

void interface_serial_free(struct interface_t * iface)
{
	if(iface)
	{
		struct serial_ctx_t * ctx = (struct serial_ctx_t *)iface->priv;
		close(ctx->fd);
	}
}
