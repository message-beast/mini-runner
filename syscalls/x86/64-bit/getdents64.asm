global syscall_getdents64_x86_64

syscall_getdents64_x86_64:
    mov rax, 217
    syscall
    ret


section .note.GNU-stack noalloc noexec nowrite progbits