#ifndef __ARM32_LINKAGE_H__
#define __ARM32_LINKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN		.align 0
#define ALIGN_STR	".align 0"

#define ENTRY(name) \
	.globl name; \
	ALIGN; \
	name:

#define WEAK(name) \
	.weak name; \
	name:

#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_LINKAGE_H__ */
