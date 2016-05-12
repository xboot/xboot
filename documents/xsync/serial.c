#include <serial.h>

static speed_t serial_get_baud(int baud)
{
	switch (baud)
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
		return -1;
	}
}

int serial_open(const char * device, int baud)
{
	struct termios cfg;
	speed_t speed = serial_get_baud(baud);
	int fd;

	if(speed == -1)
		return -1;

	fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
		return(-1);

	if(tcgetattr(fd, &cfg))
	{
		close(fd);
		return -1;
	}

	cfmakeraw(&cfg);
	cfsetispeed(&cfg, speed);
	cfsetospeed(&cfg, speed);

	/* No block */
    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 5;

	if(tcsetattr(fd, TCSANOW, &cfg))
	{
		close(fd);
		return -1;
	}

	return fd;
}

int serial_close(int fd)
{
	if(close(fd) < 0)
		return -1;
	return 0;
}

ssize_t serial_read_byte(int fd, void * byte)
{
	return read(fd, byte, 1);
}

ssize_t serial_read(int fd, void * buf, size_t len)
{
	return read(fd, buf, len);
}

ssize_t serial_write(int fd, const void * buf, size_t len)
{
	return write(fd, buf, len);
}
