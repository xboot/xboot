/*
 * kernel/shell/parser.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <shell/parser.h>

enum paser_state_t {
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
struct parser_state_transition_t
{
	/* The state that is looked up. */
	enum paser_state_t from_state;

	/* The next state, determined by FROM_STATE and INPUT. */
	enum paser_state_t to_state;

	/* The input that will determine the next state from FROM_STATE. */
	char input;

	/* If set to 1, the input is valid and should be used. */
	int keep_value;
};

/*
 * All the possible state transitions on the command line.  If a
 * transition can not be found, it is assumed that there is no
 * transition and keep_value is assumed to be 1.
 */
static struct parser_state_transition_t state_transitions[] = {
	{ PARSER_STATE_TEXT, PARSER_STATE_QUOTE, '\'', 0},
	{ PARSER_STATE_TEXT, PARSER_STATE_DQUOTE, '\"', 0},
	{ PARSER_STATE_TEXT, PARSER_STATE_VAR, '$', 0},
	{ PARSER_STATE_TEXT, PARSER_STATE_ESC, '\\', 0},
	{ PARSER_STATE_ESC, PARSER_STATE_TEXT, 0, 1},
	{ PARSER_STATE_QUOTE, PARSER_STATE_TEXT, '\'', 0},
	{ PARSER_STATE_DQUOTE, PARSER_STATE_TEXT, '\"', 0},
	{ PARSER_STATE_DQUOTE,PARSER_STATE_QVAR, '$', 0},
	{ PARSER_STATE_VAR, PARSER_STATE_VARNAME2, '{', 0},
	{ PARSER_STATE_VAR, PARSER_STATE_VARNAME, 0, 1},
	{ PARSER_STATE_VARNAME, PARSER_STATE_TEXT, ' ', 1},
	{ PARSER_STATE_VARNAME2, PARSER_STATE_TEXT, '}', 0},
	{ PARSER_STATE_QVAR, PARSER_STATE_QVARNAME2, '{', 0},
	{ PARSER_STATE_QVAR, PARSER_STATE_QVARNAME, 0, 1},
	{ PARSER_STATE_QVARNAME, PARSER_STATE_DQUOTE, ' ', 1},
	{ PARSER_STATE_QVARNAME, PARSER_STATE_TEXT, '\"', 0},
	{ PARSER_STATE_QVARNAME2, PARSER_STATE_DQUOTE, '}', 0},
	{ 0, 0, 0, 0}
};

/*
 * Determines the state following STATE, determined by C.
 */
static enum paser_state_t get_parser_state(enum paser_state_t state, char c, char * result)
{
	struct parser_state_transition_t *transition;
	struct parser_state_transition_t *next_match = 0;
	struct parser_state_transition_t default_transition;
	int found = 0;

	default_transition.to_state = state;
	default_transition.keep_value = 1;

	/* Look for a good translation.  */
	for (transition = state_transitions; transition->from_state; transition++)
	{
		/* An exact match was found, use it. */
		if (transition->from_state == state && transition->input == c)
		{
			found = 1;
			break;
		}

		/* A less perfect match was found, use this one if no exact match can be found */
		if (transition->from_state == state && transition->input == 0)
			next_match = transition;
	}

	if(!found)
	{
		if(next_match)
			transition = next_match;
		else
			transition = &default_transition;
	}

	if (transition->keep_value)
		*result = c;
	else
		*result = 0;
	return transition->to_state;
}

/*
 * Check the the parser state, return true for var,
 * otherwise return false
 */
static bool_t is_varstate(enum paser_state_t s)
{
	if(s == PARSER_STATE_VARNAME || s == PARSER_STATE_VARNAME2 ||
		s == PARSER_STATE_QVARNAME || s == PARSER_STATE_QVARNAME2)
		return TRUE;
	else
		return FALSE;
}

/*
 * Parser command line.
 * The cmdline's last character must be a space for running right
 */
bool_t parser(const char * cmdline, int * argc, char *** argv, char ** pos)
{
	enum paser_state_t state = PARSER_STATE_TEXT;
	enum paser_state_t newstate;
	char *rd = (char *)cmdline;
	char c, *args, *val;
	char *buffer, *bp;
	char *varname, *vp;
	int i;

	*argc = 0;
	*pos = 0;

	bp = buffer = malloc(SZ_4K);
	if(!buffer)
	{
		*argc = 0;
		return FALSE;
	}

	vp = varname = malloc(SZ_256);
	if(!varname)
	{
		*argc = 0;
		return FALSE;
	}

	do{
		if( !(*rd) )
		{
			break;
		}

		for(; *rd; rd++)
		{
			newstate = get_parser_state(state, *rd, &c);

			/*
			 * If a variable was being processed and this character does
			 * not describe the variable anymore, write the variable to the buffer
			 */
			if(is_varstate (state) && !is_varstate (newstate))
			{
			    *(vp++) = '\0';
			    val = getenv(varname);
			    vp = varname;
			    if(val)
			    {
			    	for(; *val; val++)
			    		*(bp++) = *val;
			    }
			}

			if(is_varstate(newstate))
			{
				if(c)
					*(vp++) = c;
			}
			else
			{
				if(newstate == PARSER_STATE_TEXT && state != PARSER_STATE_ESC && c == ' ')
				{
					/* Don't add more than one argument if multiple spaces are used. */
					if(bp != buffer && *(bp - 1))
					{
						*(bp++) = '\0';
						(*argc)++;
					}
				}
				else if(newstate == PARSER_STATE_TEXT && state != PARSER_STATE_ESC && c == ';')
				{
					if(bp != buffer && *(bp - 1))
					{
						*(bp++) = '\0';
						(*argc)++;
					}
					*pos = rd+1;
					break;
				}
				else if(c)
				{
					*(bp++) = c;
				}
			}
			state = newstate;
		}
	} while (state != PARSER_STATE_TEXT && !is_varstate(state));

	*(bp++) = '\0';

	/* A special case for when the last character was part of a variable. */
	if(is_varstate(state) && !is_varstate (PARSER_STATE_TEXT))
	{
	    *(vp++) = '\0';
	    val = getenv(varname);
	    vp = varname;
	    if(val)
	    {
	    	for(; *val; val++)
	    		*(bp++) = *val;
	    }
	}

	/* Reserve memory for the return values.  */
	args = malloc(bp - buffer);
	if(!args)
	{
		*argc = 0;
		return FALSE;
	}

	memcpy(args, buffer, bp - buffer);

	*argv = malloc (sizeof(char *) * (*argc + 1));
	if (! *argv)
	{
		*argc = 0;
		free(args);
		return FALSE;
	}

	/* The arguments are separated with 0's, setup argv so it points to the right values.  */
	bp = args;
	for (i = 0; i < *argc; i++)
	{
		(*argv)[i] = bp;
		while (*bp)
			bp++;
		bp++;
	}
	(*argv)[i] = 0;

	free(buffer);
	free(varname);

	return TRUE;
}
