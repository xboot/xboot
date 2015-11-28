#ifndef __GDBSTUB_H__
#define __GDBSTUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct gdb_iterface_t {
	int (*read)(struct gdb_iterface_t * iface, char * buf, int count);
	int (*write)(struct gdb_iterface_t * iface, const char * buf, int count);
	void (*flush)(struct gdb_iterface_t * iface);
	void * priv;
};

struct gdb_cpu_t {
	int nregs;
	int (*read_register)(struct gdb_cpu_t * cpu, char * buf, int reg);
	int (*write_register)(struct gdb_cpu_t * cpu, char * buf, int reg);
	void * priv;
};

struct gdb_state_t {
	struct gdb_iterface_t * iface;
	struct gdb_cpu_t * cpu;
};

struct gdb_state_t * gdbserver_init(const char * device);
void gdbserver_exit(struct gdb_state_t * s);
void gdbserver_start(struct gdb_state_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __GDBSTUB_H__ */
