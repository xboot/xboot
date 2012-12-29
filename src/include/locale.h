#ifndef __LOCALE_H__
#define __LOCALE_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	LC_CTYPE 		= 0,
	LC_NUMERIC 		= 1,
	LC_TIME 		= 2,
	LC_COLLATE 		= 3,
	LC_MONETARY 	= 4,
	LC_MESSAGES 	= 5,
	LC_ALL 			= 6,
};

struct lconv {
};

char * setlocale(int category, const char * locale);
struct lconv * localeconv(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOCALE_H__ */
