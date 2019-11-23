#ifndef __WBOXTEST_H__
#define __WBOXTEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct wboxtest_t
{
	struct kobj_t * kobj;
	struct hlist_node node;

	const char * group;
	const char * name;
	void * (*setup)(struct wboxtest_t * wbt);
	void (*clean)(struct wboxtest_t * wbt, void * data);
	void (*run)(struct wboxtest_t * wbt, void * data);
};

void __wboxtest_print(int cond, char * expr, const char * file, int line);

#define assert_null(x)						do { __wboxtest_print(((x) == NULL), #x, __FILE__, __LINE__); } while(0);
#define assert_not_null(x)					do { __wboxtest_print(((x) != NULL), #x, __FILE__, __LINE__); } while(0);
#define assert_true(x)						do { __wboxtest_print((x), #x, __FILE__, __LINE__); } while(0);
#define assert_false(x)						do { __wboxtest_print((!(x)), #x, __FILE__, __LINE__); } while(0);
#define assert_equal(a, b)					do { __wboxtest_print(((a) == (b)), #a" == "#b, __FILE__, __LINE__); } while(0);
#define assert_not_equal(a, b)				do { __wboxtest_print(((a) != (b)), #a" != "#b, __FILE__, __LINE__); } while(0);
#define assert_string_equal(a, b)			do { __wboxtest_print((strcmp((a), (b)) == 0), "S["#a" == "#b"]", __FILE__, __LINE__); } while(0);
#define assert_string_not_equal(a, b)		do { __wboxtest_print((strcmp((a), (b)) != 0), "S["#a" != "#b"]", __FILE__, __LINE__); } while(0);
#define assert_memory_equal(a, b, l)		do { __wboxtest_print((memcmp((a), (b), (l)) == 0), "M["#a" == "#b"]", __FILE__, __LINE__); } while(0);
#define assert_memory_not_equal(a, b, l)	do { __wboxtest_print((memcmp((a), (b), (l)) != 0), "M["#a" != "#b"]", __FILE__, __LINE__); } while(0);
#define assert_inrange(v, min, max)			do { __wboxtest_print((((v) >= (min)) && ((v) <= (max))), #v"><["#min", "#max"]", __FILE__, __LINE__); } while(0);
#define assert_not_inrange(v, min, max)		do { __wboxtest_print((((v) < (min)) && ((v) > (max))), #v"<>["#min", "#max"]", __FILE__, __LINE__); } while(0);

struct wboxtest_t * search_wboxtest(const char * group, const char * name);
bool_t register_wboxtest(struct wboxtest_t * wbt);
bool_t unregister_wboxtest(struct wboxtest_t * wbt);
void wboxtest_run_group_name(const char * group, const char * name, int count);
void wboxtest_run_group(const char * group, int count);
void wboxtest_run_all(int count);
void wboxtest_list(void);

#ifdef __cplusplus
}
#endif

#endif /* __WBOXTEST_H__ */
