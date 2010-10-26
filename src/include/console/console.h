#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <configs.h>
#include <default.h>

/*
 * define the struct of input console
 */
struct console_input
{
	/* the name of input console */
	char * name;

	/* console read */
	x_s32 (*read)(struct console_input * input, x_u8 * buf, x_s32 count);

	/* private data */
	void * priv;
};

#endif /* __CONSOLE_H__ */
