#include <x.h>
#include <sandbox.h>

static struct sandbox_t __sandbox;
static struct termios __tconfig;

static void print_usage(void)
{
	printf(
		"Usage: xboot [OPTIONS] <application>\n"
		"Options:\n"
		"  --help        Print help information\n"
		"  --json <FILE> Start xboot with a specified file of device tree using json format\n"
	);
	exit(-1);
}

static size_t file_read_to_memory(const char * filename, char ** buffer)
{
	char * buf;
	size_t len;
	int fd;

	if(!filename || !buffer)
		return 0;

	if(sandbox_file_isfile(filename) != 0)
		return 0;

	fd = sandbox_file_open(filename, "r");
	if(fd < 0)
		return 0;

	len = sandbox_file_length(fd);
	if(len <= 0)
		return 0;

	buf = malloc(len + 1);
	if(!buf)
		return 0;
	memset(buf, 0, len + 1);

	sandbox_file_seek(fd, 0);
	sandbox_file_read(fd, buf, len);
	sandbox_file_close(fd);

	*buffer = buf;
	return len;
}

struct sandbox_t * sandbox_get(void)
{
	return &__sandbox;
}

void sandbox_init(int argc, char * argv[])
{
	struct termios ta;
	char path[PATH_MAX];
	int i, idx = 0;
	char * buf;
	size_t len;

	/* Clear __sandbox */
	memset(&__sandbox, 0, sizeof(struct sandbox_t));

	/* Parse command line */
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			print_usage();
		}
		else if(!strcmp(argv[i], "--json") && (argc > i + 1))
		{
			if(sandbox_file_isfile(argv[++i]) == 0)
			{
				if((len = file_read_to_memory(argv[i], &buf)) > 0)
				{
					__sandbox.json.buffer = buf;
					__sandbox.json.size = len;
				}
			}
			else
			{
				print_usage();
			}
		}
		else
		{
			if(idx == 0)
			{
				if(sandbox_file_isdir(argv[i]) || sandbox_file_isfile(argv[i]))
					__sandbox.app = strdup(realpath(argv[i], path));
			}
			else
			{
				print_usage();
			}
			idx++;
		}
	}

	/* Require root privileges */
	if(geteuid() != 0)
		printf("WARNING: Running without root permission.\r\n");

	/* Save terminal config */
	tcgetattr(0, &__tconfig);
	ta = __tconfig;
	ta.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
	ta.c_iflag &= (~IXON & ~ICRNL);
	ta.c_oflag |= (ONLCR);
	ta.c_cc[VMIN] = 1;
	ta.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &ta);
}

void sandbox_exit(void)
{
	/* Free some memroy */
	if(__sandbox.json.buffer && (__sandbox.json.size > 0))
		free(__sandbox.json.buffer);
	if(__sandbox.app)
		free(__sandbox.app);

	/* Restore terminal config */
	fflush(stdout);
	tcsetattr(0, TCSANOW, &__tconfig);

	/* Program exit */
	exit(0);
}
