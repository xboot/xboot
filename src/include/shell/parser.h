#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <ctype.h>
#include <string.h>

enum paser_state {
	PARSER_STATE_TEXT = 1,
	PARSER_STATE_ESC,
	PARSER_STATE_QUOTE,
	PARSER_STATE_DQUOTE,
	PARSER_STATE_VAR,
	PARSER_STATE_VARNAME,
	PARSER_STATE_VARNAME2,
	PARSER_STATE_QVAR,
	PARSER_STATE_QVARNAME,
	PARSER_STATE_QVARNAME2
};

/* A single state transition. */
struct parser_state_transition
{
	/* The state that is looked up. */
	enum paser_state from_state;

	/* The next state, determined by FROM_STATE and INPUT. */
	enum paser_state to_state;

	/* The input that will determine the next state from FROM_STATE. */
	char input;

	/* If set to 1, the input is valid and should be used. */
	int keep_value;
};

bool_t parser(const char * cmdline, int * argc, char *** argv, char ** pos);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_H__ */
