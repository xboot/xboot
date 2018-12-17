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
	char * decimal_point;
	char * thousands_sep;
	char * grouping;

	char * int_curr_symbol;
	char * currency_symbol;
	char * mon_decimal_point;
	char * mon_thousands_sep;
	char * mon_grouping;
	char * positive_sign;
	char * negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
};

char * setlocale(int category, const char * locale);
struct lconv * localeconv(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOCALE_H__ */
