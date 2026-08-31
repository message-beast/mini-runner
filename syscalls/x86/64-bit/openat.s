global syscall_openat_x86_64

syscall_openat_x86_64:
    mov rax, 257
    mov r10, rcx
    syscall
    ret

section .note.GNU-stack noalloc noexec nowrite progbits