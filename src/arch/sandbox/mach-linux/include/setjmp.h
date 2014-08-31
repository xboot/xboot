#ifndef __X86_64_SETJMP_H__
#define __X86_64_SETJMP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct __jmp_buf {
	unsigned long __rbx;
	unsigned long __rsp;
	unsigned long __rbp;
	unsigned long __r12;
	unsigned long __r13;
	unsigned long __r14;
	unsigned long __r15;
	unsigned long __rip;
};

typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif /* __X86_64_SETJMP_H__ */
