.global syscall_rmdir_arm_32

syscall_rmdir_arm_32:
    mov r7, #40
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
