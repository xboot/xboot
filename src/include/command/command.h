#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <configs.h>
#include <default.h>
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
	x_s32 (*func)(x_s32 argc, const x_s8 ** argv);

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


x_bool command_register(struct command * cmd);
x_bool command_unregister(struct command * cmd);
struct command * command_search(const char * name);
x_s32 command_number(void);

#endif /* __COMMAND_H__ */
