#ifndef __MODULE_H__
#define __MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct symbol_t
{
	void * addr;
	const char * name;
};

struct module_t {
	char * name;
	void * space;
	void * module_entry;
	struct symbol_t * symtab;
	size_t nsym;
};

#define EXPORT_SYMBOL(symbol) \
	extern typeof(symbol) symbol; \
	static const char __ksym_name_##symbol[] = #symbol; \
	static const struct symbol_t __ksymtab_##symbol \
	__attribute__((__used__, section(".ksymtab.text"))) = { (void *)&symbol, __ksym_name_##symbol }

#define symbol_get(x) ((typeof(&x))(__symbol_get(#x)))
void * __symbol_get(const char * name);
bool_t register_module(struct module_t * module);
bool_t unregister_module(struct module_t * module);

#ifdef __cplusplus
}
#endif

#endif /* __MODULE_H__ */
