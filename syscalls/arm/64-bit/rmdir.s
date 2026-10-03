.section .data
rmdir_dir_path_pointer
.string ""

.global syscall_rmdir_arm_64
syscall_rmdir_arm_64:
    mov x8, #35
    mov x1, #rmdir_dir_path_pointer
    mov x2, #0x200
    svc #0
    ret
.section .note.GNU-stack, "", @progbits
