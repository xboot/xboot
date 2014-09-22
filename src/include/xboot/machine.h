#ifndef __MACHINE_H__
#define __MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct machine_t {
	const char * name;
	const char * desc;

	struct {
		const physical_addr_t start;
		const physical_size_t size;
	} banks[16 + 1];

	bool_t (*detect)(void);
	bool_t (*powerup)(void);
	bool_t (*shutdown)(void);
	bool_t (*reset)(void);
	bool_t (*sleep)(void);
	bool_t (*cleanup)(void);
	bool_t (*authentication)(void);
	enum mode_t (*getmode)(void);
};

struct machine_t * get_machine(void);
bool_t register_machine(struct machine_t * mach);
bool_t unregister_machine(struct machine_t * mach);

bool_t machine_shutdown(void);
bool_t machine_reset(void);
bool_t machine_sleep(void);
bool_t machine_cleanup(void);
bool_t machine_authentication(void);

void subsys_init_machine(void);

#ifdef __cplusplus
}
#endif

#endif /* __MACHINE_H__ */
