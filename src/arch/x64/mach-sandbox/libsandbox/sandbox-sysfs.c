#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sandbox.h>

int sandbox_sysfs_access(const char * path)
{
	if(access(path, F_OK) == 0)
		return 0;
	return -1;
}

int sandbox_sysfs_read_value(const char * path, int * v)
{
	FILE * fd;
	fd = fopen(path, "r");
	if(!fd)
		return -1;
	fscanf(fd, "%d", v);
	fclose(fd);
	return 0;
}

int sandbox_sysfs_write_value(const char * path, const int v)
{
	FILE * fd;
	fd = fopen(path, "w");
	if(!fd)
		return -1;
	fprintf(fd, "%d", v);
	fclose(fd);
	return 0;
}

int sandbox_sysfs_read_string(const char * path, char * s)
{
	FILE * fd;
	fd = fopen(path, "r");
	if(!fd)
		return -1;
	fscanf(fd, "%s", s);
	fclose(fd);
	return 0;
}

int sandbox_sysfs_write_string(const char * path, const char * s)
{
	FILE * fd;
	fd = fopen(path, "w");
	if(!fd)
		return -1;
	fprintf(fd, "%s", s);
	fclose(fd);
	return 0;
}
