#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <configs.h>
#include <default.h>
#include <mode/mode.h>

/*
 * defined the struct of machine, which contains
 * a portable operating interface.
 */
struct machine {
	/*
	 * basic information
	 */
	struct {
		const char * board_name;
		const char * board_desc;
		const char * board_id;

		const char * cpu_name;
		const char * cpu_desc;
		const char * cpu_id;
	}info;

	/*
	 * machine resource
	 */
	struct {
		const x_sys mem_start;
		const x_sys mem_end;

		/* extern crystal */
		const x_u64 xtal;
	}res;

	/*
	 * link information
	 */
	struct {
		const x_sys text_start;
		const x_sys text_end;

		const x_sys ramdisk_start;
		const x_sys ramdisk_end;

		const x_sys data_shadow_start;
		const x_sys data_shadow_end;

		const x_sys data_start;
		const x_sys data_end;

		const x_sys bss_start;
		const x_sys bss_end;

		const x_sys heap_start;
		const x_sys heap_end;

		const x_sys stack_start;
		const x_sys stack_end;
	}link;

	/*
	 * power manager
	 */
	struct {
		/* system initial, like power lock */
		void(*init)(void);

		/* system suspend */
		x_bool(*suspend)(void);

		/* system resume */
		x_bool(*resume)(void);

		/* system halt */
		x_bool(*halt)(void);

		/* system reset */
		x_bool(*reset)(void);
	}pm;

	/*
	 * misc function
	 */
	struct {
		/* get system mode */
		enum mode (*getmode)(void);

		/* clean up system before running os */
		x_bool(*cleanup)(void);

		/* for anti-piracy */
		x_bool(*genuine)(void);
	}misc;

	/*
	 * private data
	 */
	void * priv;
};

x_bool machine_register(struct machine * mach);
inline struct machine * get_machine(void);

x_bool suspend(void);
x_bool resume(void);
x_bool halt(void);
x_bool reset(void);
x_bool cleanup(void);

void do_system_antipiracy(void);

#endif /* __MACHINE_H__ */
