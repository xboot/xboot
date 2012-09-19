#ifndef __INITCALL_H__
#define __INITCALL_H__


/*
 * Used for initialization calls.
 */
typedef void (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define __init		__attribute__ ((__section__ (".init.text")))
#define __exit		__attribute__ ((__section__ (".exit.text")))

extern initcall_t __initcall_start[];
extern initcall_t __initcall_end[];
extern exitcall_t __exitcall_start[];
extern exitcall_t __exitcall_end[];


#define	LEVEL_PURE				"0"
#define	LEVEL_PURE_SYNC			"0s"
#define	LEVEL_MACH              "1"
#define	LEVEL_MACH_SYNC			"1s"
#define	LEVEL_MACH_RES			"2"
#define	LEVEL_MACH_RES_SYNC 	"2s"
#define	LEVEL_MACH_LATE         "3"
#define	LEVEL_MACH_LATE_SYNC 	"3s"
#define	LEVEL_CORE				"4"
#define	LEVEL_CORE_SYNC			"4s"
#define	LEVEL_POSTCORE			"5"
#define	LEVEL_POSTCORE_SYNC		"5s"
#define	LEVEL_SUBSYS			"6"
#define	LEVEL_SUBSYS_SYNC		"6s"
#define	LEVEL_DRIVER			"7"
#define	LEVEL_DRIVER_SYNC		"7s"
#define	LEVEL_COMMAND			"8"
#define	LEVEL_COMMAND_SYNC		"8s"
#define	LEVEL_LATE				"9"
#define	LEVEL_LATE_SYNC			"9s"


#define module_init(fn,level) \
	const initcall_t __initcall_##fn \
	__attribute__((__used__, __section__(".initcall_" level ".text"))) = fn


#define module_exit(fn,level) \
	const exitcall_t __exitcall_##fn \
	__attribute__ ((__used__, __section__ (".exitcall_" level ".text"))) = fn


void do_initcalls(void);
void do_exitcalls(void);

#endif /* __INITCALL_H__ */
