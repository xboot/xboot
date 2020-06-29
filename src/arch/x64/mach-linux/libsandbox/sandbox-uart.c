#include <x.h>
#include <sandbox.h>

static speed_t sandbox_uart_baud(int baud)
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

int sandbox_uart_open(const char * dev)
{
	return open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC);
}

int sandbox_uart_close(int fd)
{
	int ret = close(fd);
	return ret < 0 ? 0 : 1;
}

int sandbox_uart_set(int fd, int baud, int data, int parity, int stop)
{
	struct termios cfg;
	speed_t speed = sandbox_uart_baud(baud);

	if(tcgetattr(fd, &cfg))
		return 0;

	cfmakeraw(&cfg);
	cfsetispeed(&cfg, speed);
	cfsetospeed(&cfg, speed);

	cfg.c_cc[VMIN] = 0;
	cfg.c_cc[VTIME] = 5;

	cfg.c_cflag |= CLOCAL;
	cfg.c_cflag |= CREAD;
	cfg.c_cflag &= ~CRTSCTS;

	switch(data)
	{
	case 5:
		cfg.c_cflag &= ~CSIZE;
		cfg.c_cflag |= CS5;
		break;
	case 6:
		cfg.c_cflag &= ~CSIZE;
		cfg.c_cflag |= CS6;
		break;
	case 7:
		cfg.c_cflag &= ~CSIZE;
		cfg.c_cflag |= CS7;
		break;
	case 8:
		cfg.c_cflag &= ~CSIZE;
		cfg.c_cflag |= CS8;
		break;
	default:
		break;
	}

	switch(parity)
	{
	case 0:
		cfg.c_cflag &= ~PARENB;
		break;
	case 1:
		cfg.c_cflag |= PARENB;
		cfg.c_cflag |= PARODD;
		cfg.c_cflag |= INPCK;
		cfg.c_cflag |= ISTRIP;
		break;
	case 2:
		cfg.c_cflag |= PARENB;
		cfg.c_cflag &= ~PARODD;
		cfg.c_cflag |= INPCK;
		cfg.c_cflag |= ISTRIP;
		break;
	default:
		break;
	}

	switch(stop)
	{
	case 1:
		cfg.c_cflag &= ~CSTOPB;
		break;
	case 2:
		cfg.c_cflag |= CSTOPB;
		break;
	case 0:
		break;
	default:
		break;
	}

	cfg.c_oflag &= ~OPOST;
	cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	if(tcsetattr(fd, TCSANOW, &cfg))
		return 0;
	return 1;
}

ssize_t sandbox_uart_read(int fd, void * buf, size_t len)
{
	return read(fd, buf, len);
}

ssize_t sandbox_uart_write(int fd, const void * buf, size_t len)
{
	return write(fd, buf, len);
}
