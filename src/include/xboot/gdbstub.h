#ifndef __GDBSTUB_H__
#define __GDBSTUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct gdb_cpu_t {
	const int nregs;
	void (*save_register)(struct gdb_cpu_t * cpu, void * regs);
	void (*restore_register)(struct gdb_cpu_t * cpu, void * regs);
	int (*read_register)(struct gdb_cpu_t * cpu, char * buf, int n);
	int (*write_register)(struct gdb_cpu_t * cpu, char * buf, int n);
	int (*set_pc)(struct gdb_cpu_t * cpu, virtual_addr_t pc);
	int (*mem_access)(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw);
	void (*breakpoint)(struct gdb_cpu_t * cpu);
	void * env;
};

struct gdb_iterface_t {
	int (*read)(struct gdb_iterface_t * iface, char * buf, int count);
	int (*write)(struct gdb_iterface_t * iface, const char * buf, int count);
	void (*flush)(struct gdb_iterface_t * iface);
	void * priv;
};

struct gdb_state_t {
	int idle;
	struct gdb_cpu_t * cpu;
	struct gdb_iterface_t * iface;
};

int gdbserver_start(const char * device);
void gdbserver_stop(void);
void gdbserver_handle_exception(void * regs);

#ifdef __cplusplus
}
#endif

#endif /* __GDBSTUB_H__ */
