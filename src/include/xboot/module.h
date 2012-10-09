#ifndef __MODULE_H__
#define __MODULE_H__

#include <xboot.h>

struct symbol_t
{
	void * addr;
	const char * name;
};

struct module_t {
	/* module name */
	char * name;

	/* module memory space */
	void * space;

	void * module_entry;

	/* module exported symbols */
	struct symbol_t * symtab;
	u32_t nsym;
};

struct module_list
{
	struct module_t * module;
	struct list_head entry;
};

extern struct symbol_t __ksymtab_start[];
extern struct symbol_t __ksymtab_end[];

/*
 * For every exported symbol, place a symbol_t in the .ksymtab.text section.
 */
#define EXPORT_SYMBOL(symbol) \
	extern typeof(symbol) symbol; \
	static const char __ksym_name_##symbol[] = #symbol; \
	const struct symbol_t __ksymtab_##symbol \
	__attribute__((__used__, section(".ksymtab.text"))) = { (void *)&symbol, __ksym_name_##symbol }

/*
 * Get a kernel symbol (calls must be symmetric)
 */
#define symbol_get(x) ((typeof(&x))(__symbol_get(#x)))
void * __symbol_get(const char * name);
struct symbol_t * find_symbol(struct module_t * module, const char * name);
struct module_t * find_module(const char * name);

struct module_list * __module_list_init(void);
void __module_list_exit(struct module_list * m);

#endif /* __MODULE_H__ */
