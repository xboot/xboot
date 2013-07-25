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

	/* Logger output function */
	ssize_t (*output)(const char * buf, size_t count);
};

bool_t register_logger(struct logger_t * logger);
bool_t unregister_logger(struct logger_t * logger);
void logger_output(const char * buf, size_t count);
int logger_print(const char * fmt, ...);

#if	defined(CONFIG_NO_LOG) && (CONFIG_NO_LOG > 0)
#define LOG(fmt, arg...)	do { } while(0)
#else
#define LOG(fmt, arg...)	do { logger_print("[%s:%d] "fmt, __FILE__, __LINE__, ##arg); } while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
