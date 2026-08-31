global syscall_fstatat_x86_64

syscall_fstatat_x86_64:
    mov rax, 262
    mov r10, rcx
    syscall
    ret

section .note.GNU-stack noalloc noexec nowrite progbits