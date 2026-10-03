.global syscall_close_x86_64
.intel_syntax noprefix
syscall_close_x86_64:
    mov rax, 3
    mov r10, rcx
    syscall
    ret
.section .note.GNU-stack, "", @progbits
