.global syscall_close_arm_64
.intel_syntax noprefix
syscall_close_arm_64:
    mov x8, #56
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
