.global syscall_getdents64_x86_64
.intel_syntax noprefix
syscall_getdents64_x86_64:
    mov rax, 217
    syscall
    ret
.section .note.GNU-stack, "", @progbits
