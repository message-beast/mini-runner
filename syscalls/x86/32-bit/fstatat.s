global syscall_fstatat_x86_32

syscall_fstatat_x86_32:
    push ebp
    mov ebp, esp
    mov eax, 300
    mov ebx, [ebp + 20]
    mov ecx, [ebp + 16]
    mov edx, [ebp + 12]
    mov esi, [ebp + 8]
    int 0x80
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits