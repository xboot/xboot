#include <x.h>
#include <sandbox.h>

struct sandbox_t {
	struct
	{
		void * buffer;
		size_t size;
	} heap;
	struct
	{
		void * buffer;
		size_t size;
	} dma;
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
		"  --heap  <SIZE> Setting heap memory size, The default is 256MB.\r\n"
		"  --dma   <SIZE> Setting dma memory size, The default is 32MB.\r\n"
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

void sandbox_init(int argc, char * argv[])
{
	size_t heap_size = 256 * 1024 * 1024;
	size_t dma_size = 32 * 1024 * 1024;
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
		else if(!strcmp(argv[i], "--heap") && (argc > i + 1))
		{
			heap_size = strtoul(argv[++i], NULL, 0);
			if(heap_size <= 0)
				heap_size = 256 * 1024 * 1024;
		}
		else if(!strcmp(argv[i], "--dma") && (argc > i + 1))
		{
			dma_size = strtoul(argv[++i], NULL, 0);
			if(dma_size <= 0)
				dma_size = 32 * 1024 * 1024;
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

	sandbox.heap.size = heap_size;
	sandbox.heap.buffer = memalign(64, sandbox.heap.size);
	if(!sandbox.heap.buffer)
	{
		printf("ERROR: Can't alloc heap memory.\r\n");
		if(sandbox.dtree.buffer)
			free(sandbox.dtree.buffer);
		if(sandbox.application)
			free(sandbox.application);
		exit(-1);
	}

	sandbox.dma.size = dma_size;
	sandbox.dma.buffer = memalign(64, sandbox.dma.size);
	if(!sandbox.dma.buffer)
	{
		printf("ERROR: Can't alloc dma memory.\r\n");
		if(sandbox.heap.buffer)
			free(sandbox.heap.buffer);
		if(sandbox.dtree.buffer)
			free(sandbox.dtree.buffer);
		if(sandbox.application)
			free(sandbox.application);
		exit(-1);
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
}

void sandbox_exit(void)
{
	if(sandbox.heap.buffer)
		free(sandbox.heap.buffer);
	if(sandbox.dma.buffer)
		free(sandbox.dma.buffer);
	if(sandbox.dtree.buffer)
		free(sandbox.dtree.buffer);
	if(sandbox.application)
		free(sandbox.application);

	fflush(stdout);
	tcsetattr(0, TCSANOW, &tconfig);

	exit(0);
}

void * sandbox_get_heap_buffer(void)
{
	return sandbox.heap.buffer;
}

size_t sandbox_get_heap_size(void)
{
	return sandbox.heap.size;
}

void * sandbox_get_dma_buffer(void)
{
	return sandbox.dma.buffer;
}

size_t sandbox_get_dma_size(void)
{
	return sandbox.dma.size;
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
