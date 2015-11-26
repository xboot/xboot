#ifndef __ARM32_SETJMP_H__
#define __ARM32_SETJMP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct __jmp_buf {
	unsigned long long __jmp_buf[32];
};

typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_SETJMP_H__ */
