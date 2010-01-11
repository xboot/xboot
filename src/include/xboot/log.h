#ifndef __LOG_H__
#define __LOG_H__

#include <configs.h>
#include <default.h>
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
	x_s32 line;

	/* __FILE__ */
	x_s8 * file;

	/* message */
	x_s8 * message;

	struct list_head entry;
};


x_s32 log_numberof(void);
x_bool log_add(enum log_level level, const x_s32 line, const char * file, const char * fmt, ...);
x_bool log_remove(void);
struct log_list * get_log_list(void);

/*
 * LOG interface
 */
#define LOG_D(fmt, arg...)				log_add(LOG_DEBUG, __LINE__, __FILE__, fmt, ##arg)
#define LOG_I(fmt, arg...)				log_add(LOG_INFO, __LINE__, __FILE__, fmt, ##arg)
#define LOG_W(fmt, arg...)				log_add(LOG_WARNNING, __LINE__, __FILE__, fmt, ##arg)
#define LOG_E(fmt, arg...)				log_add(LOG_ERROR, __LINE__, __FILE__, fmt, ##arg)

#endif /* __LOG_H__ */
