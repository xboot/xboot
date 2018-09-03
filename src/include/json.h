#ifndef __JSON_H__
#define __JSON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

struct json_value_t;
struct json_object_entry_t;

enum json_type_t {
	JSON_NONE		= 0,
	JSON_OBJECT		= 1,
	JSON_ARRAY		= 2,
	JSON_INTEGER	= 3,
	JSON_DOUBLE		= 4,
	JSON_STRING		= 5,
	JSON_BOOLEAN	= 6,
	JSON_NULL		= 7,
};

struct json_value_t {
	struct json_value_t * parent;
	enum json_type_t type;

	union {
		int boolean;
		int64_t integer;
		double dbl;

		struct {
			unsigned int length;
			char * ptr;
		} string;

		struct {
			unsigned int length;
			struct json_object_entry_t * values;
		} object;

		struct {
			unsigned int length;
			struct json_value_t ** values;
		} array;
	} u;

	union {
		struct json_value_t * next_alloc;
		void * object_mem;
	} reserved;
};

struct json_object_entry_t {
	char * name;
	unsigned int name_length;
	struct json_value_t * value;
};

struct json_value_t * json_parse(const char * json, size_t length, char * errbuf);
void json_free(struct json_value_t * value);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_H__ */
