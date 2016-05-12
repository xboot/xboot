#include <protocol.h>
#include <serial.h>

int main(int argc, char * argv[])
{
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

	printf("%s, %s, %d\r\n", path, device, baud);
	return 0;
}
