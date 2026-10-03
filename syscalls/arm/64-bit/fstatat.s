.global syscall_fstatat_arm_64
.intel_syntax noprefix
syscall_fstatat_arm_64:
    mov x8, #79
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
