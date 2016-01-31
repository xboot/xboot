#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct logger_t
{
	const char * name;
	void (*init)(struct logger_t * logger);
	void (*output)(struct logger_t * logger, const char * buf, int count);
	void * priv;
};

bool_t register_logger(struct logger_t * logger);
bool_t unregister_logger(struct logger_t * logger);
int logger_print(const char * fmt, ...);

#if	defined(CONFIG_NO_LOG) && (CONFIG_NO_LOG > 0)
#define LOG(fmt, arg...)	do { } while(0)
#else
#define LOG(fmt, arg...)	do { logger_print("[%s:%d] " fmt "\r\n", __FILE__, __LINE__, ##arg); } while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
