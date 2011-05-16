#ifndef __ENV_H__
#define __ENV_H__


#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the environment variable
 */
struct env {
	char * key;
	char * value;
};

/*
 * the list of environment variable.
 */
struct env_list
{
	struct env env;
	struct hlist_node node;
};


char * env_get(const char * key, const char * value);
bool_t env_add(const char * key, const char * value);
bool_t env_remove(const char * key);

bool_t env_load(char * file);
bool_t env_save(char * file);

#endif /* __ENV_H__ */
