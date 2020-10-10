#include <x.h>
#include <sandbox.h>

int sandbox_sysfs_access(const char * path, const char * mode)
{
	int m = F_OK;

	while(*mode)
	{
		switch(*mode++)
		{
		case 'r':
			m |= R_OK;
			break;
		case 'w':
			m |= W_OK;
			break;
		case 'x':
			m |= X_OK;
			break;
		default:
			break;
		}
	}

	if(access(path, m) == 0)
		return 1;
	return 0;
}

int sandbox_sysfs_read_value(const char * path, int * v)
{
	FILE * fd;
	int ret = 0;
	fd = fopen(path, "r");
	if(!fd)
		return ret;
	if(fscanf(fd, "%d", v) == 1)
		ret = 1;
	fclose(fd);
	return ret;
}

int sandbox_sysfs_write_value(const char * path, const int v)
{
	FILE * fd;
	fd = fopen(path, "w");
	if(!fd)
		return 0;
	fprintf(fd, "%d", v);
	fclose(fd);
	return 1;
}

int sandbox_sysfs_read_string(const char * path, char * s)
{
	FILE * fd;
	int ret = 0;
	fd = fopen(path, "r");
	if(!fd)
		return ret;
	if(fscanf(fd, "%s", s) == 1)
		ret = 1;
	fclose(fd);
	return ret;
}

int sandbox_sysfs_write_string(const char * path, const char * s)
{
	FILE * fd;
	fd = fopen(path, "w");
	if(!fd)
		return 0;
	fprintf(fd, "%s", s);
	fclose(fd);
	return 1;
}
