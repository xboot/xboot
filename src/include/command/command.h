#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct command_t
{
	/* Command name */
	const char * name;

	/* Command description */
	const char * desc;

	/* Command usage function */
	void (*usage)(void);

	/* Command exec function */
	int (*exec)(int argc, char ** argv);
};

struct command_list_t
{
	struct command_t * cmd;
	struct list_head entry;
};

extern struct command_list_t __command_list;

struct command_t * search_command(const char * name);
bool_t register_command(struct command_t * cmd);
bool_t unregister_command(struct command_t * cmd);
int total_command_number(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_H__ */
