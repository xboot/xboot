#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <configs.h>
#include <default.h>
#include <xboot/printk.h>

/*
 * debug level (0 ~ 3)
 *
 * 0: no debug;
 * 1: debug error;
 * 2: debug error, warnning;
 * 3: debug error, warnning, information.
 */
#if ( defined(__DEBUG__) && (__DEBUG__ >= 3) )
#define DEBUG_I(fmt, arg...)			printk("[I] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#define DEBUG_W(fmt, arg...)			printk("[w] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#define DEBUG_E(fmt, arg...)			printk("[E] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#elif ( defined(__DEBUG__) && (__DEBUG__ == 2) )
#define DEBUG_I(fmt...)					do{ } while(0)
#define DEBUG_W(fmt, arg...)			printk("[w] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#define DEBUG_E(fmt, arg...)			printk("[E] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#elif  ( defined(__DEBUG__) && (__DEBUG__ == 1) )
#define DEBUG_I(fmt...)					do{ } while(0)
#define DEBUG_W(fmt...)					do{ } while(0)
#define DEBUG_E(fmt, arg...)			printk("[E] at line %d in %s: " fmt "\r\n", __LINE__, __FILE__, ##arg)
#else
#define DEBUG_I(fmt...)					do{ } while(0)
#define DEBUG_W(fmt...)					do{ } while(0)
#define DEBUG_E(fmt...)					do{ } while(0)
#endif

/*
 * assertion macro
 */
#if	( defined(__DEBUG__) && (__DEBUG__ > 0) )
#define ASSERT(e)						do { if(!(e)) printk("assert \"%s\" failed at line %d in %s\r\n", #e, __LINE__, __FILE__); } while(0)
#else
#define ASSERT(e)						do{ } while(0)
#endif

#endif /* __DEBUG_H__ */
