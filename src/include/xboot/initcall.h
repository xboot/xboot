#ifndef __INITCALL_H__
#define __INITCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define __init __attribute__ ((__section__ (".init.text")))
#define __exit __attribute__ ((__section__ (".exit.text")))

#define __define_initcall(level, fn, id) \
	static const initcall_t __initcall_##fn##id \
	__attribute__((__used__, __section__(".initcall_" level ".text"))) = fn

#define __define_exitcall(level, fn, id) \
	static const exitcall_t __exitcall_##fn##id \
	__attribute__((__used__, __section__(".exitcall_" level ".text"))) = fn

#define pure_initcall(fn)		__define_initcall("0", fn, 0)
#define machine_initcall(fn)	__define_initcall("1", fn, 1)
#define core_initcall(fn)		__define_initcall("2", fn, 2)
#define postcore_initcall(fn)	__define_initcall("3", fn, 3)
#define driver_initcall(fn)		__define_initcall("4", fn, 4)
#define subsys_initcall(fn)		__define_initcall("5", fn, 5)
#define command_initcall(fn)	__define_initcall("6", fn, 6)
#define server_initcall(fn)		__define_initcall("7", fn, 7)
#define reserver_initcall(fn)	__define_initcall("8", fn, 8)
#define late_initcall(fn)		__define_initcall("9", fn, 9)

#define pure_exitcall(fn)		__define_exitcall("0", fn, 0)
#define machine_exitcall(fn)	__define_exitcall("1", fn, 1)
#define core_exitcall(fn)		__define_exitcall("2", fn, 2)
#define postcore_exitcall(fn)	__define_exitcall("3", fn, 3)
#define driver_exitcall(fn)		__define_exitcall("4", fn, 4)
#define subsys_exitcall(fn)		__define_exitcall("5", fn, 5)
#define command_exitcall(fn)	__define_exitcall("6", fn, 6)
#define server_exitcall(fn)		__define_exitcall("7", fn, 7)
#define reserver_exitcall(fn)	__define_exitcall("8", fn, 8)
#define late_exitcall(fn)		__define_exitcall("9", fn, 9)

void do_initcalls(void);
void do_exitcalls(void);

#ifdef __cplusplus
}
#endif

#endif /* __INITCALL_H__ */
