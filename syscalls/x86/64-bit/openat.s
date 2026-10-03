.global syscall_openat_x86_64
.intel_syntax noprefix
syscall_openat_x86_64:
    mov rax, 257
    mov r10, rcx
    syscall
    ret

.section .note.GNU-stack, "", @progbits
