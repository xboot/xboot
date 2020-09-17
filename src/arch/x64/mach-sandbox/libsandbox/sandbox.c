#include <x.h>
#include <sandbox.h>

struct sandbox_t {
	struct {
		void * buffer;
		size_t size;
	} dtree;
	char * application;
};
static struct sandbox_t sandbox;
static struct termios tconfig;

static void print_usage(void)
{
	printf(
		"Usage: xboot [OPTIONS] <application>\r\n"
		"Options:\r\n"
		"  --help         Print help information.\r\n"
		"  --dtree <FILE> Start xboot using the specified device tree in json format.\r\n"
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
	if(!sandbox_file_isfile(filename))
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

static void set_gperf_status(int signum)
{
	static int flag = 0;
	if(signum == SIGUSR2)
	{
		if(!flag)
		{
			flag = 1;
			ProfilerStart("xboot.prof");
			printf("profiler start\r\n");
		}
		else
		{
			flag = 0;
			ProfilerStop();
			printf("profiler stop\r\n");
		}
	}
}

void sandbox_init(int argc, char * argv[])
{
	struct termios ta;
	char path[PATH_MAX];
	char * buf;
	size_t len;
	int i, idx = 0;

	memset(&sandbox, 0, sizeof(struct sandbox_t));
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			print_usage();
		}
		else if(!strcmp(argv[i], "--dtree") && (argc > i + 1))
		{
			if(sandbox_file_isfile(argv[++i]))
			{
				if((len = file_read_to_memory(argv[i], &buf)) > 0)
				{
					sandbox.dtree.buffer = buf;
					sandbox.dtree.size = len;
				}
			}
			else
			{
				print_usage();
			}
		}
		else if(*argv[i] == '-')
		{
			print_usage();
		}
		else
		{
			if(idx == 0)
			{
				if(sandbox_file_isdir(argv[i]) || sandbox_file_isfile(argv[i]))
					sandbox.application = strdup(realpath(argv[i], path));
			}
			else
			{
				print_usage();
			}
			idx++;
		}
	}

	if(geteuid() != 0)
		printf("WARNING: Running without root permission.(%d)\r\n", geteuid());

	if((readlink("/proc/self/exe", path, sizeof(path)) <= 0) || (chdir(dirname(path)) != 0))
		printf("WARNING: Can't change working directory.(%s)\r\n", getcwd(path, sizeof(path)));

	tcgetattr(0, &tconfig);
	ta = tconfig;
	ta.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
	ta.c_iflag &= (~IXON & ~ICRNL);
	ta.c_oflag |= (ONLCR);
	ta.c_cc[VMIN] = 1;
	ta.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &ta);

	SDL_Init(SDL_INIT_EVERYTHING);
	signal(SIGUSR2, set_gperf_status);
}

void sandbox_exit(void)
{
	if(sandbox.dtree.buffer)
		free(sandbox.dtree.buffer);
	if(sandbox.application)
		free(sandbox.application);

	fflush(stdout);
	tcsetattr(0, TCSANOW, &tconfig);

	SDL_Quit();
	exit(0);
}

void * sandbox_get_dtree_buffer(void)
{
	return sandbox.dtree.buffer;
}

size_t sandbox_get_dtree_size(void)
{
	return sandbox.dtree.size;
}

char * sandbox_get_application(void)
{
	return sandbox.application;
}
