#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

struct command_t
{
	/* The command name */
	const char * name;

	/* The command description */
	const char * desc;

	/* The usage function */
	void (*usage)(void);

	/* The exec function */
	int (*exec)(int argc, char ** argv);
};

struct command_list_t
{
	struct command_t * cmd;
	struct list_head entry;
};

bool_t command_register(struct command_t * cmd);
bool_t command_unregister(struct command_t * cmd);
struct command_t * command_search(const char * name);
int command_number(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_H__ */
