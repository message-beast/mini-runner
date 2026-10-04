.global syscall_fstatat_arm_32

syscall_fstatat_arm_32:
    mov r7, #327
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
