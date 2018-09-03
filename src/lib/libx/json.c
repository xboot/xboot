/*
 * libx/json.c
 */

#include <xboot.h>
#include <json.h>

enum {
	FLAG_NEXT             = 1 << 0,
	FLAG_REPROC           = 1 << 1,
	FLAG_NEED_COMMA       = 1 << 2,
	FLAG_SEEK_VALUE       = 1 << 3,
	FLAG_ESCAPED          = 1 << 4,
	FLAG_STRING           = 1 << 5,
	FLAG_NEED_COLON       = 1 << 6,
	FLAG_DONE             = 1 << 7,
	FLAG_NUM_NEGATIVE     = 1 << 8,
	FLAG_NUM_ZERO         = 1 << 9,
	FLAG_NUM_E            = 1 << 10,
	FLAG_NUM_E_GOT_SIGN   = 1 << 11,
	FLAG_NUM_E_NEGATIVE   = 1 << 12,
	FLAG_LINE_COMMENT     = 1 << 13,
	FLAG_BLOCK_COMMENT    = 1 << 14,
};

struct json_state_t
{
	unsigned long used_memory;
	unsigned int uint_max;
	unsigned long ulong_max;
	int first_pass;
	const char * ptr;
	unsigned int cur_line;
	unsigned int cur_col;
};

static unsigned char hex_value(char c)
{
	if(isdigit(c))
		return c - '0';

	switch(c)
	{
	case 'a':
	case 'A':
		return 0x0a;
	case 'b':
	case 'B':
		return 0x0b;
	case 'c':
	case 'C':
		return 0x0c;
	case 'd':
	case 'D':
		return 0x0d;
	case 'e':
	case 'E':
		return 0x0e;
	case 'f':
	case 'F':
		return 0x0f;
	default:
		return 0xff;
	}
}

static void * json_alloc(struct json_state_t * state, unsigned long size, int zero)
{
	if((state->ulong_max - state->used_memory) < size)
		return 0;
	return zero ? calloc(1, size) : malloc(size);
}

static int new_value(struct json_state_t * state, struct json_value_t ** top, struct json_value_t ** root, struct json_value_t ** alloc, enum json_type_t type)
{
	struct json_value_t * value;
	int values_size;

	if(!state->first_pass)
	{
		value = *top = *alloc;
		*alloc = (*alloc)->reserved.next_alloc;

		if(!*root)
			*root = value;

		switch(value->type)
		{
		case JSON_ARRAY:
			if(value->u.array.length == 0)
				break;
			if(!(value->u.array.values = (struct json_value_t **)json_alloc(state, value->u.array.length * sizeof(struct json_value_t *), 0)))
				return 0;
			value->u.array.length = 0;
			break;

		case JSON_OBJECT:
			if(value->u.object.length == 0)
				break;
			values_size = sizeof(*value->u.object.values) * value->u.object.length;

			if(!(value->u.object.values = (struct json_object_entry_t *)json_alloc(state, values_size + ((unsigned long)value->u.object.values), 0)))
				return 0;
			value->reserved.object_mem = (*(char **)&value->u.object.values) + values_size;
			value->u.object.length = 0;
			break;

		case JSON_STRING:
			if(!(value->u.string.ptr = (char *)json_alloc(state, (value->u.string.length + 1) * sizeof(char), 0)))
				return 0;
			value->u.string.length = 0;
			break;

		default:
			break;
		};
		return 1;
	}

	if(!(value = (struct json_value_t *)json_alloc(state, sizeof(struct json_value_t), 1)))
		return 0;

	if(!*root)
		*root = value;

	value->type = type;
	value->parent = *top;

	if(*alloc)
		(*alloc)->reserved.next_alloc = value;
	*alloc = *top = value;
	return 1;
}

struct json_value_t * json_parse(const char * json, size_t length, char * errbuf)
{
	char error[128];
	const char * end;
	struct json_value_t * top, * root, * alloc = 0;
	struct json_state_t state = { 0 };
	long flags;
	long num_digits = 0, num_e = 0;
	int64_t num_fraction = 0;

	if (length >= 3 && ((unsigned char)json[0]) == 0xef
					&& ((unsigned char)json[1]) == 0xbb
					&& ((unsigned char)json[2]) == 0xbf)
	{
		json += 3;
		length -= 3;
	}

	error[0] = '\0';
	end = json + length;
	memset(&state.uint_max, 0xff, sizeof(state.uint_max));
	memset(&state.ulong_max, 0xff, sizeof(state.ulong_max));
	state.uint_max -= 8;
	state.ulong_max -= 8;

   for(state.first_pass = 1; state.first_pass >= 0; --state.first_pass)
   {
		unsigned int uchar;
		unsigned char uc_b1, uc_b2, uc_b3, uc_b4;
		char * string = 0;
		unsigned int string_length = 0;

		top = root = 0;
		flags = FLAG_SEEK_VALUE;
		state.cur_line = 1;

		for(state.ptr = json;; ++state.ptr)
		{
			char b = (state.ptr == end ? 0 : *state.ptr);

			if(flags & FLAG_STRING)
			{
				if(!b)
				{
					sprintf(error, "Unexpected EOF in string (at %d:%d)", state.cur_line, state.cur_col);
					goto e_failed;
				}

				if(string_length > state.uint_max)
					goto e_overflow;

				if(flags & FLAG_ESCAPED)
				{
					flags &= ~FLAG_ESCAPED;

					switch(b)
					{
					case 'b':
						if(!state.first_pass)
							string[string_length] = '\b';
						++string_length;
						break;
					case 'f':
						if(!state.first_pass)
							string[string_length] = '\f';
						++string_length;
						break;
					case 'n':
						if(!state.first_pass)
							string[string_length] = '\n';
						++string_length;
						break;
					case 'r':
						if(!state.first_pass)
							string[string_length] = '\r';
						++string_length;
						break;
					case 't':
						if(!state.first_pass)
							string[string_length] = '\t';
						++string_length;
						break;
					case 'u':
						if(end - state.ptr <= 4
								|| (uc_b1 = hex_value(*++state.ptr)) == 0xff
								|| (uc_b2 = hex_value(*++state.ptr)) == 0xff
								|| (uc_b3 = hex_value(*++state.ptr)) == 0xff
								|| (uc_b4 = hex_value(*++state.ptr)) == 0xff) {
							sprintf(error, "Invalid character value `%c` (at %d:%d)", b, state.cur_line, state.cur_col);
							goto e_failed;
						}

						uc_b1 = (uc_b1 << 4) | uc_b2;
						uc_b2 = (uc_b3 << 4) | uc_b4;
						uchar = (uc_b1 << 8) | uc_b2;

						if((uchar & 0xf800) == 0xd800)
						{
							unsigned int uchar2;

							if(end - state.ptr <= 6
									|| (*++state.ptr) != '\\'
									|| (*++state.ptr) != 'u'
									|| (uc_b1 = hex_value(*++state.ptr)) == 0xff
									|| (uc_b2 = hex_value(*++state.ptr)) == 0xff
									|| (uc_b3 = hex_value(*++state.ptr)) == 0xff
									|| (uc_b4 = hex_value(*++state.ptr)) == 0xff)
							{
								sprintf(error, "Invalid character value `%c` (at %d:%d)", b, state.cur_line, state.cur_col);
								goto e_failed;
							}

							uc_b1 = (uc_b1 << 4) | uc_b2;
							uc_b2 = (uc_b3 << 4) | uc_b4;
							uchar2 = (uc_b1 << 8) | uc_b2;
							uchar = 0x010000 | ((uchar & 0x3ff) << 10) | (uchar2 & 0x3ff);
						}

						if(sizeof(char) >= sizeof(unsigned int) || (uchar <= 0x7f))
						{
							if(!state.first_pass)
								string[string_length] = (char )uchar;
							++string_length;
							break;
						}

						if(uchar <= 0x7ff)
						{
							if(state.first_pass)
								string_length += 2;
							else
							{
								string[string_length++] = 0xc0 | (uchar >> 6);
								string[string_length++] = 0x80 | (uchar & 0x3f);
							}
							break;
						}

						if(uchar <= 0xffff)
						{
							if(state.first_pass)
								string_length += 3;
							else
							{
								string[string_length++] = 0xe0 | (uchar >> 12);
								string[string_length++] = 0x80 | ((uchar >> 6) & 0x3f);
								string[string_length++] = 0x80 | (uchar & 0x3f);
							}
							break;
						}

						if(state.first_pass)
							string_length += 4;
						else {
							string[string_length++] = 0xf0 | (uchar >> 18);
							string[string_length++] = 0x80 | ((uchar >> 12) & 0x3f);
							string[string_length++] = 0x80 | ((uchar >> 6) & 0x3f);
							string[string_length++] = 0x80 | (uchar & 0x3f);
						}
						break;

					default:
						if(!state.first_pass)
							string[string_length] = b;
						++string_length;
					};
					continue;
				}

				if(b == '\\')
				{
					flags |= FLAG_ESCAPED;
					continue;
				}

				if(b == '"')
				{
					if(!state.first_pass)string[string_length] = 0;
					flags &= ~FLAG_STRING;
					string = 0;

					switch(top->type)
					{
					case JSON_STRING:
						top->u.string.length = string_length;
						flags |= FLAG_NEXT;
						break;

					case JSON_OBJECT:
						if(state.first_pass)
							(*(char **)&top->u.object.values) += string_length + 1;
						else
						{
							top->u.object.values[top->u.object.length].name = (char *)top->reserved.object_mem;
							top->u.object.values[top->u.object.length].name_length = string_length;
							(*(char **)&top->reserved.object_mem) += string_length + 1;
						}
						flags |= FLAG_SEEK_VALUE | FLAG_NEED_COLON;
						continue;

					default:
						break;
					};
				}
				else
				{
					if(!state.first_pass)
						string[string_length] = b;
					++string_length;
					continue;
				}
			}

			if(flags & (FLAG_LINE_COMMENT | FLAG_BLOCK_COMMENT))
			{
				if(flags & FLAG_LINE_COMMENT)
				{
					if(b == '\r' || b == '\n' || !b)
					{
						flags &= ~FLAG_LINE_COMMENT;
						--state.ptr;
					}
					continue;
				}

				if(flags & FLAG_BLOCK_COMMENT)
				{
					if(!b)
					{
						sprintf(error, "%d:%d: Unexpected EOF in block comment", state.cur_line, state.cur_col);
						goto e_failed;
					}
					if(b == '*' && state.ptr < (end - 1) && state.ptr[1] == '/')
					{
						flags &= ~FLAG_BLOCK_COMMENT;
						++state.ptr;
					}
					continue;
				}
			}
			else if(b == '/')
			{
				if(!(flags & (FLAG_SEEK_VALUE | FLAG_DONE)) && top->type != JSON_OBJECT)
				{
					sprintf(error, "%d:%d: Comment not allowed here", state.cur_line, state.cur_col);
					goto e_failed;
				}

				if(++state.ptr == end)
				{
					sprintf(error, "%d:%d: EOF unexpected", state.cur_line, state.cur_col);
					goto e_failed;
				}

				switch(b = *state.ptr)
				{
				case '/':
					flags |= FLAG_LINE_COMMENT;
					continue;

				case '*':
					flags |= FLAG_BLOCK_COMMENT;
					continue;

				default:
					sprintf(error, "%d:%d: Unexpected `%c` in comment opening sequence", state.cur_line, state.cur_col, b);
					goto e_failed;
				};
			}

			if(flags & FLAG_DONE)
			{
				if(!b)
					break;

				switch(b)
				{
				case '\n':
					++state.cur_line;
					state.cur_col = 0;
				case ' ':
				case '\t':
				case '\r':
					continue;

				default:
					sprintf(error, "%d:%d: Trailing garbage: `%c`", state.cur_line, state.cur_col, b);
					goto e_failed;
				};
			}

			if(flags & FLAG_SEEK_VALUE)
			{
				switch(b)
				{
				case '\n':
					++state.cur_line;
					state.cur_col = 0;
				case ' ':
				case '\t':
				case '\r':
					continue;

				case ']':
					if(top && top->type == JSON_ARRAY)
						flags = (flags & ~(FLAG_NEED_COMMA | FLAG_SEEK_VALUE)) | FLAG_NEXT;
					else
					{
						sprintf(error, "%d:%d: Unexpected ]", state.cur_line, state.cur_col);
						goto e_failed;
					}
					break;

				default:
					if(flags & FLAG_NEED_COMMA)
					{
						if(b == ',')
						{
							flags &= ~FLAG_NEED_COMMA;
							continue;
						}
						else
						{
							sprintf(error, "%d:%d: Expected , before %c", state.cur_line, state.cur_col, b);
							goto e_failed;
						}
					}

					if(flags & FLAG_NEED_COLON)
					{
						if(b == ':')
						{
							flags &= ~FLAG_NEED_COLON;
							continue;
						}
						else
						{
							sprintf(error, "%d:%d: Expected : before %c", state.cur_line, state.cur_col, b);
							goto e_failed;
						}
					}

					flags &= ~FLAG_SEEK_VALUE;
					switch(b)
					{
					case '{':
						if(!new_value(&state, &top, &root, &alloc, JSON_OBJECT))
							goto e_alloc_failure;
						continue;

					case '[':
						if(!new_value(&state, &top, &root, &alloc, JSON_ARRAY))
							goto e_alloc_failure;
						flags |= FLAG_SEEK_VALUE;
						continue;

					case '"':
						if(!new_value(&state, &top, &root, &alloc, JSON_STRING))
							goto e_alloc_failure;
						flags |= FLAG_STRING;
						string = top->u.string.ptr;
						string_length = 0;
						continue;

					case 't':
						if((end - state.ptr) < 3
								|| *(++state.ptr) != 'r'
								|| *(++state.ptr) != 'u'
								|| *(++state.ptr) != 'e')
						{
							goto e_unknown_value;
						}
						if(!new_value(&state, &top, &root, &alloc, JSON_BOOLEAN))
							goto e_alloc_failure;

						top->u.boolean = 1;
						flags |= FLAG_NEXT;
						break;

					case 'f':
						if((end - state.ptr) < 4
								|| *(++state.ptr) != 'a'
								|| *(++state.ptr) != 'l'
								|| *(++state.ptr) != 's'
								|| *(++state.ptr) != 'e')
						{
							goto e_unknown_value;
						}
						if(!new_value(&state, &top, &root, &alloc, JSON_BOOLEAN))
							goto e_alloc_failure;
						flags |= FLAG_NEXT;
						break;

					case 'n':
						if((end - state.ptr) < 3
								|| *(++state.ptr) != 'u'
								|| *(++state.ptr) != 'l'
								|| *(++state.ptr) != 'l')
						{
							goto e_unknown_value;
						}
						if(!new_value(&state, &top, &root, &alloc, JSON_NULL))
							goto e_alloc_failure;
						flags |= FLAG_NEXT;
						break;

					default:
						if(isdigit(b) || b == '-')
						{
							if(!new_value(&state, &top, &root, &alloc, JSON_INTEGER))
								goto e_alloc_failure;
							if(!state.first_pass)
							{
								while(isdigit(b) || b == '+' || b == '-' || b == 'e' || b == 'E' || b == '.')
								{
									if((++state.ptr) == end)
									{
										b = 0;
										break;
									}
									b = *state.ptr;
								}
								flags |= FLAG_NEXT | FLAG_REPROC;
								break;
							}

							flags &= ~(FLAG_NUM_NEGATIVE
									| FLAG_NUM_E
									| FLAG_NUM_E_GOT_SIGN
									| FLAG_NUM_E_NEGATIVE
									| FLAG_NUM_ZERO);
							num_digits = 0;
							num_fraction = 0;
							num_e = 0;

							if(b != '-')
							{
								flags |= FLAG_REPROC;
								break;
							}
							flags |= FLAG_NUM_NEGATIVE;
							continue;
						}
						else
						{
							sprintf(error, "%d:%d: Unexpected %c when seeking value", state.cur_line, state.cur_col, b);
							goto e_failed;
						}
					}
				}
			}
			else
			{
				switch(top->type)
				{
				case JSON_OBJECT:
					switch(b)
					{
					case '\n':
						++state.cur_line;
						state.cur_col = 0;
					case ' ':
					case '\t':
					case '\r':
						continue;

					case '"':
						if(flags & FLAG_NEED_COMMA)
						{
							sprintf(error, "%d:%d: Expected , before \"", state.cur_line, state.cur_col);
							goto e_failed;
						}
						flags |= FLAG_STRING;
						string = (char *) top->reserved.object_mem;
						string_length = 0;
						break;

					case '}':
						flags = (flags & ~FLAG_NEED_COMMA) | FLAG_NEXT;
						break;

					case ',':
						if(flags & FLAG_NEED_COMMA)
						{
							flags &= ~FLAG_NEED_COMMA;
							break;
						}

					default:
						sprintf(error, "%d:%d: Unexpected `%c` in object", state.cur_line, state.cur_col, b);
						goto e_failed;
					}
					break;

				case JSON_INTEGER:
				case JSON_DOUBLE:
					if(isdigit(b))
					{
						++num_digits;

						if(top->type == JSON_INTEGER || (flags & FLAG_NUM_E))
						{
							if(!(flags & FLAG_NUM_E))
							{
								if(flags & FLAG_NUM_ZERO)
								{
									sprintf(error, "%d:%d: Unexpected `0` before `%c`", state.cur_line, state.cur_col, b);
									goto e_failed;
								}
								if (num_digits == 1 && b == '0')
									flags |= FLAG_NUM_ZERO;
							}
							else
							{
								flags |= FLAG_NUM_E_GOT_SIGN;
								num_e = (num_e * 10) + (b - '0');
								continue;
							}
							top->u.integer = (top->u.integer * 10) + (b - '0');
							continue;
						}
						num_fraction = (num_fraction * 10) + (b - '0');
						continue;
					}

					if(b == '+' || b == '-')
					{
						if((flags & FLAG_NUM_E) && !(flags & FLAG_NUM_E_GOT_SIGN))
						{
							flags |= FLAG_NUM_E_GOT_SIGN;
							if(b == '-')
								flags |= FLAG_NUM_E_NEGATIVE;
							continue;
						}
					}
					else if(b == '.' && top->type == JSON_INTEGER)
					{
						if(!num_digits)
						{
							sprintf(error, "%d:%d: Expected digit before `.`", state.cur_line, state.cur_col);
							goto e_failed;
						}
						top->type = JSON_DOUBLE;
						top->u.dbl = (double)top->u.integer;
						num_digits = 0;
						continue;
					}

					if(!(flags & FLAG_NUM_E))
					{
						if(top->type == JSON_DOUBLE)
						{
							if(!num_digits)
							{
								sprintf(error, "%d:%d: Expected digit after `.`", state.cur_line, state.cur_col);
								goto e_failed;
							}
							top->u.dbl += ((double) num_fraction) / (pow(10.0, (double) num_digits));
						}

						if(b == 'e' || b == 'E')
						{
							flags |= FLAG_NUM_E;
							if(top->type == JSON_INTEGER)
							{
								top->type = JSON_DOUBLE;
								top->u.dbl = (double) top->u.integer;
							}
							num_digits = 0;
							flags &= ~FLAG_NUM_ZERO;
							continue;
						}
					}
					else
					{
						if(!num_digits)
						{
							sprintf(error, "%d:%d: Expected digit after `e`", state.cur_line, state.cur_col);
							goto e_failed;
						}
						top->u.dbl *= pow(10.0, (double)(flags & FLAG_NUM_E_NEGATIVE ? -num_e : num_e));
					}

					if(flags & FLAG_NUM_NEGATIVE)
					{
						if(top->type == JSON_INTEGER)
							top->u.integer = -top->u.integer;
						else
							top->u.dbl = -top->u.dbl;
					}
					flags |= FLAG_NEXT | FLAG_REPROC;
					break;

				default:
					break;
				};
			}

			if(flags & FLAG_REPROC)
			{
				flags &= ~FLAG_REPROC;
				--state.ptr;
			}

			if(flags & FLAG_NEXT)
			{
				flags = (flags & ~FLAG_NEXT) | FLAG_NEED_COMMA;

				if(!top->parent)
				{
					flags |= FLAG_DONE;
					continue;
				}

				if(top->parent->type == JSON_ARRAY)
					flags |= FLAG_SEEK_VALUE;

				if(!state.first_pass)
				{
					struct json_value_t * parent = top->parent;

					switch(parent->type)
					{
					case JSON_OBJECT:
						parent->u.object.values[parent->u.object.length].value = top;
						break;

					case JSON_ARRAY:
						parent->u.array.values[parent->u.array.length] = top;
						break;

					default:
						break;
					};
				}

				if((++top->parent->u.array.length) > state.uint_max)
					goto e_overflow;
				top = top->parent;
				continue;
			}
		}
		alloc = root;
	}
	return root;

e_unknown_value:
	sprintf(error, "%d:%d: Unknown value", state.cur_line, state.cur_col);
	goto e_failed;

e_alloc_failure:
	strcpy(error, "Memory allocation failure");
	goto e_failed;

e_overflow:
	sprintf(error, "%d:%d: Too long (caught overflow)", state.cur_line, state.cur_col);
	goto e_failed;

e_failed:
	if(errbuf)
	{
		if(*error)
			strcpy(errbuf, error);
		else
			strcpy(errbuf, "Unknown error");
	}
	if(state.first_pass)
		alloc = root;
	while(alloc)
	{
		top = alloc->reserved.next_alloc;
		free(alloc);
		alloc = top;
	}
	if(!state.first_pass)
		json_free(root);
	return 0;
}

void json_free(struct json_value_t * value)
{
	struct json_value_t * v;

	if(!value)
		return;

	value->parent = 0;
	while(value)
	{
		switch(value->type)
		{
		case JSON_ARRAY:
			if(!value->u.array.length)
			{
				free(value->u.array.values);
				break;
			}
			value = value->u.array.values[--value->u.array.length];
			continue;

		case JSON_OBJECT:
			if(!value->u.object.length)
			{
				free(value->u.object.values);
				break;
			}
			value = value->u.object.values[--value->u.object.length].value;
			continue;

		case JSON_STRING:
			free(value->u.string.ptr);
			break;

		default:
			break;
		};

		v = value;
		value = value->parent;
		free(v);
	}
}
