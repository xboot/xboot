#include <protocol.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    xsync [-d device] [-b baud] <path>\r\n");
}

int main(int argc, char * argv[])
{
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

	interface_serial_free(iface);
	return 0;
}
