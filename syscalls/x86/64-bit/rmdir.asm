global syscall_rmdir_x86_64

syscall_rmdir_x86_64:
    mov rax, 84
    syscall
    ret


section .note.GNU-stack noalloc noexec nowrite progbits