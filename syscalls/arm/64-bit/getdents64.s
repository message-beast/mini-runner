.global syscall_getdents64_arm_64

syscall_getdents64_arm_64:
    mov x8, #61
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
