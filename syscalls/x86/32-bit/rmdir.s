global syscall_rmdir_x86_32

syscall_rmdir_x86_32:
    push ebp
    mov ebp, esp
    mov eax, 40
    mov ebx, [ebp + 8]
    int 0x80
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits