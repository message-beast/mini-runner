.global syscall_rmdir_x86_64
.intel_syntax noprefix
syscall_rmdir_x86_64:
    mov rax, 84
    syscall
    ret


.section .note.GNU-stack, "", @progbits
