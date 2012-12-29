#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * defined the struct of command
 */
struct command
{
	/* the command name */
	const char * name;

	/* the command function */
	int (*func)(int argc, char ** argv);

	/* the command description */
	const char * desc;

	/* usage message, short tips */
	const char * usage;

	/* help message, long tips */
	const char * help;
};

/*
 * the list of console.
 */
struct command_list
{
	struct command * cmd;
	struct list_head entry;
};

bool_t command_register(struct command * cmd);
bool_t command_unregister(struct command * cmd);
struct command * command_search(const char * name);
int command_number(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_H__ */
