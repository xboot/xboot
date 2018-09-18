#ifndef __RISCV32_SETJMP_H__
#define __RISCV32_SETJMP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct __jmp_buf {
	unsigned long __jmp_buf[40];
};
typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif /* __RISCV32_SETJMP_H__ */
