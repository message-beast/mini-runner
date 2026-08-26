compile-close:
	nasm -felf64 close.asm -o close.o
compile-fstatat:
	nasm -felf64 fstatat.asm -o fstat.o
compile-getdents64:
	nasm -felf64 getdents64.asm -o getdents64.o
compile-openat:
	nasm -felf64 openat.asm -o openat.o
compile-rmdir:
	nasm -felf64 rmdir.asm -o rmdir.o
compile-unlinkat:
	nasm -felf64 unlinkat.asm -o unlinkat.o
compile-all:
	make compile-close && make compile-fstatat && make compil-getdents64 && make compile-openat && make compile-rmdir && make compile-unlinkat