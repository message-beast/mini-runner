global getdents64_x86_32

getdents64_x86_32:
    push ebp
    mov ebp, esp
    mov eax, 220
    mov ebx, [ebp + 16]
    mov ecx, [ebp + 12]
    mov edx, [ebp + 8]
    int 0x80
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits