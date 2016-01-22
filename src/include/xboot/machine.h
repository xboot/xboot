#ifndef __MACHINE_H__
#define __MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum map_type_t {
	MT_DEVICE	= 0,
	MT_MEMORY	= 1,
};

struct mmap_t {
	const char * name;
	virtual_addr_t virt;
	physical_addr_t phys;
	physical_size_t size;
	enum map_type_t type;
};

struct machine_t {
	struct kobj_t * kobj;
	const char * name;
	const char * desc;
	const struct mmap_t * map;

	bool_t (*detect)(struct machine_t * mach);
	bool_t (*memmap)(struct machine_t * mach);
	bool_t (*shutdown)(struct machine_t * mach);
	bool_t (*reboot)(struct machine_t * mach);
	bool_t (*sleep)(struct machine_t * mach);
	bool_t (*cleanup)(struct machine_t * mach);
	const char * (*uniqueid)(struct machine_t * mach);
	int (*keygen)(struct machine_t * mach, const char * msg, void * key);
};

bool_t register_machine(struct machine_t * mach);
bool_t unregister_machine(struct machine_t * mach);
struct machine_t * get_machine(void);
bool_t machine_shutdown(void);
bool_t machine_reboot(void);
bool_t machine_sleep(void);
bool_t machine_cleanup(void);
const char * machine_uniqueid(void);
int machine_keygen(const char * msg, void * key);
void subsys_init_machine(void);

#ifdef __cplusplus
}
#endif

#endif /* __MACHINE_H__ */
