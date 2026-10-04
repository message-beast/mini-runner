.global syscall_getdents64_arm_32

syscall_getdents64_arm_32:
    mov r7, #217
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
