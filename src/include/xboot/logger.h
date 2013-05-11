#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct logger_t
{
	/* The logger name */
	const char * name;

	/* Logger initial function */
	void (*init)(void);

	/* Logger clean up function */
	void (*exit)(void);

	/* Logger write function */
	ssize_t (*write)(const u8_t * buf, size_t count);
};

bool_t register_logger(struct logger_t * logger);
bool_t unregister_logger(struct logger_t * logger);
int logger_output(const char * file, const int line, const char * fmt, ...);

#if	defined(CONFIG_NO_LOG) && (CONFIG_NO_LOG > 0)
#define LOG(fmt, arg...)	do { } while(0)
#else
#define LOG(fmt, arg...)	do { logger_output(__FILE__, __LINE__, fmt, ##arg); } while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
