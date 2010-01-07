#ifndef __STDARG_H__
#define __STDARG_H__


#ifndef __HAVE_VA_LIST
	#define __HAVE_VA_LIST
	typedef char *va_list;
#endif

/*
 * round up the size of TYPE as numbers of sizeof(int), this is
 * because GCC will pad the size of TYPE aligning to size of int
 */
#define __va_rounded_size(TYPE) \
		(((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

/*
 * prepare to access variable args
 */
#define va_start(AP, LASTARG) \
		(AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))

/*
 * what the caller will get is "*((TYPE *)(AP - __va_rounded_size(TYPE)))".
 * the caller will get the value of current argemnt
 */
#define va_arg(AP, TYPE) \
		(AP += __va_rounded_size(TYPE), \
		*((TYPE *)(AP - __va_rounded_size(TYPE))))

/*
 * just do nothing
 */
#define va_end(AP)


#endif /* __STDARG_H__ */
