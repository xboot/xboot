#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <xboot.h>
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
			const ptrdiff_t start;
			const ptrdiff_t end;
		} mem_banks[8 + 1];

		/* extern crystal */
		const u64_t xtal;
	}res;

	/*
	 * link information
	 */
	struct {
		const ptrdiff_t text_start;
		const ptrdiff_t text_end;

		const ptrdiff_t romdisk_start;
		const ptrdiff_t romdisk_end;

		const ptrdiff_t data_shadow_start;
		const ptrdiff_t data_shadow_end;

		const ptrdiff_t data_start;
		const ptrdiff_t data_end;

		const ptrdiff_t bss_start;
		const ptrdiff_t bss_end;

		const ptrdiff_t heap_start;
		const ptrdiff_t heap_end;

		const ptrdiff_t stack_start;
		const ptrdiff_t stack_end;
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

		/* machine authentication */
		bool_t(*authentication)(void);
	}misc;

	/*
	 * private data
	 */
	void * priv;
};

bool_t register_machine(struct machine * mach);
inline struct machine * get_machine(void);

bool_t machine_suspend(void);
bool_t machine_resume(void);
bool_t machine_halt(void);
bool_t machine_reset(void);
bool_t machine_cleanup(void);
bool_t machine_authentication(void);

#endif /* __MACHINE_H__ */
