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
		struct {
			const void * start;
			const void * end;
		} mem_banks[8];

		/* extern crystal */
		const u64_t xtal;
	}res;

	/*
	 * link information
	 */
	struct {
		const void * text_start;
		const void * text_end;

		const void * ramdisk_start;
		const void * ramdisk_end;

		const void * data_shadow_start;
		const void * data_shadow_end;

		const void * data_start;
		const void * data_end;

		const void * bss_start;
		const void * bss_end;

		const void * heap_start;
		const void * heap_end;

		const void * stack_start;
		const void * stack_end;
	}link;

	/*
	 * power manager
	 */
	struct {
		/* system initial, like power lock */
		void(*init)(void);

		/* system suspend */
		bool_t(*suspend)(void);

		/* system resume */
		bool_t(*resume)(void);

		/* system halt */
		bool_t(*halt)(void);

		/* system reset */
		bool_t(*reset)(void);
	}pm;

	/*
	 * misc function
	 */
	struct {
		/* get system mode */
		enum mode (*getmode)(void);

		/* clean up system before running os */
		bool_t(*cleanup)(void);

		/* for anti-piracy */
		bool_t(*genuine)(void);
	}misc;

	/*
	 * private data
	 */
	void * priv;
};

bool_t machine_register(struct machine * mach);
inline struct machine * get_machine(void);

bool_t suspend(void);
bool_t resume(void);
bool_t halt(void);
bool_t reset(void);
bool_t cleanup(void);

void do_system_antipiracy(void);

#endif /* __MACHINE_H__ */
