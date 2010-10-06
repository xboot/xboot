/*
 * system caller
 */
	.global syscall
	.align 4
syscall:
     stmfd r13!, {r0 - r10, r12, lr}
     mov r10, r0
     ldmia r10, {r0 - r9}
     swi 0
     stmia r10, {r0 - r9}
     ldmfd r13!, {r0 - r10, r12, pc}
