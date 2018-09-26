#ifndef __RISCV64_SETJMP_H__
#define __RISCV64_SETJMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __riscv_xlen == 32
struct __jmp_buf {
	unsigned long __jmp_buf[38];
};
#elif __riscv_xlen == 64
struct __jmp_buf {
	unsigned long long __jmp_buf[26];
};
#endif

typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_SETJMP_H__ */
