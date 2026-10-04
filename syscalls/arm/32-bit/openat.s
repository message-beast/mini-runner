.global syscall_openat_arm_32

syscall_openat_arm_32:
    mov r7, #322
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
