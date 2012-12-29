#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * log level
 */
enum log_level {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNNING,
	LOG_ERROR,
};

/*
 * the list of log
 */
struct log_list
{
	/* log level */
	enum log_level level;

	/* __LINE__ */
	int line;

	/* __FILE__ */
	char * file;

	/* message */
	char * message;

	struct list_head entry;
};


s32_t log_numberof(void);
bool_t log_add(enum log_level level, const int line, const char * file, const char * fmt, ...);
bool_t log_remove(void);
struct log_list * get_log_list(void);

/*
 * LOG interface
 */
#define LOG_D(fmt, arg...)				log_add(LOG_DEBUG, __LINE__, __FILE__, fmt, ##arg)
#define LOG_I(fmt, arg...)				log_add(LOG_INFO, __LINE__, __FILE__, fmt, ##arg)
#define LOG_W(fmt, arg...)				log_add(LOG_WARNNING, __LINE__, __FILE__, fmt, ##arg)
#define LOG_E(fmt, arg...)				log_add(LOG_ERROR, __LINE__, __FILE__, fmt, ##arg)

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
