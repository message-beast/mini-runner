.global syscall_unlinkat_arm_64

syscall_unlinkat_arm_64:
    mov x8, #35
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
