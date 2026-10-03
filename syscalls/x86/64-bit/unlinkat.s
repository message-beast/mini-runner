.global syscall_unlinkat_x86_64
.intel_syntax noprefix
syscall_unlinkat_x86_64:
    mov rax, 263
    syscall
    ret


.section .note.GNU-stack, "", @progbits
