.global syscall_close_arm_64

syscall_close_arm_64:
    mov x8, #57
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
