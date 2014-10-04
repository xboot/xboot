/*
 * libc/time/strftime.c
 */

#include <stdarg.h>
#include <time.h>

static char * aday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char * day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

static char * amonth[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char * month[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static char buf[26];

static int powers[5] = { 1, 10, 100, 1000, 10000 };

static void strfmt(char * str, const char * fmt, ...)
{
	int ival, ilen;
	char *sval;
	va_list vp;

	va_start(vp, fmt);
	while (*fmt)
	{
		if (*fmt++ == '%')
		{
			ilen = *fmt++ - '0';
			if (ilen == 0)
			{
				sval = va_arg(vp, char *);
				while (*sval)
					*str++ = *sval++;
			}
			else
			{
				ival = va_arg(vp, int);

				while (ilen)
				{
					ival %= powers[ilen--];
					*str++ = (char) ('0' + ival / powers[ilen]);
				}
			}
		}
		else
			*str++ = fmt[-1];
	}
	*str = '\0';
	va_end(vp);
}

size_t strftime(char * s, size_t max, const char * fmt, const struct tm * t)
{
	int w, d;
	char *p, *q, *r;

	p = s;
	q = s + max - 1;
	while ((*fmt != '\0'))
	{
		if (*fmt++ == '%')
		{
			r = buf;
			switch (*fmt++)
			{
			case '%':
				r = "%";
				break;

			case 'a':
				r = aday[t->tm_wday];
				break;

			case 'A':
				r = day[t->tm_wday];
				break;

			case 'b':
				r = amonth[t->tm_mon];
				break;

			case 'B':
				r = month[t->tm_mon];
				break;

			case 'c':
				strfmt(r, "%0 %0 %2 %2:%2:%2 %4", aday[t->tm_wday],
						amonth[t->tm_mon], t->tm_mday, t->tm_hour, t->tm_min,
						t->tm_sec, t->tm_year + 1900);
				break;

			case 'd':
				strfmt(r, "%2", t->tm_mday);
				break;

			case 'H':
				strfmt(r, "%2", t->tm_hour);
				break;

			case 'I':
				strfmt(r, "%2", (t->tm_hour % 12) ? t->tm_hour % 12 : 12);
				break;

			case 'j':
				strfmt(r, "%3", t->tm_yday + 1);
				break;

			case 'm':
				strfmt(r, "%2", t->tm_mon + 1);
				break;

			case 'M':
				strfmt(r, "%2", t->tm_min);
				break;

			case 'p':
				r = (t->tm_hour > 11) ? "PM" : "AM";
				break;

			case 'S':
				strfmt(r, "%2", t->tm_sec);
				break;

			case 'U':
				w = t->tm_yday / 7;
				if (t->tm_yday % 7 > t->tm_wday)
					w++;
				strfmt(r, "%2", w);
				break;

			case 'W':
				w = t->tm_yday / 7;
				if (t->tm_yday % 7 > (t->tm_wday + 6) % 7)
					w++;
				strfmt(r, "%2", w);
				break;

			case 'V':
				w = (t->tm_yday + 7 - (t->tm_wday ? t->tm_wday - 1 : 6)) / 7;
				d = (t->tm_yday + 7 - (t->tm_wday ? t->tm_wday - 1 : 6)) % 7;

				if (d >= 4)
				{
					w++;
				}
				else if (w == 0)
				{
					w = 53;
				}
				strfmt(r, "%2", w);
				break;

			case 'w':
				strfmt(r, "%1", t->tm_wday);
				break;

			case 'x':
				strfmt(r, "%3s %3s %2 %4", aday[t->tm_wday], amonth[t->tm_mon],
						t->tm_mday, t->tm_year + 1900);
				break;

			case 'X':
				strfmt(r, "%2:%2:%2", t->tm_hour, t->tm_min, t->tm_sec);
				break;

			case 'y':
				strfmt(r, "%2", t->tm_year % 100);
				break;

			case 'Y':
				strfmt(r, "%4", t->tm_year + 1900);
				break;

			case 'Z':
				r = t->tm_isdst ? "DST" : "GMT";
				break;

			default:
				buf[0] = '%';
				buf[1] = fmt[-1];
				buf[2] = '\0';
				if (buf[1] == 0)
					fmt--;
				break;
			}
			while (*r)
			{
				if (p == q)
				{
					*q = '\0';
					return 0;
				}
				*p++ = *r++;
			}
		}
		else
		{
			if (p == q)
			{
				*q = '\0';
				return 0;
			}
			*p++ = fmt[-1];
		}
	}

	*p = '\0';
	return p - s;
}
EXPORT_SYMBOL(strftime);
