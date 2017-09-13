#include <xsync.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    xsync [-d device] [-b baud] <path>\r\n");
}

static void xsync_show_progress(const char * filename, int percent)
{
	int l = 50;
	int a = l * percent / 100;
	int b = l - a;
	int i;

	printf("\r[%3d%%][", percent);
    for(i = 0; i < a; i++)
    {
    	if((b > 0) && (i == a - 1))
    		printf("%c", '>');
    	else
    		printf("%c", '=');
    }
    for(i = 0; i < b; i++)
    	printf("%c", '.');
	printf("] - %s", filename);
	fflush(stdout);
}

static void xsync_transfer_file(struct interface_t * iface, const char * filename)
{
	struct packet_t request, response;
	char buf[PACKET_DATA_MAX];
	uint32_t crc = 0;
	int filesize, len = 0;
	int fd;
	int n;
	int result, retry;

	xsync_show_progress(filename, 0);

    fd = open(filename, O_RDONLY);
    if(fd < 0)
    	return;

    filesize = lseek(fd, 0, SEEK_END);
    if(filesize < 0)
    {
        close(fd);
        return;
    }

    if(lseek(fd, 0, SEEK_SET) != 0)
    {
        close(fd);
        return;
    }

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		crc = crc32(crc, (const uint8_t *)buf, n);
	}

    if(lseek(fd, 0, SEEK_SET) != 0)
    {
        close(fd);
        return;
    }

	buf[0] = (crc >> 24) & 0xff;
	buf[1] = (crc >> 16) & 0xff;
	buf[2] = (crc >>  8) & 0xff;
	buf[3] = (crc >>  0) & 0xff;
	n = sprintf(&buf[4], "%s", filename) + 4;
	retry = 0;
	packet_init(&request, XSYNC_COMMAND_START, (uint8_t *)buf, n);
	do {
		result = packet_transfer(iface, &request, &response, 1000);
    } while((result < 0) && (++retry < 3));
	if(result < 0)
	{
        close(fd);
        return;
	}
	else if(response.data[0] == 0)
	{
	    close(fd);
	    return;
	}
	else if(response.data[0] == 1)
	{
	    close(fd);
	    xsync_show_progress(filename, 100);
	    return;
	}

	result = 0;
	while(((n = read(fd, buf, sizeof(buf))) > 0) && !(result < 0))
	{
		retry = 0;
		packet_init(&request, XSYNC_COMMAND_TRANSFER, (uint8_t *)buf, n);
		do {
			result = packet_transfer(iface, &request, &response, 1000);
	    } while((result < 0) && (++retry < 3));
		if(!(result < 0))
		{
			len += n;
			xsync_show_progress(filename, len * 100 / filesize);
		}
	}
	if(result < 0)
	{
        close(fd);
        return;
	}

	retry = 0;
	packet_init(&request, XSYNC_COMMAND_STOP, 0, 0);
	do {
		result = packet_transfer(iface, &request, &response, 1000);
    } while((result < 0) && (++retry < 3));
	if(result < 0)
	{
        close(fd);
        return;
	}

    close(fd);
    xsync_show_progress(filename, 100);
    return;
}

int main(int argc, char * argv[])
{
	struct packet_t request, response;
	struct interface_t * iface;
	char * path = ".";
	char * device = "/dev/ttyUSB0";
	int baud = 115200;
	int result, retry;
	int i;

	for(i = 1; i < argc; i++)
	{
		if( !strcmp(argv[i], "-d") && (argc > i + 1) )
		{
			device = argv[i + 1];
			i++;
		}
		else if( !strcmp(argv[i], "-b") && (argc > i+1) )
		{
			baud = (int)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else
		{
			path = argv[i];
		}
	}

	iface = interface_serial_alloc(device, baud);
	if(!iface)
	{
		usage();
		return -1;
	}

	retry = 0;
	packet_init(&request, XSYNC_COMMAND_ALIVE, 0, 0);
	do {
		interface_write(iface, "xsync\n", 6);
		result = packet_transfer(iface, &request, &response, 1000);
    } while((result < 0) && (++retry < 3));
	if(result < 0)
	{
		printf("Can't connect device with serial port [%s, %d]\r\n", device, baud);
		return -1;
	}

	xsync_transfer_file(iface, path);
	printf("\r\n");

	retry = 0;
	packet_init(&request, XSYNC_COMMAND_SYSTEM, (uint8_t *)"./", 7);
	do {
		result = packet_transfer(iface, &request, &response, 1000);
    } while((result < 0) && (++retry < 3));

	interface_serial_free(iface);
	return 0;
}
