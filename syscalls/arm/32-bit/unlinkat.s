.global syscall_unlinkat_arm_32

syscall_unlinkat_arm_32:
    mov r7, #328
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
