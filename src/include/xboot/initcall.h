#ifndef __INITCALL_H__
#define __INITCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Used for initialization calls.
 */
typedef void (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define __init	__attribute__ ((__section__ (".init.text")))
#define __exit	__attribute__ ((__section__ (".exit.text")))

/*
 * This only exists for built-in code, not for modules.
 */
#define __define_initcall(level, fn, id) \
	const initcall_t __initcall_##fn##id \
	__attribute__((__used__, __section__(".initcall_" level ".text"))) = fn

#define __define_exitcall(level, fn, id) \
	const exitcall_t __exitcall_##fn##id \
	__attribute__((__used__, __section__(".exitcall_" level ".text"))) = fn

#define pure_initcall(fn)			__define_initcall("0",  fn, 0)
#define pure_initcall_sync(fn)		__define_initcall("0s", fn, 0s)
#define core_initcall(fn)			__define_initcall("1",  fn, 1)
#define core_initcall_sync(fn)		__define_initcall("1s", fn, 1s)
#define postcore_initcall(fn)		__define_initcall("2",  fn, 2)
#define postcore_initcall_sync(fn)	__define_initcall("2s", fn, 2s)
#define arch_initcall(fn)			__define_initcall("3",  fn, 3)
#define arch_initcall_sync(fn)		__define_initcall("3s", fn, 3s)
#define subsys_initcall(fn)			__define_initcall("4",  fn, 4)
#define subsys_initcall_sync(fn)	__define_initcall("4s", fn, 4s)
#define fs_initcall(fn)				__define_initcall("5",  fn, 5)
#define fs_initcall_sync(fn)		__define_initcall("5s", fn, 5s)
#define device_initcall(fn)			__define_initcall("6",  fn, 6)
#define device_initcall_sync(fn)	__define_initcall("6s", fn, 6s)
#define command_initcall(fn)		__define_initcall("7",  fn, 7)
#define command_initcall_sync(fn)	__define_initcall("7s", fn, 7s)
#define late_initcall(fn)			__define_initcall("8",  fn, 8)
#define late_initcall_sync(fn)		__define_initcall("8s", fn, 8s)

#define pure_exitcall(fn)			__define_exitcall("0",  fn, 0)
#define pure_exitcall_sync(fn)		__define_exitcall("0s", fn, 0s)
#define core_exitcall(fn)			__define_exitcall("1",  fn, 1)
#define core_exitcall_sync(fn)		__define_exitcall("1s", fn, 1s)
#define postcore_exitcall(fn)		__define_exitcall("2",  fn, 2)
#define postcore_exitcall_sync(fn)	__define_exitcall("2s", fn, 2s)
#define arch_exitcall(fn)			__define_exitcall("3",  fn, 3)
#define arch_exitcall_sync(fn)		__define_exitcall("3s", fn, 3s)
#define subsys_exitcall(fn)			__define_exitcall("4",  fn, 4)
#define subsys_exitcall_sync(fn)	__define_exitcall("4s", fn, 4s)
#define fs_exitcall(fn)				__define_exitcall("5",  fn, 5)
#define fs_exitcall_sync(fn)		__define_exitcall("5s", fn, 5s)
#define device_exitcall(fn)			__define_exitcall("6",  fn, 6)
#define device_exitcall_sync(fn)	__define_exitcall("6s", fn, 6s)
#define command_exitcall(fn)		__define_exitcall("7",  fn, 7)
#define command_exitcall_sync(fn)	__define_exitcall("7s", fn, 7s)
#define late_exitcall(fn)			__define_exitcall("8",  fn, 8)
#define late_exitcall_sync(fn)		__define_exitcall("8s", fn, 8s)


void do_initcalls(void);
void do_exitcalls(void);

#ifdef __cplusplus
}
#endif

#endif /* __INITCALL_H__ */
