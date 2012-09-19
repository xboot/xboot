#ifndef __MODULE_H__
#define __MODULE_H__

struct kernel_symbol
{
	void * addr;
	const char * name;
};

extern struct kernel_symbol __ksymtab_start[];
extern struct kernel_symbol __ksymtab_end[];

/*
 * For every exported symbol, place a kernel_symbol in the .ksymtab.text section.
 */
#define EXPORT_SYMBOL(symbol) \
	extern typeof(symbol) symbol; \
	static const char __ksym_name_##symbol[] = #symbol; \
	const struct kernel_symbol __ksymtab_##symbol \
	__attribute__((__used__, section(".ksymtab.text"))) = { (void *)&symbol, __ksym_name_##symbol }

/*
 * Get a kernel symbol (calls must be symmetric)
 */
void * __symbol_get(const char * name);
#define symbol_get(x)	((typeof(&x))(__symbol_get(#x)))

#endif /* __MODULE_H__ */
