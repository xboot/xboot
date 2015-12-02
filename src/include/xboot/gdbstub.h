#ifndef __GDBSTUB_H__
#define __GDBSTUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct gdb_cpu_t {
	const int nregs;
	void * env;
	void * bplist;
	void (*save_register)(struct gdb_cpu_t * cpu, void * regs);
	void (*restore_register)(struct gdb_cpu_t * cpu, void * regs);
	int (*read_register)(struct gdb_cpu_t * cpu, char * buf, int n);
	int (*write_register)(struct gdb_cpu_t * cpu, char * buf, int n);
	int (*set_program_counter)(struct gdb_cpu_t * cpu, virtual_addr_t pc);
	int (*acess_memory)(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw);
	int (*breakpoint_insert)(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int type);
	int (*breakpoint_remove)(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int type);
	int (*breakpoint_remove_all)(struct gdb_cpu_t * cpu);
	void (*breakpoint)(struct gdb_cpu_t * cpu);
	void (*singlestep)(struct gdb_cpu_t * cpu);
	void * priv;
};

struct gdb_iterface_t {
	int (*read)(struct gdb_iterface_t * iface, char * buf, int count);
	int (*write)(struct gdb_iterface_t * iface, const char * buf, int count);
	void (*flush)(struct gdb_iterface_t * iface);
	void * priv;
};

struct gdb_state_t {
	int trapping;
	int connected;
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
