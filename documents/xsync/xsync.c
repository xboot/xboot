#include <xsync.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    xsync [-d device] [-b baud] <path>\r\n");
}

static char * load_file(const char * filename, int * size)
{
    char * buf;
    int sz;
    int fd;

    fd = open(filename, O_RDONLY);
    if(fd < 0)
    	return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0)
    {
        close(fd);
        return 0;
    }

    if(lseek(fd, 0, SEEK_SET) != 0)
    {
        close(fd);
        return 0;
    }

    buf = (char *)malloc(sz);
    if(!buf)
    {
        close(fd);
        return 0;
    }

    if(read(fd, buf, sz) != sz)
    {
    	free(buf);
        close(fd);
        return 0;
    }

    close(fd);
    if(size)
    	*size = sz;
    return buf;
}

static void send_file(const char * filename)
{
	char * buf;
	int size;
	uint32_t crc;
	uint8_t digest[SHA1_DIGEST_SIZE];

	buf = load_file(filename, &size);
	crc = crc32(0, buf, size);
	sha1_hash(buf, size, &digest[0]);

	printf("crc = 0x%08x\r\n", crc);

	int i;
	for(i = 0; i < SHA1_DIGEST_SIZE; i++)
		printf("0x%02x ", digest[i]);

	free(buf);
}

int main(int argc, char * argv[])
{
	struct packet_t request, response;
	struct interface_t * iface;
	char * path = ".";
	char * device = "/dev/ttyUSB0";
	int baud = 115200;
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

	send_file(path);

	packet_init(&request, XSYNC_COMMAND_ALIVE, (uint8_t *)"123", 3);
	if(packet_transform(iface, &request, &response, 3000) == 0)
	{
		printf("response\r\n");
	}

	interface_serial_free(iface);
	return 0;
}
