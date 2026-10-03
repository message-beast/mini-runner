.global syscall_getdents64_arm_64
.intel_syntax noprefix
syscall_getdents64_arm_64:
    mov x8, #61
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
