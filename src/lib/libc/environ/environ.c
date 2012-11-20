/*
 * libc/environ/environ.c
 */

#include <types.h>
#include <malloc.h>
#include <string.h>
#include <xml.h>
#include <fs/fileio.h>
#include <runtime.h>
#include <environ.h>

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

bool_t saveenv(char * file)
{
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * p;
	struct xml * root, * env;
	char * str;
	int fd;

	root = xml_new("environment");
	if(!root)
		return FALSE;

	for(p = environ->next; p != environ; p = p->next)
	{
		env = xml_add_child(root, "env", 0);
		xml_set_txt(env, p->content);
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
