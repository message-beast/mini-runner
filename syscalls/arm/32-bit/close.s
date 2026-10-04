.global syscall_close_arm_32

syscall_close_arm_32:
    mov r7, #6
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
