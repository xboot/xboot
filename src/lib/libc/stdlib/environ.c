/*
 * libc/stdlib/environ.c
 */

#include <types.h>
#include <malloc.h>
#include <string.h>
#include <xml.h>
#include <fs/fileio.h>
#include <stdlib.h>

static char ** __environ = 0;

char *** __environ_location(void)
{
	return &__environ;
}

/*
 * load environment variable
 */
bool_t loadenv(char * file)
{
	struct xml * root, * env;

	root = xml_parse_file(file);
	if(!root || !root->name)
		return FALSE;

	if(strcmp(root->name, "environment") != 0)
	{
		xml_free(root);
		return FALSE;
	}

	clearenv();

	for(env = xml_child(root, "env"); env; env = env->next)
	{
		if(env->txt)
			putenv(env->txt);
	}

	xml_free(root);
	return TRUE;
}

/*
 * save environment variable
 */
bool_t saveenv(char * file)
{
	struct xml * root, * env;
	char * str;
	char ** ep;
	int fd;

	root = xml_new("environment");
	if(!root)
		return FALSE;

	for(ep = environ; *ep; ep++)
	{
		env = xml_add_child(root, "env", 0);
		xml_set_txt(env, *ep);
	}

	str = xml_toxml(root);
	if(!str)
	{
		xml_free(root);
		return FALSE;
	}

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		free(str);
		xml_free(root);
		return FALSE;
	}

	write(fd, str, strlen(str));
	close(fd);

	free(str);
	xml_free(root);

	return TRUE;
}
