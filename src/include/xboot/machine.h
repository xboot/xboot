#ifndef __MACHINE_H__
#define __MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <mode/mode.h>

struct battery_info {
	bool_t charging;
	s32_t voltage;				/* mV */
	s32_t charge_current;		/* mA */
	s32_t discharge_current;	/* mA */
	s32_t temperature;			/* 0.1 Centigrade */
	s32_t capacity;				/* mAh */
	s32_t internal_resistance;	/* mohm */
	s32_t level;				/* percentage */
};

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
		void (*init)(void);

		/* system sleep */
		bool_t (*sleep)(void);

		/* system halt */
		bool_t (*halt)(void);

		/* system reset */
		bool_t (*reset)(void);
	}pm;

	/*
	 * misc function
	 */
	struct {
		/* get system mode */
		enum mode_t (*getmode)(void);

		/* get battery information */
		bool_t (*batinfo)(struct battery_info * info);

		/* clean up system before running os */
		bool_t (*cleanup)(void);

		/* machine authentication */
		bool_t (*authentication)(void);
	}misc;

	/*
	 * private data
	 */
	void * priv;
};

bool_t register_machine(struct machine * mach);
inline struct machine * get_machine(void);

bool_t machine_sleep(void);
bool_t machine_halt(void);
bool_t machine_reset(void);
bool_t machine_batinfo(struct battery_info * info);
bool_t machine_cleanup(void);
bool_t machine_authentication(void);

#ifdef __cplusplus
}
#endif

#endif /* __MACHINE_H__ */
