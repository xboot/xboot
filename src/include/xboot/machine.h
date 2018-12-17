#ifndef __MACHINE_H__
#define __MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <sizes.h>
#include <list.h>
#include <io.h>
#include <irqflags.h>
#include <spinlock.h>
#include <xboot/kobj.h>
#include <xboot/initcall.h>

struct mmap_t {
	struct list_head list;
	const char * name;
	virtual_addr_t virt;
	physical_addr_t phys;
	physical_size_t size;
	int type;
};

struct machine_t {
	struct kobj_t * kobj;
	struct list_head list;
	struct list_head mmap;

	const char * name;
	const char * desc;

	int (*detect)(struct machine_t * mach);
	void (*memmap)(struct machine_t * mach);
	void (*smpinit)(struct machine_t * mach, int cpu);
	void (*smpboot)(struct machine_t * mach, int cpu, void (*func)(int cpu));
	void (*shutdown)(struct machine_t * mach);
	void (*reboot)(struct machine_t * mach);
	void (*sleep)(struct machine_t * mach);
	void (*cleanup)(struct machine_t * mach);
	void (*logger)(struct machine_t * mach, const char * buf, int count);
	const char * (*uniqueid)(struct machine_t * mach);
	int (*keygen)(struct machine_t * mach, const char * msg, void * key);
};

bool_t machine_mmap(struct machine_t * mach, const char * name, virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type);
bool_t register_machine(struct machine_t * mach);
bool_t unregister_machine(struct machine_t * mach);
struct machine_t * get_machine(void);
void machine_smpinit(int cpu);
void machine_smpboot(int cpu, void (*func)(int cpu));
void machine_shutdown(void);
void machine_reboot(void);
void machine_sleep(void);
void machine_cleanup(void);
int machine_logger(const char * fmt, ...);
const char * machine_uniqueid(void);
int machine_keygen(const char * msg, void * key);

#if	defined(CONFIG_NO_LOG) && (CONFIG_NO_LOG > 0)
#define LOG(fmt, arg...)	do { } while(0)
#else
#define LOG(fmt, arg...)	do { machine_logger(" " fmt "\r\n", ##arg); } while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MACHINE_H__ */
