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

struct wboxtest_t * search_wboxtest(const char * group, const char * name);
bool_t register_wboxtest(struct wboxtest_t * wbt);
bool_t unregister_wboxtest(struct wboxtest_t * wbt);
void wboxtest_run_group_name(const char * group, const char * name, int count);
void wboxtest_run_group(const char * group, int count);
void wboxtest_run_all(int count);
void wboxtest_list(void);

#define assert_null(x)						do { wboxtest_assert(((x) == NULL), #x, __FILE__, __LINE__); } while(0);
#define assert_not_null(x)					do { wboxtest_assert(((x) != NULL), #x, __FILE__, __LINE__); } while(0);
#define assert_true(x)						do { wboxtest_assert((x), #x, __FILE__, __LINE__); } while(0);
#define assert_false(x)						do { wboxtest_assert((!(x)), #x, __FILE__, __LINE__); } while(0);
#define assert_equal(a, b)					do { wboxtest_assert(((a) == (b)), #a" == "#b, __FILE__, __LINE__); } while(0);
#define assert_not_equal(a, b)				do { wboxtest_assert(((a) != (b)), #a" != "#b, __FILE__, __LINE__); } while(0);
#define assert_string_equal(a, b)			do { wboxtest_assert((strcmp((a), (b)) == 0), "S["#a" == "#b"]", __FILE__, __LINE__); } while(0);
#define assert_string_not_equal(a, b)		do { wboxtest_assert((strcmp((a), (b)) != 0), "S["#a" != "#b"]", __FILE__, __LINE__); } while(0);
#define assert_memory_equal(a, b, l)		do { wboxtest_assert((memcmp((a), (b), (l)) == 0), "M["#a" == "#b"]", __FILE__, __LINE__); } while(0);
#define assert_memory_not_equal(a, b, l)	do { wboxtest_assert((memcmp((a), (b), (l)) != 0), "M["#a" != "#b"]", __FILE__, __LINE__); } while(0);
#define assert_inrange(v, min, max)			do { wboxtest_assert((((v) >= (min)) && ((v) <= (max))), #v"><["#min", "#max"]", __FILE__, __LINE__); } while(0);
#define assert_not_inrange(v, min, max)		do { wboxtest_assert((((v) < (min)) && ((v) > (max))), #v"<>["#min", "#max"]", __FILE__, __LINE__); } while(0);

int wboxtest_random_int(int a, int b);
double wboxtest_random_float(double a, double b);
char * wboxtest_random_string(char * buf, int len);
char * wboxtest_random_buffer(char * buf, int len);
int wboxtest_print(const char * fmt, ...);
void wboxtest_assert(int cond, char * expr, const char * file, int line);

#ifdef __cplusplus
}
#endif

#endif /* __WBOXTEST_H__ */
