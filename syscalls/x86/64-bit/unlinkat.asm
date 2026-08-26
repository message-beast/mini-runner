global syscall_unlinkat_x86_64

syscall_unlinkat_x86_64:
    mov rax, 263
    syscall
    ret


section .note.GNU-stack noalloc noexec nowrite progbits